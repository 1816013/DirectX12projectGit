#include "Dx12Wrapper.h"
#include <Windows.h>
#include <cassert>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <string>
#include "../Application.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;

XMFLOAT3 vertices_[3];

void Dx12Wrapper::CreateVertices()
{
	vertices_[0] = { -0.7, -0.7f, 0.0f };
	vertices_[1] = { 0.0f, 0.7f, 0.0f };
	vertices_[2] = { 0.7f, -0.7f, 0.0f };
}

void Dx12Wrapper::CreateVertexBaffer()
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ↓なくてもよい
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.VisibleNodeMask = 0;
	heapProp.CreationNodeMask = 0;
	// ここまで

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeof(vertices_);	// 頂点情報のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer_));
	assert(SUCCEEDED(result));

	//頂点データ転送
	XMFLOAT3* mappedData = nullptr;
	result = vertexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(std::begin(vertices_), std::end(vertices_), mappedData);
	vertexBuffer_->Unmap(0, nullptr);

	// 頂点バッファビュー
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices_[0]);
	vbView_.SizeInBytes = sizeof(vertices_);
}

bool Dx12Wrapper::CreatePipelineState()
{
	HRESULT result = S_OK;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	// IA(InputAssembler)
	// 入力レイアウト
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// 頂点シェーダ
	ID3DBlob* errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/vs.hlsl", nullptr, 
		nullptr, "VS", "vs_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &vertexShader_, &errBlob);
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));	
	plsDesc.VS.pShaderBytecode = vertexShader_->GetBufferPointer();
	plsDesc.VS.BytecodeLength = vertexShader_->GetBufferSize();

	// ラスタライザ設定
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	plsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	// ピクセルシェーダ
	result = D3DCompileFromFile(L"Shader/ps.hlsl", nullptr, 
		nullptr, "PS", "ps_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &pixelShader_, &errBlob);
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));

	plsDesc.PS.pShaderBytecode = pixelShader_->GetBufferPointer();
	plsDesc.PS.BytecodeLength = pixelShader_->GetBufferSize();

	// その他設定
	// デプスとステンシル設定
	plsDesc.DepthStencilState.DepthEnable = false;
	plsDesc.DepthStencilState.StencilEnable = false;

	plsDesc.NumRenderTargets = 1;
	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = 0xffffffff;

	// 出力設定
	// ブレンド
	plsDesc.BlendState.AlphaToCoverageEnable = false;
	plsDesc.BlendState.IndependentBlendEnable = false;
	plsDesc.BlendState.RenderTarget[0].BlendEnable = false;
	plsDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// ルートシグネチャ
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.DescriptorTable.NumDescriptorRanges = 1;

	D3D12_ROOT_SIGNATURE_DESC rsd = {};
	rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsd.NumParameters = 0;
	rsd.NumStaticSamplers = 0;

	// シグネチャ設定
	ID3DBlob* sig = nullptr;
	D3D12SerializeRootSignature(&rsd,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sig,
		&errBlob);

	// ルートシグネチャの生成
	result = dev_->CreateRootSignature(0,
		sig->GetBufferPointer(),
		sig->GetBufferSize(),
		IID_PPV_ARGS(&rootSig_));
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));
	plsDesc.pRootSignature = rootSig_;

	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
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
		errBlob->Release();
	}
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
	ID3D12Debug3* debug = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	debug->EnableDebugLayer();
	debug->Release();
#endif
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	// enum classを使ってないため警告が出るためキャスト
	D3D_FEATURE_LEVEL level = static_cast<D3D_FEATURE_LEVEL>(D3D_FEATURE_LEVEL_1_0_CORE);	
	
	for (auto lv : levels)
	{
		result = D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&dev_));
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
#if _DEBUG
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgi_));
#else 
	result = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgi_));
#endif
	assert(SUCCEEDED(result));

	// コマンドキュー作成
	D3D12_COMMAND_QUEUE_DESC cmdQDesc = {};
	cmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQDesc.NodeMask = 0;
	cmdQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = dev_->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(&cmdQue_));
	assert(SUCCEEDED(result));

	// swapchainDesc作成
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = static_cast<UINT>(wSize.width);
	scDesc.Height = static_cast<UINT>(wSize.height);
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferCount = 2;	// 表と裏画面
	// ※バックバッファでも動くが
	// 　違いが判らないため要検証
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; 
	scDesc.Flags = 0/*DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH*/;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Stereo = false;	// VRの時true
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	
	// swapchain作成
	result = dxgi_->CreateSwapChainForHwnd(cmdQue_,
		hwnd,
		&scDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&swapchain_);
	assert(SUCCEEDED(result));

	// コマンドアロケーターの作成
	dev_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		IID_PPV_ARGS(&cmdAllocator_));
	assert(SUCCEEDED(result));

	// コマンドリストの作成
	dev_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, 
		cmdAllocator_, nullptr,
		IID_PPV_ARGS(&cmdList_));
	assert(SUCCEEDED(result));
	cmdList_->Close();
	

	// フェンスを作る(スレッドセーフに必要)
	dev_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	fenceValue_ = fence_->GetCompletedValue();

	// レンダーターゲットを作成
	CreateRenderTargetDescriptorHeap();

	// 頂点バッファを作成
	CreateVertices();
	CreateVertexBaffer();

	if (!CreatePipelineState())
	{
		return false;
	}

	//// シェーダー読み込み
	//result = D3DCompileFromFile(L"Shader/vs.hlsl", nullptr, nullptr, "VS", "vs_5_1",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
	//	0, &vertexShader_, nullptr);
	//assert(SUCCEEDED(result));

	//result = D3DCompileFromFile(L"Shader/ps.hlsl", nullptr, nullptr, "PS", "ps_5_1",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
	//	0, &pixelShader_, nullptr);
	//assert(SUCCEEDED(result));
	//
	//// ルートシグネクチャ

	//D3D12_ROOT_SIGNATURE_DESC rsd = {};
	//rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//// シグネチャ設定
	//D3D12SerializeRootSignature(&rsd,
	//	D3D_ROOT_SIGNATURE_VERSION_1,
	//	&signature_,
	//	&error_);

	//// ルートシグネチャの生成
	//result = dev_->CreateRootSignature(0,
	//	signature_->GetBufferPointer(),
	//	signature_->GetBufferSize(),
	//	IID_PPV_ARGS(&rootSig_));
	//assert(SUCCEEDED(result));

	//// 頂点レイアウト
	//D3D12_INPUT_ELEMENT_DESC layout[] = {
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	//};

	//// パイプラインステートオブジェクト生成
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	//// -ルートシグネクチャと頂点レイアウト
	//plsDesc.pRootSignature = rootSig_;
	//plsDesc.InputLayout.pInputElementDescs = layout;
	//plsDesc.InputLayout.NumElements = _countof(layout);
	//// -シェーダ系
	//plsDesc.VS.pShaderBytecode = vertexShader_->GetBufferPointer();
	//plsDesc.VS.BytecodeLength = vertexShader_->GetBufferSize();
	//plsDesc.PS.pShaderBytecode = pixelShader_->GetBufferPointer();
	//plsDesc.PS.BytecodeLength = pixelShader_->GetBufferSize();
	//// --使わない
	////plsDesc.HS;
	////plsDesc.DS;
	////plsDesc.GS;
	//plsDesc.NumRenderTargets = 1;
	//plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	//// 深度ステンシル
	//plsDesc.DepthStencilState.DepthEnable = false;
	//plsDesc.DepthStencilState.StencilEnable = false;
	//plsDesc.DSVFormat;

	//// ラスタライザ
	//plsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	//plsDesc.RasterizerState.FrontCounterClockwise = false;
	//plsDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	//plsDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	//plsDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	//plsDesc.RasterizerState.DepthClipEnable = true;
	//plsDesc.RasterizerState.MultisampleEnable = false;
	//plsDesc.RasterizerState.AntialiasedLineEnable = false;
	//plsDesc.RasterizerState.ForcedSampleCount = 0;
	//plsDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//// その他
	//plsDesc.BlendState.AlphaToCoverageEnable = false;
	//plsDesc.BlendState.IndependentBlendEnable = false;
	//plsDesc.BlendState.RenderTarget[0].BlendEnable = false;
	//plsDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	//plsDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	//plsDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//plsDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	//plsDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//plsDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//plsDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	//plsDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//plsDesc.NodeMask = 0;
	//plsDesc.SampleDesc.Count = 1;
	//plsDesc.SampleDesc.Quality = 0;
	//plsDesc.SampleMask = 0xffffffff;
	////plsDesc.Flags;// デフォルトOK
	//plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; //三角形	
	//result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(&pipelineState_));
	//assert(SUCCEEDED(result));

	// ビューポート
	viewPort_.TopLeftX = 0;
	viewPort_.TopLeftY = 0;
	viewPort_.Width = wSize.width;
	viewPort_.Height = wSize.height;
	viewPort_.MaxDepth = 1.0f;
	viewPort_.MinDepth = 0.0f;
	
	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = wSize.width;
	scissorRect_.bottom = wSize.height;
	return true;
}

bool Dx12Wrapper::Update()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_, pipelineState_);
	// ここに命令(コマンドリストを積んでいく)	
	auto bbIdx = swapchain_->GetCurrentBackBufferIndex();	
	
	// リソースバリアを設定プレゼントからレンダーターゲット
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = bbResouces[bbIdx];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList_->ResourceBarrier(1, &barrier);

	// レンダーターゲットをセット	
	auto rtvHeap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto rtvIncSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHeap.ptr += bbIdx * rtvIncSize;
	cmdList_->OMSetRenderTargets(1, &rtvHeap, false, nullptr);
	// 背景色をクリア(色変える)
	float clsCol[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	cmdList_->ClearRenderTargetView(rtvHeap, clsCol, 0, nullptr);

	cmdList_->SetGraphicsRootSignature(rootSig_);
	// ビューポートとシザー矩形の設定
	cmdList_->RSSetViewports(1, &viewPort_);
	cmdList_->RSSetScissorRects(1, &scissorRect_);
	// 三角形描画
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	cmdList_->DrawInstanced(3, 1, 0, 0);

	// リソースバリアを設定レンダーターゲットからプレゼント
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList_->ResourceBarrier(1, &barrier);

	cmdList_->Close();

	ID3D12CommandList* cmdLists[] = { cmdList_ };
	cmdQue_->ExecuteCommandLists(1, cmdLists);
	cmdQue_->Signal(fence_,++fenceValue_);
	// Execute完了まで待つ処理
	while (true)
	{
		if (fence_->GetCompletedValue() == fenceValue_)
		{
			break;
		}
	}
	swapchain_->Present(1, 0);
	return true;
}

void Dx12Wrapper::Terminate()
{
	dev_->Release();
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
	swapchain_->GetDesc1(&swDesc);
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
		dev_->CreateRenderTargetView(bbResouces[i], &rtvDesc, heap);	// キャンパスと職人を紐づける
		heap.ptr += incSize;// 職人とキャンバスのペアのぶん次のところまでオフセット
	}

	return SUCCEEDED(result);
}
