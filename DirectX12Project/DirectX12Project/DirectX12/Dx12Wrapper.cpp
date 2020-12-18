#include "Dx12Wrapper.h"
#include <cassert>
#include <d3dcompiler.h>
#include "../Application.h"
#include "../Common/StrOperater.h"
#include "../Common.h"
#include "PMDResource.h"
#include "PMDActor.h"
#include "../PMDLoder/PMDLoder.h"
#include "TexManager.h"
#include "Renderer.h"
#include "mesh/PrimitiveManager.h"
#include "Effect/EffectManager.h"


//#include "../BMPLoder/BmpLoder.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DxGuid.lib")

using namespace DirectX;
using namespace std;

namespace
{
	//vector<PMDVertex> vertices_;
	//vector<unsigned short>indices_;

	constexpr int minTexSize = 4;
	constexpr int maxTexHeight = 256;
		
	// 中間バッファ一時保持用
	vector<ComPtr<ID3D12Resource>>intermediateBuffList_;
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

//bool Dx12Wrapper::CreateMonoColorTexture(ColTexType colType, const Color col)
//{	
//	HRESULT result = S_OK;
//	Size size = { minTexSize, minTexSize };
//	std::vector<Color>texData(size.width * size.height);
//	
//	std::fill(texData.begin(), texData.end(), col);	// 全部0xffで初期化
//	
//	D3D12_SUBRESOURCE_DATA subResData = {};
//	subResData.pData = texData.data();
//	subResData.RowPitch = sizeof(texData[0]) * size.width;
//	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
//	SetUploadTexure(subResData, colType);
//	
//	return true;
//}
//
//bool Dx12Wrapper::CreateGradationTexture(const Size size)
//{
//	std::vector<Color>texData(size.width * size.height);
//	for (size_t i = 0; i < 256; ++i)
//	{
//		fill_n(&texData[i * 4], 4, Color(static_cast<uint8_t>(255 - i)));	// rgb全部0x00で初期化
//	}
//	D3D12_SUBRESOURCE_DATA subResData = {};
//	subResData.pData = texData.data();
//	subResData.RowPitch = sizeof(texData[0]) * size.width;
//	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
//	SetUploadTexure(subResData, ColTexType::Grad);
//	return true;
//}
//
//void Dx12Wrapper::SetUploadTexure(D3D12_SUBRESOURCE_DATA& subResData, ColTexType colType)
//{
//	defTextures_.resize(static_cast<int>(ColTexType::Max));
//	auto& texture = defTextures_[static_cast<int>(colType)];
//	// 転送先
//	auto width = subResData.RowPitch / sizeof(Color);
//	auto height = subResData.SlicePitch / subResData.RowPitch;
//	
//	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
//	D3D12_RESOURCE_DESC resDesc = 
//		CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 
//			width,
//			static_cast<UINT>( height));
//	auto result = dev_->CreateCommittedResource(&heapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&resDesc,
//		D3D12_RESOURCE_STATE_COPY_DEST,
//		nullptr,
//		IID_PPV_ARGS(texture.ReleaseAndGetAddressOf()));
//	assert(SUCCEEDED(result));
//
//	// 転送元
//	ComPtr<ID3D12Resource>intermediateBuff;	// 中間バッファ
//	auto buffSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);
//	auto intermediateHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
//	auto intermediateResDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
//
//	result = dev_->CreateCommittedResource(&intermediateHeapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&intermediateResDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(intermediateBuff.ReleaseAndGetAddressOf()));
//	assert(SUCCEEDED(result));
//	intermediateBuffList_.push_back(intermediateBuff);
//	
//	// コマンドリストに登録
//	// 中でCopyTextureRegionが走っているため
//	// コマンドキュー待ちが必要
//	UpdateSubresources(cmdList_.Get(), texture.Get(),
//		intermediateBuff.Get(), 0, 0, 1, &subResData);
//	cmdList_->ResourceBarrier(1,
//		&CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(),
//			D3D12_RESOURCE_STATE_COPY_DEST,
//			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
//}

bool Dx12Wrapper::CreateDefaultTextures()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);
	defTextures_.resize(static_cast<int>(ColTexType::Max));
	// cmdListにテクスチャデータを積む
	auto& texMng = TexManager::GetInstance();
	texMng.CreateMonoColorTexture(cmdList_.Get(),ColTexType::White,Color(0xff));	// 白
	texMng.CreateMonoColorTexture(cmdList_.Get(), ColTexType::Black,Color(0x00));	// 黒
	texMng.CreateGradationTexture(cmdList_.Get(), { minTexSize ,256 });	// グラデ
	cmdList_->Close();
	ExecuteAndWait();
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
		IID_PPV_ARGS(depthBuffer_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC desDesc = {};
	desDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desDesc.NumDescriptors = 1;
	desDesc.NodeMask = 0;
	desDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = dev_->CreateDescriptorHeap(&desDesc,
		IID_PPV_ARGS(depthDescHeap_.ReleaseAndGetAddressOf()));
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

void Dx12Wrapper::CreateRenderTargetTexture()
{
	HRESULT result = S_OK;
	// バッファの作成
	auto bbResDesc = bbResouces[0]->GetDesc();
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(bbResDesc.Format, bbResDesc.Width, bbResDesc.Height);
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	
	D3D12_CLEAR_VALUE clearValue = {};
	float col[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
	copy_n(col, 4, clearValue.Color);
	clearValue.Format = resDesc.Format;
	for (auto& rtTex : rtTextures_)
	{
		result = dev_->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&clearValue,
			IID_PPV_ARGS(rtTex.ReleaseAndGetAddressOf())
		);
		assert(SUCCEEDED(result));
	}

	// 2つのディスクリプタヒープ作成
	// レンダーターゲットビュー
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = 2;// 表と裏画面用
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = dev_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(firstRtvHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = resDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	auto rtvHeapPos = firstRtvHeap_->GetCPUDescriptorHandleForHeapStart();
	for (auto& rtTex : rtTextures_)
	{
		dev_->CreateRenderTargetView(rtTex.Get(), &rtvDesc, rtvHeapPos);
		rtvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// シェーダーリソースビュー
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; 
	srvHeapDesc.NodeMask = 0;
	srvHeapDesc.NumDescriptors = 6;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = dev_->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(firstSrvHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	auto srvHeapPos = firstSrvHeap_->GetCPUDescriptorHandleForHeapStart();

	// 1パス目のレンダリング結果
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = resDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dev_->CreateShaderResourceView(rtTextures_[0].Get(), &srvDesc, srvHeapPos);
	// ノーマルマップ
	srvDesc.Format = normalMapTex_->GetDesc().Format;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(normalMapTex_.Get(), &srvDesc, srvHeapPos);
	// ライト深度
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(shadowDepthBuffer_.Get(), &srvDesc, srvHeapPos);
	// 深度ビュー
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(depthBuffer_.Get(), &srvDesc, srvHeapPos);
	// 1パス目の法線
	srvDesc.Format = resDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(rtTextures_[1].Get(), &srvDesc, srvHeapPos);
	// 定数バッファ(ボードトランスフォーム)
	boardConstBuffer_ = CreateBuffer(Common::AligndValue(sizeof(BoardConstBuffer), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
	boardConstBuffer_->Map(0, nullptr, (void**)&mappedBoardBuffer_);
	mappedBoardBuffer_->pos = XMFLOAT2(0, 0);
	mappedBoardBuffer_->time = 1;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	auto cbDesc = boardConstBuffer_->GetDesc();
	cbvDesc.BufferLocation = boardConstBuffer_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(cbDesc.Width);
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateConstantBufferView(&cbvDesc, srvHeapPos);
	
}

void Dx12Wrapper::CreateBoardPolyVerts()
{
	XMFLOAT3 verts[] = { {-1,-1,0.1f},
	{-1,1,0.1f},
	{1,-1,0.1f},
	{1,1,0.1f} };
	boardPolyVerts_ = CreateBuffer(sizeof(verts));
	XMFLOAT3* mappedData = nullptr;
	boardPolyVerts_->Map(0, nullptr, (void**)&mappedData);
	copy(begin(verts), end(verts), mappedData);
	boardPolyVerts_->Unmap(0, nullptr);

	// ビュー作成
	boardVBView_.BufferLocation = boardPolyVerts_->GetGPUVirtualAddress();
	boardVBView_.StrideInBytes = sizeof(verts[0]);
	boardVBView_.SizeInBytes = static_cast<UINT>(sizeof(verts));

}

void Dx12Wrapper::CreateBoardPipeline()
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
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}		
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// 頂点シェーダ
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/boardVS.hlsl",
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
	result = D3DCompileFromFile(L"Shader/boardPS.hlsl",
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
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	// その他設定
	// デプスとステンシル設定
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState.DepthEnable = false;
	//plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

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
	// ルートシグネチャ
	D3D12_ROOT_PARAMETER rp[1] = {};
	D3D12_DESCRIPTOR_RANGE range[2] = {};

	// レンジ
	// 行列定数バッファ
	range[0] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // レンジタイプ t
		5,// デスクリプタ数	t0～t4まで	1:1パス目,2:歪み用,3:ライトパス, 4: 深度テクスチャ, 5: 1パス目法線 
		0);// ベースレジスタ番号 t0	

	range[1] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // レンジタイプ d
		1,// デスクリプタ数	d0～d0まで
		0);// ベースレジスタ番号 d0	

	// ルートパラメータ
	// 座標変換
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[0],	// ルートパラメータ
		2,		// レンジ数
		&range[0],// レンジ先頭アドレス
		D3D12_SHADER_VISIBILITY_ALL);	

	D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
	samplerDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	/*samplerDesc->AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc->AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;*/

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc(1, rp, 1, samplerDesc);
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// シグネチャ設定
	//ComPtr<ID3DBlob> errBlob = nullptr;
	ComPtr<ID3DBlob> sigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sigBlob,
		&errBlob);
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	// ルートシグネチャの生成
	result = dev_->CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(boardSig_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	plsDesc.pRootSignature = boardSig_.Get();
	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(boardPipeLine_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void Dx12Wrapper::CreateShadowMapBufferAndView()
{
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);

	//auto rtvDesc = bbResouces[0]->GetDesc();
	//auto length = Common::AligndValue()
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32_TYPELESS,
		1024,
		1024);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	CD3DX12_CLEAR_VALUE clearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	auto result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(shadowDepthBuffer_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC desDesc = {};
	desDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desDesc.NumDescriptors = 1;
	desDesc.NodeMask = 0;
	desDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = dev_->CreateDescriptorHeap(&desDesc,
		IID_PPV_ARGS(shadowDSVHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	dev_->CreateDepthStencilView(shadowDepthBuffer_.Get(),
		&dsvDesc,
		shadowDSVHeap_->GetCPUDescriptorHandleForHeapStart());

	D3D12_DESCRIPTOR_HEAP_DESC srvDesDesc = {};
	srvDesDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesDesc.NumDescriptors = 1;
	srvDesDesc.NodeMask = 0;
	srvDesDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = dev_->CreateDescriptorHeap(&srvDesDesc,
		IID_PPV_ARGS(shadowSRVHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// モデルに深度値をセット
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dev_->CreateShaderResourceView(shadowDepthBuffer_.Get(),
		&srvDesc,
		shadowSRVHeap_->GetCPUDescriptorHandleForHeapStart());
	for (auto actor : pmdActor_)
	{
		auto& shadowResBind = actor->GetPMDResource().GetGroops(GroopType::DEPTH);
		shadowResBind.AddBuffers(shadowDepthBuffer_.Get());
	}

}

void Dx12Wrapper::CreateShadowPipeline()
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
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// ボーン番号 2バイト整数型
		{"BONE_NO", 0, DXGI_FORMAT_R16G16_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		//  2バイト整数型
		{"WEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// 頂点シェーダ
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/ShadowVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"ShadowVS", "vs_5_1",
		0,
		0, vsBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));
	plsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());

	// ラスタライザ設定
	plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plsDesc.RasterizerState.FrontCounterClockwise = true;
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// 出力設定
	plsDesc.NumRenderTargets = 0;
	//plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// その他設定
	// デプスとステンシル設定
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// ブレンド
	plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// シグネチャ設定
	ComPtr<ID3DBlob> sigBlob = nullptr;
	result = D3DGetBlobPart(vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		D3D_BLOB_ROOT_SIGNATURE, 0, &sigBlob);
	assert(SUCCEEDED(result));
	// ルートシグネチャの生成
	result = dev_->CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(shadowSig_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	plsDesc.pRootSignature = shadowSig_.Get();
	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(shadowPipeline_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void Dx12Wrapper::DrawShadow(BasicMatrix& mat)
{
	cmdList_->SetPipelineState(shadowPipeline_.Get());
	cmdList_->SetGraphicsRootSignature(shadowSig_.Get());
	
	auto dsvHandle = shadowDSVHeap_->GetCPUDescriptorHandleForHeapStart();
	cmdList_->OMSetRenderTargets(0, nullptr, false, &dsvHandle);
	cmdList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	XMVECTOR camPos = { -20,20,20 };
	XMVECTOR direction = { 1,-1,-1 };
	mat.lightVP = XMMatrixLookToRH(camPos, direction, { 0,1,0,0 });
	mat.lightVP *= XMMatrixOrthographicRH(50.0f, 50.0f, 1.0f, 500.0f);

	// ビューポートとシザー矩形の設定
	auto depthDesk = shadowDepthBuffer_->GetDesc();
	CD3DX12_VIEWPORT vp(shadowDepthBuffer_.Get());
	cmdList_->RSSetViewports(1, &vp);
	auto scissorRect = CD3DX12_RECT(0, 0, static_cast<UINT>(depthDesk.Width), depthDesk.Height);
	cmdList_->RSSetScissorRects(1, &scissorRect);

	// 描画命令
	for (auto& actor : pmdActor_)
	{
		auto descHeap = actor->GetPMDResource().GetGroops(GroopType::TRANSFORM).descHeap_.Get();
		cmdList_->SetDescriptorHeaps(1, &descHeap);
		cmdList_->SetGraphicsRootDescriptorTable(0, descHeap->GetGPUDescriptorHandleForHeapStart());
		cmdList_->DrawIndexedInstanced(static_cast<UINT>(actor->GetPMDModel().GetIndexData().size()), 1, 0, 0, 0);
	}
	
	//// リソースバリアを設定デプスからピクセルシェーダ
	//auto depthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
	//	depthBuffer_.Get(),	// リソース
	//	D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 前ターゲット
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// 後ろターゲット
	//);
	//cmdList_->ResourceBarrier(1, &depthBarrier);
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
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgi_.ReleaseAndGetAddressOf()));
#else 
	result = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgi_));
#endif
	assert(SUCCEEDED(result));
	vector<IDXGIAdapter*>adapters;
	IDXGIAdapter* adapter = nullptr;
	for (int i = 0; dxgi_->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND;++i)
	{
		adapters.push_back(adapter);
	}
	for (auto& adpt : adapters)
	{
		DXGI_ADAPTER_DESC adptDesc = {};
		result = adpt->GetDesc(&adptDesc);
		assert(SUCCEEDED(result));
		std::wstring strDesc = adptDesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			adapter = adpt;
			break;
		}
		
	}
	CreateDevice(adapter);

	TexManager::GetInstance().SetDevice(dev_.Get());

	InitCommandSet();
	
	CreateSwapChain(hwnd);
	
	CreateFence();

	renderer_ = make_shared<Renderer>(*dev_.Get());
	//const char* modelPath = "Resource/PMD/桜ミク/mikuXS桜ミク.pmd";
	//const char* modelPath = "Resource/PMD/雲雀/雲雀Ver1.10SW.pmd";
	const char* modelPath = "Resource/PMD/model/初音ミク.pmd";
	//const char* modelPath = "Resource/PMD/model/初音ミクmetal.pmd";
	//const char* modelPath = "Resource/PMD/model/巡音ルカ.pmd";
	//const char* modelPath = "Resource/PMD/我那覇響v1.0/我那覇響v1.pmd";
	//const char* modelPath = "Resource/PMD/古明地さとり/古明地さとり152Normal.pmd";
	//const char* modelPath = "Resource/PMD/霊夢/reimu_F02.pmd";
	pmdActor_.push_back(make_shared<PMDActor>(dev_, modelPath, XMFLOAT3(0,0,0)));
	/*modelPath = "Resource/PMD/古明地さとり/古明地さとり152Normal.pmd";
	pmdActor_.push_back(make_shared<PMDActor>(dev_, modelPath, XMFLOAT3(10,0,0)));*/
	

	// レンダーターゲットを作成
	CreateRenderTargetDescriptorHeap();
	
	// 深度バッファビュー作成
	CreateDepthBufferView();
	// シャドウマップ用バッファとdsv作成
	CreateShadowMapBufferAndView();
	// 板ポリバーテックス作成
	CreateBoardPolyVerts();

	// 基本テクスチャ作成
	CreateDefaultTextures();

	// レンダー用テクスチャ作成
	//texManager_ = make_shared<TexManager>(*dev_.Get());
	TexManager::GetInstance().CreateTexture(L"Resource/image/NormalMap2.png", normalMapTex_);

	// 1パス目書き込みバッファと対応するRTV,
	// SRVを作る
	CreateRenderTargetTexture();

	for (auto actor : pmdActor_)
	{
		// テクスチャ作成
		actor->CreatePMDModelTexture();

		// マテリアルバッファ及びビューの作成*
		actor->CreateMaterialBufferView(defTextures_);

		// 定数バッファ作成*カメラ行列を分離
		actor->CreateTransformBuffer();
		// ボーンバッファ作成*
		actor->CreateBoneBuffer();

		// 座標変換SRV用ディスクリプタヒープ作成*
		actor->CreateBasicDescriptors();

		// リソースデータをビルド
		actor->GetPMDResource().Build({ GroopType::TRANSFORM, GroopType::MATERIAL, GroopType::DEPTH });
	}

	// 床のデスクリプタヒープ
	CreatePrimitiveBufferView();

	// ビューポートとシザー矩形初期化
	InitViewRect();
	// 板ポリパイプライン作成
	CreateBoardPipeline();
	// 影のパイプライン作成
	CreateShadowPipeline();

	primManager_ = make_shared<PrimitiveManager>(dev_);
	primManager_->CreatePlane(XMFLOAT3(0, 0, 0), 50, 50);
	//efcMng_ = make_shared<EffectManager>(dev_.Get(), cmdQue_.Get());

	return true;
}

void Dx12Wrapper::CreatePrimitiveBufferView()
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = static_cast<UINT>(1 + pmdActor_.size());
	descHeapDesc.NodeMask = 0;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	auto result = dev_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(primitiveDescHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	auto handle = primitiveDescHeap_->GetCPUDescriptorHandleForHeapStart();
	// 座標変換及びライトから撮影した深度のテクスチャのためのビュー
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	for (auto actor : pmdActor_)
	{
		auto transDesc = actor->GetPMDResource().GetGroops(GroopType::TRANSFORM).resources_[0];
		cbvDesc.BufferLocation = transDesc.resource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = static_cast<UINT>(transDesc.resource->GetDesc().Width);
		dev_->CreateConstantBufferView(&cbvDesc, handle);
		handle.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	dev_->CreateShaderResourceView(shadowDepthBuffer_.Get(), &srvDesc, handle);
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
	static auto lastTime = GetTickCount64();
	static float deltaTime = 0.0f;
	static int frame = 0;
	auto rtHandle = firstRtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtHandle.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE  rtvHeaps[] = { firstRtvHeap_->GetCPUDescriptorHandleForHeapStart(), rtHandle };
	// pmdモデルアップデート
	for (auto actor : pmdActor_)
	{
		actor->Update(deltaTime);
		cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList_->IASetVertexBuffers(0, 1, &actor->GetVbView());
		cmdList_->IASetIndexBuffer(&actor->GetIbView());
		// 影
		// 復元用
		DrawShadow(actor->GetBasicMarix());
		// モデル描画
		CD3DX12_VIEWPORT vp(bbResouces[bbIdx_].Get());	// これでできるが分割できない
		cmdList_->RSSetViewports(1, &vp);
		cmdList_->RSSetScissorRects(1, &scissorRect_);
		cmdList_->SetGraphicsRootSignature(renderer_->GetRootSignature().Get());
		cmdList_->SetPipelineState(renderer_->GetPipelineState().Get());
		cmdList_->OMSetRenderTargets(2, rtvHeaps
			, false, &depthDescHeap_->GetCPUDescriptorHandleForHeapStart());
		actor->DrawModel(cmdList_);
	}
	primManager_->Draw(cmdList_.Get(), primitiveDescHeap_.Get());
	
	// エフェクト(effekseer)
	//efcMng_->Update(deltaTime * 60, cmdList_.Get());
	BYTE keyState[256];
	static BYTE lastState[256];
	auto result = GetKeyboardState(keyState);
	if (keyState[VK_SPACE] & 0x80 && !(lastState[VK_SPACE] & 0x80))
	{
		//efcMng_->PlayEffect("depthTest" , {0,10,0});
	}
	result = GetKeyboardState(lastState);
	frame++;

	// 板ポリ更新
	mappedBoardBuffer_->time += deltaTime;
	mappedBoardBuffer_->time = fmodf(mappedBoardBuffer_->time, 2);

	// リソースバリアを設定レンダーターゲットからシェーダ
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		rtTextures_[0].Get(),	// リソース
		D3D12_RESOURCE_STATE_RENDER_TARGET,	// 前ターゲット
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// 後ろターゲット
	);
	cmdList_->ResourceBarrier(1, &barrier);
	

	// -板ポリにバックバッファを書き込む
	const auto rtvIncSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	auto bbRtvHeap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	bbRtvHeap.ptr += static_cast<SIZE_T>(bbIdx_)* rtvIncSize;
	cmdList_->OMSetRenderTargets(1, &bbRtvHeap, false, nullptr);
	// 板ポリ描画
	cmdList_->SetPipelineState(boardPipeLine_.Get());
	cmdList_->SetGraphicsRootSignature(boardSig_.Get());
	ID3D12DescriptorHeap* deskHeaps[] = { firstSrvHeap_.Get() };
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, firstSrvHeap_->GetGPUDescriptorHandleForHeapStart());
	cmdList_->IASetVertexBuffers(0, 1, &boardVBView_);
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList_->DrawInstanced(4, 1, 0, 0);

	// リソースバリアを設定シェーダからレンダーターゲット
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		rtTextures_[0].Get(),	// リソース
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// 前ターゲット		
		D3D12_RESOURCE_STATE_RENDER_TARGET// 後ろターゲット
	);
	cmdList_->ResourceBarrier(1, &barrier);

	auto nowTime = GetTickCount64();
	deltaTime = static_cast<float>(nowTime - oldTime) / 1000;
	oldTime = static_cast<float>(nowTime);
	
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
	swapchain_->Present(0, 0);
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

	// バックバッファにセット	
	auto rtHandle = firstRtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtHandle.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE  rtvHeaps[] = { firstRtvHeap_->GetCPUDescriptorHandleForHeapStart(), rtHandle };
	D3D12_CPU_DESCRIPTOR_HANDLE  dsvHeaps[] = { depthDescHeap_->GetCPUDescriptorHandleForHeapStart() };
	cmdList_->OMSetRenderTargets(2, rtvHeaps, false, dsvHeaps);
	// 画面をクリア(色変える)
	float clsCol[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
	cmdList_->ClearRenderTargetView(rtvHeaps[0], clsCol, 0, nullptr);
	float clsCol2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
	cmdList_->ClearRenderTargetView(rtvHeaps[1], clsCol2, 0, nullptr);
	cmdList_->ClearDepthStencilView(
		depthDescHeap_->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);

	// ビューポートとシザー矩形の設定
	CD3DX12_VIEWPORT vp(bbResouces[bbIdx_].Get());	// これでできるが分割できない
	cmdList_->RSSetViewports(1, &vp);
	cmdList_->RSSetScissorRects(1, &scissorRect_);
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


bool Dx12Wrapper::CreateDevice(IDXGIAdapter* adapter)
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
		auto result = D3D12CreateDevice(adapter, lv, IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf()));
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
	result = dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(rtvHeap_.ReleaseAndGetAddressOf()));
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
	rtvDesc.Texture2D.PlaneSlice = 0;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	auto heap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto incSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (int i = 0; i < num_rtvs; i++)
	{
		swapchain_->GetBuffer(i, IID_PPV_ARGS(bbResouces[i].ReleaseAndGetAddressOf()));//「キャンバス」を取得
		dev_->CreateRenderTargetView(bbResouces[i].Get(), &rtvDesc, heap);	// キャンパスと職人を紐づける
		heap.ptr += incSize;// 職人とキャンバスのペアのぶん次のところまでオフセット
	}

	return SUCCEEDED(result);
}
