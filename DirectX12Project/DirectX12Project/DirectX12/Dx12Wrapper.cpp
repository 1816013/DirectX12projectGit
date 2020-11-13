#include "Dx12Wrapper.h"
#include <Windows.h>
#include <cassert>
#include <d3dcompiler.h>
#include <string>
#include <random>
#include <DirectXTex.h>
#include <stdint.h>
#include "../Application.h"
#include "../PMDLoder/PMDLoder.h"
#include "../Common/StrOperater.h"
#include "../Common.h"
#include "PMDResource.h"
#include "PMDActor.h"
//#include "../BMPLoder/BmpLoder.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DxGuid.lib")

using namespace DirectX;
using namespace std;

namespace
{
	/// <summary>
	/// 頂点構造体
	/// </summary>
	/*struct Vertex
	{
		XMFLOAT3 vertex;
		XMFLOAT2 uv;
		Vertex(XMFLOAT3 vert, XMFLOAT2 tuv) : vertex(vert), uv(tuv) {};
	};*/
	//vector<PMDVertex> vertices_;
	//vector<unsigned short>indices_;

	constexpr int minTexSize = 4;
	constexpr int maxTexHeight = 256;
	
	/// <summary>
	/// alignの倍数にした値を返す
	/// </summary>
	/// <param name="value">値</param>
	/// <param name="align">align値</param>
	/// <returns>alignの倍数にした値</returns>
	UINT AligndValue(UINT value, UINT align)
	{
		//return (value + (align - 1)) &~ (align-1);
		return value + (align - (value % align)) % align;
	};
	// 中間バッファ一時保持用
	vector<ComPtr<ID3D12Resource>>intermediateBuffList;

}

void CreateVertices()
{
	//// 手前
	//vertices_.push_back(Vertex({ -100.0f, -100.0f,100 }, { 0.0f,1.0f })); //左上
	//vertices_.push_back(Vertex({ -100.0f, 100.0f, 100}, { 0.0f,0.0f }));  //左下
	//vertices_.push_back(Vertex({ 100.0f,-100.0f,100 }, { 1.0f,1.0f }));	  //右上
	//vertices_.push_back(Vertex({ 100.0f, 100.0f,100 }, { 1.0f,0.0f }));	  //右下

	//// 奥
	//vertices_.push_back(Vertex({ -100.0f, -100.0f,-100 }, { 1.0f,1.0f }));	//左上
	//vertices_.push_back(Vertex({ -100.0f, 100.0f, -100 }, { 1.0f,0.0f }));	//左下
	//vertices_.push_back(Vertex({ 100.0f, -100.0f,-100 }, { 0.0f,1.0f }));	//右上
	//vertices_.push_back(Vertex({ 100.0f, 100.0f,-100 }, { 0.0f,0.0f }));	//右下


	//	// 上
	//vertices_.push_back(Vertex({ 100.0f, 100.0f,100 }, { 0.0f,1.0f }));	//左上
	//vertices_.push_back(Vertex({ -100.0f, 100.0f, 100 }, { 0.0f,0.0f }));	//左下
	//vertices_.push_back(Vertex({ 100.0f, 100.0f,-100 }, { 1.0f,1.0f }));	//右上
	//vertices_.push_back(Vertex({ -100.0f, 100.0f,-100 }, { 1.0f,0.0f }));	//右下
	//// 下
	//vertices_.push_back(Vertex({ -100.0f, -100.0f,100 }, { 0.0f,1.0f }));	//左上
	//vertices_.push_back(Vertex({ 100.0f, -100.0f, 100 }, { 0.0f,0.0f }));	//左下
	//vertices_.push_back(Vertex({ -100.0f, -100.0f,-100 }, { 1.0f,1.0f }));	//右上
	//vertices_.push_back(Vertex({ 100.0f, -100.0f,-100 }, { 1.0f,0.0f }));	//右下


}

void CreateIndices()
{
	//indices_ = { 0, 1, 2, 2, 1, 3 ,	// 前面
	//			 2, 3, 6, 6, 3, 7,	// 右面
	//			 6, 7, 4, 4, 7, 5,	// 裏面
	//			 4, 5, 0, 0, 5, 1,	// 左面
	//			 8, 9, 10, 10, 9, 11,// 上面
	//			 12, 13, 14, 14, 13, 15 // 下面
	//			 };
}

void Dx12Wrapper::CreateVertexBuffer()
{
	const auto& vertices = pmdActor_->GetPMDModel().GetVertexData();
	vertexBuffer_ = CreateBuffer(vertices.size() * sizeof(vertices[0]));
	//頂点データ転送
	PMDVertex* mappedData = nullptr;
	auto result = vertexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(vertices.begin(), vertices.end(), mappedData);
	vertexBuffer_->Unmap(0, nullptr);

	// 頂点バッファビュー
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices[0]);
	vbView_.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(vertices[0]));
}

void Dx12Wrapper::CreateIndexBuffer()
{
	const auto& indices = pmdActor_->GetPMDModel().GetIndexData();
	indexBuffer_ = CreateBuffer(indices.size() * sizeof(indices[0]));
	//インデックスデータ転送
	auto forType = indices.back();
	decltype(forType)* mappedData = nullptr;
	auto result = indexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(indices.begin(), indices.end(), mappedData);
	indexBuffer_->Unmap(0, nullptr);

	// インデックスビュー
	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(indices[0]));
}

bool Dx12Wrapper::CreateTexture(const std::wstring& path, ComPtr<ID3D12Resource>& res)
{
	/*auto it = textureResource_.find(path);
	if (it != textureResource_.end())
	{
		res = it->second;
		return true;
	}*/
	HRESULT result = S_OK;
	TexMetadata metaData= {};
	ScratchImage scratchImg = {};
	if (StrOperater::GetExtension(path) != L"tga")
	{
		result = LoadFromWICFile(path.c_str(), WIC_FLAGS_IGNORE_SRGB, &metaData, scratchImg);		
	}
	else
	{
		result = LoadFromTGAFile(path.c_str(), TGA_FLAGS_IGNORE_SRGB, &metaData, scratchImg);
	}
	if (FAILED(result))
	{
		return false;
	}
	assert(SUCCEEDED(result));
	auto img = scratchImg.GetImage(0, 0, 0);// 生データ抽出

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = metaData.format;
	resDesc.Width = metaData.width;
	resDesc.Height = static_cast<UINT>(metaData.height);
	resDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	resDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	/*resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;*/

	result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&res));
	assert(SUCCEEDED(result));
	
	//BmpLoder bmp("Resource/image/sample.bmp");
	//auto bSize = bmp.GetBmpSize();
	//auto& rawData = bmp.GetRawData();
	//// テクスチャデータ作成※本来は外部から読み込む
	//vector<uint8_t>texData(4 * bSize.width * bSize.height);
	//int texIdx = 0;
	//for (int y = bSize.height - 1; y > 0; --y)
	//{
	//	for (int x = 0; x < bSize.width; ++x)
	//	{
	//		texData[texIdx++] = rawData[(x + y * bSize.width) * 3 + 0];
	//		texData[texIdx++] = rawData[(x + y * bSize.width) * 3 + 1];
	//		texData[texIdx++] = rawData[(x + y * bSize.width) * 3 + 2];
	//		texData[texIdx++] = 0xff;
	//	}
	//}
	//D3D12_BOX box;
	//box.left = 0;
	//box.right = metaData.width;
	//box.top = 0;
	//box.bottom = metaData.height;
	//box.front = 0;
	//box.back = 1;
	result = res->WriteToSubresource(0,
		nullptr,
		img->pixels,	// 元データアドレス
		static_cast<UINT>(img->rowPitch),	// 1ラインサイズ
		static_cast<UINT>(img->slicePitch)	// 1枚サイズ
	);
	assert(SUCCEEDED(result));
	textureResource_[path] = res.Get();
	return true;
}

bool Dx12Wrapper::CreateMonoColorTexture(ComPtr<ID3D12Resource>& tex, const Color col)
{	
	HRESULT result = S_OK;
	Size size = { minTexSize, minTexSize };
	std::vector<Color>texData(size.width * size.height);
	
	std::fill(texData.begin(), texData.end(), col);	// 全部0xffで初期化
	
	D3D12_SUBRESOURCE_DATA subResData = {};
	subResData.pData = texData.data();
	subResData.RowPitch = sizeof(texData[0]) * size.width;
	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
	SetUploadTexure(tex, subResData);
	
	return true;
}
bool Dx12Wrapper::CreateGradationTexture(ComPtr<ID3D12Resource>& tex, const Size size)
{
	std::vector<Color>texData(size.width * size.height);
	for (size_t i = 0; i < 256; ++i)
	{
		fill_n(&texData[i * 4], 4, Color(255 - i));	// rgb全部0x00で初期化
	}
	D3D12_SUBRESOURCE_DATA subResData = {};
	subResData.pData = texData.data();
	subResData.RowPitch = sizeof(texData[0]) * size.width;
	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
	SetUploadTexure(tex, subResData);
	return true;
}

void Dx12Wrapper::SetUploadTexure(ComPtr<ID3D12Resource>& tex, D3D12_SUBRESOURCE_DATA& subResData)
{
	// 転送先
	auto width = subResData.RowPitch / sizeof(Color);
	auto height = subResData.SlicePitch / subResData.RowPitch;
	
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	auto result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(tex.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// 転送元
	ComPtr<ID3D12Resource>intermediateBuff;	// 中間バッファ
	auto buffSize = GetRequiredIntermediateSize(tex.Get(), 0, 1);
	result = dev_->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(intermediateBuff.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	intermediateBuffList.push_back(intermediateBuff);
	
	// コマンドリストに登録
	// 中でCopyTextureRegionが走っているため
	// コマンドキュー待ちが必要
	UpdateSubresources(cmdList_.Get(), tex.Get(),
		intermediateBuff.Get(), 0, 0, 1, &subResData);
	cmdList_->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(tex.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

bool Dx12Wrapper::CreateDefaultTextures()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), pipelineState_.Get());
	// cmdListにテクスチャデータを積む
	CreateMonoColorTexture(whiteTex_, Color(0xff));	// 白
	CreateMonoColorTexture(blackTex_, Color(0x00));	// 黒
	CreateGradationTexture(gradTex_, { minTexSize ,256 });	// グラデ
	cmdList_->Close();
	ExecuteAndWait();
	intermediateBuffList.clear();
	return true;
}

bool Dx12Wrapper::CreateBasicDescriptors()
{
	//// 座標変換用ディスクリプタヒープ
	//D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	//desc.NumDescriptors = 1;
	//desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//desc.NodeMask = 0;
	//auto result = dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&resViewHeap_));
	//assert(SUCCEEDED(result));

	//auto cbDesc = transformBuffer_->GetDesc();
	//D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	//cbvDesc.BufferLocation = transformBuffer_->GetGPUVirtualAddress();
	//cbvDesc.SizeInBytes = static_cast<UINT>(cbDesc.Width);
	//dev_->CreateConstantBufferView(&cbvDesc, resViewHeap_->GetCPUDescriptorHandleForHeapStart());

	pmdResource_->Reset(GroopType::TRANSFORM);
	auto& transResBind = pmdResource_->GetGroops(GroopType::TRANSFORM);
	transResBind.Init(dev_.Get(), { BuffType::CBV });
	transResBind.AddBuffers(transformBuffer_.Get());
	return true;
}

bool Dx12Wrapper::CreateTransformBuffer()
{
	transformBuffer_ = CreateBuffer(AligndValue(sizeof(XMMATRIX), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
	auto wSize = Application::GetInstance().GetWindowSize();
	/*XMFLOAT4X4 tempMat = {};
	tempMat._11 = 1.0f / (static_cast<float>(wSize.width) / 2.0f);
	tempMat._22 = 1.0f / (static_cast<float>(wSize.height) / 2.0f);
	tempMat._33 = 1.0f;
	tempMat._44 = 1.0f;
	tempMat._41 = -1.0f;
	tempMat._42 = 1.0f;*/

	XMMATRIX world = XMMatrixIdentity();

	// 2D表示
	//tmpMat.r[0].m128_f32[0] = 1.0f / (static_cast<float>(wSize.width) / 2.0f);
	//tmpMat.r[1].m128_f32[1] = -1.0f / (static_cast<float>(wSize.height) / 2.0f);
	//tmpMat.r[3].m128_f32[0] = -1.0f;
	//tmpMat.r[3].m128_f32[1] = 1.0f;
	// ここまで2D表示

	// 3D表示
	// ワールド行列(モデル自身に対する変換)
	world *= XMMatrixRotationY(XM_PI);

	// カメラ行列(ビュー行列)
	XMMATRIX viewproj = XMMatrixLookAtRH(
		{ 0.0f, 10.0f, 30.0f, 1.0f },	// 視点
		{ 0.0f, 10.0f, 0.0f, 1.0f },		// 注視店
		{ 0.0f, 1.0f, 0.0f,1.0f });		// 上(仮の上)

	// プロジェクション行列(パースペクティブ行列or射影行列)
	viewproj *= XMMatrixPerspectiveFovRH(XM_PIDIV4 , // 画角(FOV)
		static_cast<float>(wSize.width) / static_cast<float>(wSize.height), 
		0.1f,	// ニア(近い)
		300.0f);	//　ファー(遠い)

	mappedBasicMatrix_ = make_shared<BasicMatrix>();
	// 後でいじるために開けっ放しにしておく
	transformBuffer_->Map(0, nullptr, (void**)&mappedBasicMatrix_);
	
	mappedBasicMatrix_->viewproj = viewproj;
	mappedBasicMatrix_->world = world;
	return true;
}

bool Dx12Wrapper::CreateDepthBufferView()
{
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	auto rtvDesc = bbResouces[0]->GetDesc();

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		rtvDesc.Width,
		rtvDesc.Height);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	CD3DX12_CLEAR_VALUE clearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	/*D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;*/

	auto result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&depthBuffer_));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC desDesc = {};
	desDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desDesc.NumDescriptors = 1;
	desDesc.NodeMask = 0;
	desDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = dev_->CreateDescriptorHeap(&desDesc,
		IID_PPV_ARGS(&depthDescHeap_));
	assert(SUCCEEDED(result));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	dev_->CreateDepthStencilView(depthBuffer_.Get(),
		&dsvDesc,
		depthDescHeap_->GetCPUDescriptorHandleForHeapStart());
	return true;
}

bool Dx12Wrapper::CreateMaterialBufferView()
{
	// マテリアルバッファの作成
	HRESULT result = S_OK;
	auto& mats = pmdActor_->GetPMDModel().GetMaterialData();
	auto strideBytes = AligndValue(sizeof(BasicMaterial), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	materialBuffer_ = CreateBuffer(mats.size() * strideBytes/* + texBuffers_.size()*/);
	auto gAddress = materialBuffer_->GetGPUVirtualAddress();

	// マテリアルディスクリプタ作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NodeMask = 0;
	// マテリアルCBV + 通常SRV + 乗算スフィアマップSRV 
	// +  加算スフィアマップSRV + toonマップ  = 5
	heapDesc.NumDescriptors = static_cast<UINT>(mats.size() * 5); 
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = dev_->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&materialDescHeap_));
	assert(SUCCEEDED(result));
	//auto gAddress = materialBuffer_->GetGPUVirtualAddress();
	auto heapSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto heapAddress = materialDescHeap_->GetCPUDescriptorHandleForHeapStart();

	// マテリアル定数バッファ用ディスク
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.SizeInBytes = strideBytes;
	cbvDesc.BufferLocation = gAddress;

	// テクスチャ用ディスク
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);

	uint8_t* mappedMaterial = nullptr;
	result = materialBuffer_->Map(0, nullptr, (void**)&mappedMaterial);
		assert(SUCCEEDED(result));
	for (int i = 0; i < mats.size(); ++i)
	{		
		// マテリアル定数バッファビュー
		((BasicMaterial*)mappedMaterial)->diffuse = mats[i].diffuse;
		((BasicMaterial*)mappedMaterial)->ambient = mats[i].ambient;
		((BasicMaterial*)mappedMaterial)->speqular = mats[i].speqular;
		((BasicMaterial*)mappedMaterial)->alpha = mats[i].alpha;
		((BasicMaterial*)mappedMaterial)->speqularity = mats[i].speqularity;

		cbvDesc.BufferLocation = gAddress;
		dev_->CreateConstantBufferView(
			&cbvDesc,
			heapAddress
		);
		mappedMaterial += strideBytes;
		gAddress += strideBytes;
		heapAddress.ptr += heapSize;

		// テクスチャビュー
		CreateSRVView(srvDesc, texBuffers_[i], whiteTex_, heapAddress, heapSize);

		// 乗算スフィアマップビュー
		CreateSRVView(srvDesc, sphBuffers_[i], whiteTex_, heapAddress, heapSize);
		
		// 加算スフィアマップ
		CreateSRVView(srvDesc, spaBuffers_[i], blackTex_, heapAddress, heapSize);

		// トゥーンマップ
		CreateSRVView(srvDesc, toonBuffers_[i], gradTex_, heapAddress, heapSize);
	}
	//uint8_t* mappedMaterial = nullptr;
	//result = materialBuffer_->Map(0, nullptr, (void**)&mappedMaterial);
	//	assert(SUCCEEDED(result));
	//materialBuffer_->Unmap(0, nullptr);
	//pmdResource_->Reset(GroopType::MATERIAL);
	//auto& transResBind = pmdResource_->GetGroops(GroopType::MATERIAL);
	//transResBind.Init(dev_.Get(), { BuffType::CBV, BuffType::SRV, BuffType::SRV, BuffType::SRV, BuffType::SRV });
	//for (int i = 0; i < mats.size(); ++i)
	//{
	//	// マテリアル定数バッファビュー
	//	((BasicMaterial*)mappedMaterial)->diffuse = mats[i].diffuse;
	//	((BasicMaterial*)mappedMaterial)->ambient = mats[i].ambient;
	//	((BasicMaterial*)mappedMaterial)->speqular = mats[i].speqular;
	//	((BasicMaterial*)mappedMaterial)->alpha = mats[i].alpha;
	//	((BasicMaterial*)mappedMaterial)->speqularity = mats[i].speqularity;

	//	/*cbvDesc.BufferLocation = gAddress;
	//	dev_->CreateConstantBufferView(
	//		&cbvDesc,
	//		heapAddress
	//	);*/
	//	
	//	//heapAddress.ptr += heapSize;
	//	transResBind.AddBuffers(materialBuffer_.Get());
	//	mappedMaterial += strideBytes;
	//	gAddress += strideBytes;
	//	transResBind.AddBuffers(texBuffers_[i].Get());
	//	transResBind.AddBuffers(sphBuffers_[i].Get());
	//	transResBind.AddBuffers(spaBuffers_[i].Get());
	//	transResBind.AddBuffers(toonBuffers_[i].Get());
	//}
	return true;
}

void Dx12Wrapper::CreateSRVView(
	D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc,
	ComPtr<ID3D12Resource>&buffer,
	ComPtr<ID3D12Resource>& defaultTex,
	D3D12_CPU_DESCRIPTOR_HANDLE& heapAddress, 
	const UINT& heapSize)
{
	// フォーマット初期化忘れないように
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (buffer != nullptr)
	{
		srvDesc.Format = buffer->GetDesc().Format;
	}
	dev_->CreateShaderResourceView(
		buffer != nullptr ? buffer.Get() : defaultTex.Get(),
		&srvDesc,
		heapAddress);
	heapAddress.ptr += heapSize;
}

bool Dx12Wrapper::CreatePipelineState()
{
	HRESULT result = S_OK;
	// パイプラインステートデスク
	D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	// IA(InputAssembler)
	// 入力レイアウト
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		// 頂点情報
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// 頂点情報
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// UV情報
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// 頂点シェーダ
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/vs.hlsl",
		nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_1",
		0,
		0, vsBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));	
	plsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());

	// ピクセルシェーダ
	ComPtr<ID3DBlob> psBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/ps.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_1",
		0,
		0, psBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	plsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// ラスタライザ設定
	plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// その他設定
	// デプスとステンシル設定
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 出力設定
	plsDesc.NumRenderTargets = 1;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ブレンド
	plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	
	// ルートシグネチャ生成
	CreateRootSignature(plsDesc);
	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(&pipelineState_));
	return true;
}

void Dx12Wrapper::CreateRootSignature(D3D12_GRAPHICS_PIPELINE_STATE_DESC& plsDesc)
{
	HRESULT result = S_OK;
	// ルートシグネチャ
	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};

	D3D12_ROOT_PARAMETER rp[2] = {};
	D3D12_DESCRIPTOR_RANGE range[3] = {};

	// 行列定数バッファ
	range[0] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // レンジタイプ b
		1,// デスクリプタ数	b0～b0まで
		0);// ベースレジスタ番号 b0	

	// マテリアル
	range[1] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // レンジタイプ b
		1,// デスクリプタ数	b1～b1まで
		1);// ベースレジスタ番号 b1

	// テクスチャ
	range[2] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // レンジタイプ b
		4,// デスクリプタ数	t0～t3まで
		0);// ベースレジスタ番号 t0

	// 座標変換
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[0],	// ルートパラメータ
		1,		// レンジ数
		&range[0]);	// レンジ先頭アドレス
	// マテリアル&テクスチャ
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[1],	// ルートパラメータ
		2,	// レンジ数
		&range[1],	// レンジ先頭アドレス
		D3D12_SHADER_VISIBILITY_PIXEL);	// どのシェーダで使うか

	// s0を定義
	// サンプラの定義、サンプラはuvが0未満や1越えとかの時や
	// UVをもとに色をとってくるときのルールを指定するもの
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	samplerDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	samplerDesc[1] = CD3DX12_STATIC_SAMPLER_DESC(1);
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	rsDesc.Init(_countof(rp), rp, _countof(samplerDesc), samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// シグネチャ設定
	ComPtr<ID3DBlob> errBlob = nullptr;
	ComPtr<ID3DBlob> sigBlob = nullptr;
	D3D12SerializeRootSignature(&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sigBlob,
		&errBlob);
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	// ルートシグネチャの生成
	result = dev_->CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSig_));
	assert(SUCCEEDED(result));
	plsDesc.pRootSignature = rootSig_.Get();

	
	assert(SUCCEEDED(result));
}

void Dx12Wrapper::OutputFromErrorBlob(ID3DBlob* errBlob)
{
	if (errBlob != nullptr)
	{
		string errStr = "";
		auto errSize = errBlob->GetBufferSize();
		errStr.resize(errSize);
		copy_n((char*)errBlob->GetBufferPointer(), errSize, errStr.begin());
		OutputDebugStringA(errStr.c_str());
	}
}

ComPtr<ID3D12Resource> Dx12Wrapper::CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType)
{
	ComPtr<ID3D12Resource>ret;
	CD3DX12_HEAP_PROPERTIES heapProp(heapType);

	// 確保する用途(Resource)に関する設定
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	auto result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	return ret;
}

Dx12Wrapper::Dx12Wrapper()
{
	
}

Dx12Wrapper::~Dx12Wrapper()
{
}

bool Dx12Wrapper::Init(HWND hwnd)
{
	HRESULT result = S_OK;
#if _DEBUG
	ComPtr<ID3D12Debug3> debug = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf()));
	debug->EnableDebugLayer();
#endif
	CheckFeatureLevel();
#if _DEBUG
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgi_.ReleaseAndGetAddressOf()));
#else 
	result = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgi_));
#endif
	assert(SUCCEEDED(result));

	InitCommandSet();
	
	pmdActor_ = make_shared<PMDActor>(dev_.Get());
	//const char* modelPath = "Resource/PMD/桜ミク/mikuXS雪ミク.pmd";
	//const char* modelPath = "Resource/PMD/雲雀/雲雀Ver1.10SW.pmd";
	//const char* modelPath = "Resource/PMD/model/初音ミク.pmd";
	//const char* modelPath = "Resource/PMD/我那覇響v1.0/我那覇響v1.pmd";
	//const char* modelPath = "Resource/PMD/古明地さとり/古明地さとり152Normal.pmd";
	const char* modelPath = "Resource/PMD/霊夢/reimu_F02.pmd";
	pmdActor_->LoadModel(modelPath);
	
	CreateSwapChain(hwnd);
	
	CreateFence();

	// レンダーターゲットを作成
	CreateRenderTargetDescriptorHeap();
	// 深度バッファビュー作成
	CreateDepthBufferView();

	// 頂点バッファを作成
	CreateVertices();
	CreateVertexBuffer();

	// インデックスバッファを作成
	CreateIndices();
	CreateIndexBuffer();

	// 定数バッファ作成
	CreateTransformBuffer();

	// テクスチャ作成
	CreatePMDModelTexture();
	//pmdActor_->CreatePMDModelTexture();
	// 基本テクスチャ作成
	CreateDefaultTextures();

	pmdResource_ = make_shared<PMDResource>(dev_.Get());
	// マテリアルバッファの作成
	CreateMaterialBufferView();
	
	// 座標変換SRV用ディスクリプタヒープ作成
	CreateBasicDescriptors();

	pmdResource_->Build({ GroopType::TRANSFORM,/* GroopType::MATERIAL*/ });

	if (!CreatePipelineState())
	{
		return false;
	}
	// ビューポートとシザー矩形初期化
	InitViewRect();

	return true;
}

void Dx12Wrapper::CreatePMDModelTexture()
{
	auto modelPath = pmdActor_->GetPMDModel().GetModelPath();
	auto& paths = pmdActor_->GetPMDModel().GetTexturePaths();
	auto& toonPaths = pmdActor_->GetPMDModel().GetToonPaths();
	texBuffers_.resize(paths.size());
	sphBuffers_.resize(paths.size());
	spaBuffers_.resize(paths.size());
	toonBuffers_.resize(toonPaths.size());

	for (int i = 0; i < paths.size(); ++i)
	{
		if (toonPaths[i] != "")
		{
			string strToonPath = StrOperater::GetTextureFromModelAndTexPath(modelPath, toonPaths[i]);

			if (!CreateTexture(
				StrOperater::GetWideStringfromString(strToonPath),
				toonBuffers_[i]))
			{
				strToonPath = "Resource/PMD/toon/" + toonPaths[i];
				CreateTexture(
					StrOperater::GetWideStringfromString(strToonPath),
					toonBuffers_[i]);
			}
		}
		if (paths[i] == "")
		{
			continue;
		}

		auto pathVec = StrOperater::SplitFileName(paths[i]);
		for (auto path : pathVec)
		{
			auto ext = StrOperater::GetExtension(path);
			if (ext == "spa")
			{
				auto str = StrOperater::GetTextureFromModelAndTexPath(modelPath, path);
				CreateTexture(
					StrOperater::GetWideStringfromString(str),
					spaBuffers_[i]);
				continue;
			}
			if (ext == "sph")
			{
				auto str = StrOperater::GetTextureFromModelAndTexPath(modelPath, path);
				CreateTexture(
					StrOperater::GetWideStringfromString(str),
					sphBuffers_[i]);
				continue;
			}
			auto str = StrOperater::GetTextureFromModelAndTexPath(modelPath, path);
			auto result = CreateTexture(
				StrOperater::GetWideStringfromString(str),
				texBuffers_[i]);
			assert(result);

		}
	}
}

bool Dx12Wrapper::InitViewRect()
{
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	// ビューポート	
	viewPort_.TopLeftX = 0;
	viewPort_.TopLeftY = 0;
	viewPort_.Width = static_cast<FLOAT>(wSize.width);
	viewPort_.Height = static_cast<FLOAT>(wSize.height);
	viewPort_.MaxDepth = 1.0f;
	viewPort_.MinDepth = 0.0f;

	// シザー矩形の設定
	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = static_cast<LONG>(wSize.width);
	scissorRect_.bottom = static_cast<LONG>(wSize.height);

	return true;
}

bool Dx12Wrapper::Update()
{
	// モデルを動かす
	static float modelY = 0.0f;
	static float angle = 0.0f;
	BYTE keyState[256];
	auto result = GetKeyboardState(keyState);
	if (keyState[VK_UP] & 0x80)
	{
		modelY += 0.1f;
	}
	if( keyState[VK_DOWN] & 0x80)
	{
		modelY -= 0.1f;
	}
	angle += 0.02f;
	mappedBasicMatrix_->world = XMMatrixRotationY(angle);
	mappedBasicMatrix_->world *= XMMatrixTranslation(0, modelY, 0);

	// 画面クリア
	//ClearDrawScreen();

	// 描画処理
	DrawPMDModel();

	// バッファフリップ
	//DrawExcute();
	return true;
}

void Dx12Wrapper::DrawExcute()
{
	// リソースバリアを設定レンダーターゲットからプレゼント
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		bbResouces[bbIdx_].Get(),	// リソース
		D3D12_RESOURCE_STATE_RENDER_TARGET,	// 前ターゲット
		D3D12_RESOURCE_STATE_PRESENT	// 後ろターゲット
	);
	cmdList_->ResourceBarrier(1, &barrier);
	cmdList_->Close();

	ExecuteAndWait();
	swapchain_->Present(1, 0);
}

void Dx12Wrapper::ClearDrawScreen()
{
	// バックバッファインデックス設定
	bbIdx_ = swapchain_->GetCurrentBackBufferIndex();

	// コマンドリセット
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), pipelineState_.Get());
	// リソースバリアを設定プレゼントからレンダーターゲット
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		bbResouces[bbIdx_].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	cmdList_->ResourceBarrier(1, &barrier);

	// レンダーターゲットをセット	
	auto rtvHeap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto rtvIncSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHeap.ptr += static_cast<SIZE_T>(bbIdx_)* rtvIncSize;
	cmdList_->OMSetRenderTargets(1, &rtvHeap, false, &depthDescHeap_->GetCPUDescriptorHandleForHeapStart());
	// 画面をクリア(色変える)
	float clsCol[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	cmdList_->ClearRenderTargetView(rtvHeap, clsCol, 0, nullptr);
	cmdList_->ClearDepthStencilView(
		depthDescHeap_->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);

	// シグネチャセット
	cmdList_->SetGraphicsRootSignature(rootSig_.Get());
	// ビューポートとシザー矩形の設定
	CD3DX12_VIEWPORT vp(bbResouces[bbIdx_].Get());	// これでできるが分割できない
	cmdList_->RSSetViewports(1, &vp);
	cmdList_->RSSetScissorRects(1, &scissorRect_);
}

void Dx12Wrapper::DrawPMDModel()
{
	// 座標変換ヒープセット
	auto resHeap = pmdResource_->GetGroops(GroopType::TRANSFORM).descHeap_.Get();
	ID3D12DescriptorHeap* deskHeaps[] = { resHeap/*resViewHeap_.Get()*/ };
	auto heapPos = /*resViewHeap_*/resHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, heapPos);
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	//cmdList_->DrawInstanced(vertices_.size(), 1, 0, 0);
	cmdList_->IASetIndexBuffer(&ibView_);
	auto material = pmdActor_->GetPMDModel().GetMaterialData();
	uint32_t indexOffset = 0;

	// マテリアル&テクスチャヒープセット
	//auto matHeap = pmdResource_->GetGroops(GroopType::MATERIAL).descHeap_.Get();

	ID3D12DescriptorHeap* matDeskHeaps[] = { materialDescHeap_.Get() };
	cmdList_->SetDescriptorHeaps(1, matDeskHeaps);
	auto materialHeapPos = materialDescHeap_->GetGPUDescriptorHandleForHeapStart();
	const auto heapSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (auto& m : material)
	{
		cmdList_->SetGraphicsRootDescriptorTable(1, materialHeapPos);
		auto indexNum = m.indexNum;

		cmdList_->DrawIndexedInstanced(
			indexNum,		// インデックス数
			1,				// インスタンス数
			indexOffset,	// インデックスオフセット
			0,				// 頂点オフセット
			0);				// インスタンスオフセット
		indexOffset += indexNum;
		materialHeapPos.ptr += static_cast<UINT64>(heapSize) * 5;
	}
}

void Dx12Wrapper::ExecuteAndWait()
{
	ID3D12CommandList* cmdLists[] = { cmdList_.Get() };
	cmdQue_->ExecuteCommandLists(1, cmdLists);
	cmdQue_->Signal(fence_.Get(), ++fenceValue_);
	// Execute完了まで待つ処理
	while (true)
	{
		if (fence_->GetCompletedValue() == fenceValue_)
		{
			break;
		}
	}
	
}

void Dx12Wrapper::Terminate()
{
}

ID3D12Device* Dx12Wrapper::GetDevice()
{
	return dev_.Get();
}

bool Dx12Wrapper::CheckFeatureLevel()
{
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_1_0_CORE;

	for (auto lv : levels)
	{
		auto result = D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf()));
		if (SUCCEEDED(result))
		{
			level = lv;
			break;
		}
	}
	if (level == D3D_FEATURE_LEVEL_1_0_CORE)
	{
		OutputDebugString(L"feature level not found");
		return false;
	}
	return true;
}

bool Dx12Wrapper::InitCommandSet()
{
	// コマンドキュー作成
	D3D12_COMMAND_QUEUE_DESC cmdQDesc = {};
	cmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQDesc.NodeMask = 0;
	cmdQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	auto result = dev_->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(cmdQue_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// コマンドアロケーターの作成
	dev_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdAllocator_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// コマンドリストの作成
	dev_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(cmdList_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	cmdList_->Close();
	return true;
}

bool Dx12Wrapper::CreateSwapChain(const HWND& hwnd)
{
	// swapchainDesc作成
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = static_cast<UINT>(wSize.width);
	scDesc.Height = static_cast<UINT>(wSize.height);
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferCount = 2;	// 表と裏画面
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.Flags = 0/*DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH*/;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Stereo = false;	// VRの時true
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ComPtr<IDXGISwapChain1>swapchain;
	// swapchain作成
	auto result = dxgi_->CreateSwapChainForHwnd(cmdQue_.Get(),
		hwnd,
		&scDesc,
		nullptr,
		nullptr,
		&swapchain);
	assert(SUCCEEDED(result));

	result = swapchain.As(&swapchain_);
	assert(SUCCEEDED(result));
	return true;
}

bool Dx12Wrapper::CreateFence()
{
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	// フェンスを作る(スレッドセーフに必要)
	dev_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()));
	fenceValue_ = fence_->GetCompletedValue();
	return true;
}

bool Dx12Wrapper::CreateRenderTargetDescriptorHeap()
{
	HRESULT result = S_OK;
	// 表示画面用メモリ確保
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	desc.NodeMask = 0;
	desc.NumDescriptors = 2;// 表と裏画面用
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap_));
	assert(SUCCEEDED(result));

	// レンダーターゲットを設定
	DXGI_SWAP_CHAIN_DESC1 swDesc;
	result = swapchain_->GetDesc1(&swDesc);
	assert(SUCCEEDED(result));
	int num_rtvs = swDesc.BufferCount;	
	bbResouces.resize(num_rtvs);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = swDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	auto heap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto incSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (int i = 0; i < num_rtvs; i++)
	{
		swapchain_->GetBuffer(i, IID_PPV_ARGS(&bbResouces[i]));//「キャンバス」を取得
		dev_->CreateRenderTargetView(bbResouces[i].Get(), &rtvDesc, heap);	// キャンパスと職人を紐づける
		heap.ptr += incSize;// 職人とキャンバスのペアのぶん次のところまでオフセット
	}

	return SUCCEEDED(result);
}
