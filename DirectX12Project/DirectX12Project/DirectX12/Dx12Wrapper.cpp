#include "Dx12Wrapper.h"
#include <Windows.h>
#include <cassert>
#include <d3dcompiler.h>
#include <string>
#include <random>
#include <DirectXTex.h>
#include <stdint.h>
#include <algorithm>
#include "../Application.h"
#include "../PMDLoder/PMDLoder.h"
#include "../Common/StrOperater.h"
#include "../Common.h"
#include "PMDResource.h"
#include "PMDActor.h"
#include "../PMDLoder/VMDMotion.h"
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

bool Dx12Wrapper::CreateMonoColorTexture(ColTexType colType, const Color col)
{	
	HRESULT result = S_OK;
	Size size = { minTexSize, minTexSize };
	std::vector<Color>texData(size.width * size.height);
	
	std::fill(texData.begin(), texData.end(), col);	// 全部0xffで初期化
	
	D3D12_SUBRESOURCE_DATA subResData = {};
	subResData.pData = texData.data();
	subResData.RowPitch = sizeof(texData[0]) * size.width;
	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
	SetUploadTexure(subResData, colType);
	
	return true;
}

bool Dx12Wrapper::CreateGradationTexture(const Size size)
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
	SetUploadTexure(subResData, ColTexType::Grad);
	return true;
}

void Dx12Wrapper::SetUploadTexure(D3D12_SUBRESOURCE_DATA& subResData, ColTexType colType)
{
	auto& texture = defTextures_[static_cast<int>(colType)];
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
		IID_PPV_ARGS(texture.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// 転送元
	ComPtr<ID3D12Resource>intermediateBuff;	// 中間バッファ
	auto buffSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);
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
	UpdateSubresources(cmdList_.Get(), texture.Get(),
		intermediateBuff.Get(), 0, 0, 1, &subResData);
	cmdList_->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

bool Dx12Wrapper::CreateDefaultTextures()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);
	defTextures_.resize(static_cast<int>(ColTexType::Max));
	// cmdListにテクスチャデータを積む
	CreateMonoColorTexture(ColTexType::White,Color(0xff));	// 白
	CreateMonoColorTexture(ColTexType::Black,Color(0x00));	// 黒
	CreateGradationTexture({ minTexSize ,256 });	// グラデ
	cmdList_->Close();
	ExecuteAndWait();
	intermediateBuffList.clear();
	return true;
}

bool Dx12Wrapper::CreateBasicDescriptors()
{
	auto& transResBind = pmdResource_->GetGroops(GroopType::TRANSFORM);
	transResBind.Init({ BuffType::CBV, BuffType::CBV });
	transResBind.AddBuffers(transformBuffer_.Get());
	transResBind.AddBuffers(boneBuffer_.Get());

	return true;
}

bool Dx12Wrapper::CreateTransformBuffer()
{
	transformBuffer_ = CreateBuffer(AligndValue(sizeof(BasicMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
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
	materialBuffer_ = CreateBuffer(mats.size() * strideBytes);
	auto gAddress = materialBuffer_->GetGPUVirtualAddress();
	uint8_t* mappedMaterial = nullptr;
	result = materialBuffer_->Map(0, nullptr, (void**)&mappedMaterial);
		assert(SUCCEEDED(result));

	auto& transResBind = pmdResource_->GetGroops(GroopType::MATERIAL);
	transResBind.Init({ BuffType::CBV, BuffType::SRV, BuffType::SRV, BuffType::SRV, BuffType::SRV });
	array<pair<string, ID3D12Resource*>, 4>texPairList;
	texPairList = { make_pair("bmp",defTextures_[static_cast<int>(ColTexType::White)].Get()),
					make_pair("sph",defTextures_[static_cast<int>(ColTexType::White)].Get()),
					make_pair("spa",defTextures_[static_cast<int>(ColTexType::Black)].Get()),
					make_pair("toon",defTextures_[static_cast<int>(ColTexType::Grad)].Get()) };
	for (int i = 0; i < mats.size(); ++i)
	{
		// マテリアル定数バッファビュー
		((BasicMaterial*)mappedMaterial)->diffuse = mats[i].diffuse;
		((BasicMaterial*)mappedMaterial)->ambient = mats[i].ambient;
		((BasicMaterial*)mappedMaterial)->speqular = mats[i].speqular;
		((BasicMaterial*)mappedMaterial)->alpha = mats[i].alpha;
		((BasicMaterial*)mappedMaterial)->speqularity = mats[i].speqularity;

		transResBind.AddBuffers(materialBuffer_.Get(), strideBytes);
		mappedMaterial += strideBytes;
		for (auto texpair : texPairList)
		{
			ID3D12Resource* res = pmdActor_->GetTextures(texpair.first)[i].Get();
			if (res == nullptr)
			{
				res = texpair.second;
			}
			transResBind.AddBuffers(res);
		}
	}
	materialBuffer_->Unmap(0, nullptr);
	return true;
}

bool Dx12Wrapper::CreateBoneBuffer()
{
	HRESULT result = S_OK;
	auto size = AligndValue(sizeof(XMFLOAT4X4) * 512, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	boneBuffer_ = CreateBuffer(size);
	result = boneBuffer_->Map(0, nullptr, (void**)&mappedBone_);
	assert(SUCCEEDED(result));
	const auto& bData = pmdActor_->GetPMDModel().GetBoneData();
	for (int i = 0; i < bData.size(); ++i)
	{
		boneTable_.emplace(bData[i].name, i);
	}
	// 全部単位行列に初期化
	fill_n(mappedBone_, 512, XMMatrixIdentity());	
	
	//vector<string> nameTbl = { "左ひじ","左腕","右ひじ","右腕" };
	//float rotatetable[] = { XM_PIDIV2, XM_PIDIV4,-XM_PIDIV2, -XM_PIDIV4 };
	//UpdateBones(0);
	// 値をmapしているものにコピー
	
	return true;
}

void Dx12Wrapper::UpdateBones(int currentFrameNo)
{
	const auto& bData = pmdActor_->GetPMDModel().GetBoneData();
	auto mats = pmdActor_->GetPMDModel().GetBoneMat();
	mats.resize(bData.size());
	fill(mats.begin(), mats.end(), XMMatrixIdentity());

	auto a = vmdMotion_->GetVMDData().data;
	for (auto& motion : vmdMotion_->GetVMDData().data)
	{		
		if (boneTable_.find(motion.first) == boneTable_.end())
		{
			continue;
		}
		auto bidx = boneTable_[motion.first];
		auto& bpos = bData[bidx].pos;
		auto rit = find_if(motion.second.rbegin(), motion.second.rend(),
			[currentFrameNo](const auto& v)
			{
				return v.frameNo <= currentFrameNo;
			});

		auto q = XMLoadFloat4(&motion.second[0].quaternion);
		XMFLOAT3 mov(0, 0, 0);
		if (rit != motion.second.rend())
		{		
			mov = rit->pos;
			q = XMLoadFloat4(&rit->quaternion);
			auto it = rit.base();
			if (it != motion.second.end())
			{
				auto t = static_cast<float>((currentFrameNo - rit->frameNo)) / 
					static_cast<float>((it->frameNo - rit->frameNo));
				//q = (1.0f - t) * q + t * XMLoadFloat4(&it->quaternion);
				q = XMQuaternionSlerp(q, XMLoadFloat4(&it->quaternion), t);
				/*auto vPos = XMVectorScale(XMLoadFloat3(&mov), (1.0f - t)) + 
							XMVectorScale(XMLoadFloat3(&it->pos),  t);*/
				auto vPos = XMVectorLerp(XMLoadFloat3(&mov), XMLoadFloat3(&it->pos), t);
				XMStoreFloat3(&mov,vPos);
			}	
		}
		mats[bidx] = XMMatrixIdentity();
		mats[bidx] *= XMMatrixTranslation(-bpos.x, -bpos.y, -bpos.z);
		// 回転
		mats[bidx] *= XMMatrixRotationQuaternion(q);
	
		mats[bidx] *= XMMatrixTranslation(bpos.x, bpos.y , bpos.z);
		// 移動
		mats[bidx] *= XMMatrixTranslation(mov.x, mov.y, mov.z);
	}
	RecursiveCalucurate(bData, mats, 0);
	copy(mats.begin(), mats.end(), mappedBone_);
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

void Dx12Wrapper::RecursiveCalucurate(const std::vector<PMDBone>& bones, std::vector<DirectX::XMMATRIX>& mats, int idx)
{
	auto& mat = mats[idx];
	auto& bone = bones[idx];
	for (auto child : bone.children)
	{
		mats[child] *= mat;
		RecursiveCalucurate(bones,mats, child);
	}
}

float Dx12Wrapper::CalucurateFromBezier(float x, DirectX::XMFLOAT2 bz[2])
{
	// ベジェが直線だったらxを返す
	if (bz[0].x == bz[1].x && bz[0].y == bz[1].y)
	{
		return x;
	}
	// P0(0.0)*(1-t)^3 + 3*P1*(1-t)^2*t + 3* P2*(1-t)*t^2 + P3(1,1)*(1-t)^3
	// ---t = f(x);
	// 3*P1.x*(1-t)^2*t + 3 * P2.x*(1-t)*t^2 + (1-t)^3
	// 3*P1.x*(t - 2t^2 + t^3) + 3*P2.x*(t^2 - t^3) + t^3
	// 次数で分ける
	// t^3 = 3*P1.x + 3*P2.x +1
	// t^2 = -6*P1.x + 3*P2.x;
	// t = 3*P1.x

	return 0.0f;
}

float Dx12Wrapper::CalucurateFromBezier(float x, DirectX::XMFLOAT2 bezier[4])
{
	return 0.0f;
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
	//const char* modelPath = "Resource/PMD/桜ミク/mikuXS桜ミク.pmd";
	//const char* modelPath = "Resource/PMD/雲雀/雲雀Ver1.10SW.pmd";
	const char* modelPath = "Resource/PMD/model/初音ミク.pmd";
	//const char* modelPath = "Resource/PMD/model/巡音ルカ.pmd";
	//const char* modelPath = "Resource/PMD/我那覇響v1.0/我那覇響v1.pmd";
	//const char* modelPath = "Resource/PMD/古明地さとり/古明地さとり152Normal.pmd";
	//const char* modelPath = "Resource/PMD/霊夢/reimu_F02.pmd";
	pmdActor_->LoadModel(modelPath);
	vmdMotion_ = make_shared<VMDLoder>();
	vmdMotion_->Load("Resource/VMD/swing2.vmd");
	
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
	pmdResource_ = make_shared<PMDResource>(dev_.Get());
	// 定数バッファ作成
	CreateTransformBuffer();
	
	// テクスチャ作成
	pmdActor_->CreatePMDModelTexture();
	// 基本テクスチャ作成
	CreateDefaultTextures();

	// マテリアルバッファの作成
	CreateMaterialBufferView();
	
	// ボーンバッファ作成
	CreateBoneBuffer();

	// 座標変換SRV用ディスクリプタヒープ作成
	CreateBasicDescriptors();
	
	// リソースデータをビルド
	pmdResource_->Build({ GroopType::TRANSFORM, GroopType::MATERIAL });
	// ビューポートとシザー矩形初期化
	InitViewRect();


	return true;
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
	static auto lastTime = GetTickCount();
	static size_t frame;
	
	//transResBind.AddBuffers(boneBuffer_.Get());
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
	angle = 0.02f;
	mappedBasicMatrix_->world *= XMMatrixRotationY(angle);
	mappedBasicMatrix_->world *= XMMatrixTranslation(0, modelY, 0);
	UpdateBones(frame);
	// 画面クリア
	//ClearDrawScreen();
	
	// 描画処理
	DrawPMDModel();

	// バッファフリップ
	//DrawExcute();
	frame = static_cast<float>((GetTickCount() - lastTime)) / (1000.0f / 30.0f);
	frame = frame % vmdMotion_->GetVMDData().duration;
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
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);
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
	cmdList_->SetGraphicsRootSignature(pmdResource_->GetRootSignature().Get());
	// ビューポートとシザー矩形の設定
	CD3DX12_VIEWPORT vp(bbResouces[bbIdx_].Get());	// これでできるが分割できない
	cmdList_->RSSetViewports(1, &vp);
	cmdList_->RSSetScissorRects(1, &scissorRect_);
}

void Dx12Wrapper::DrawPMDModel()
{

	cmdList_->SetPipelineState(pmdResource_->GetPipelineState().Get());
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	cmdList_->IASetIndexBuffer(&ibView_);

	// 座標変換ヒープセット
	auto resHeap = pmdResource_->GetGroops(GroopType::TRANSFORM).descHeap_.Get();
	ID3D12DescriptorHeap* deskHeaps[] = { resHeap/*resViewHeap_.Get()*/ };
	auto heapPos = /*resViewHeap_*/resHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, heapPos);

	// マテリアル&テクスチャヒープセット
	auto material = pmdActor_->GetPMDModel().GetMaterialData();
	uint32_t indexOffset = 0;
	auto matHeap = pmdResource_->GetGroops(GroopType::MATERIAL).descHeap_.Get();
	ID3D12DescriptorHeap* matDeskHeaps[] = { matHeap };
	cmdList_->SetDescriptorHeaps(1, matDeskHeaps);
	auto materialHeapPos = matHeap->GetGPUDescriptorHandleForHeapStart();
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
