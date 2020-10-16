#include "Dx12Wrapper.h"
#include <Windows.h>
#include <cassert>
#include "../Application.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

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
	scDesc.Height = static_cast<UINT>(wSize.hight);
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


	CreateRenderTargetDescriptorHeap();

	//// レンダーターゲットをクリア
	//auto heapStart = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	//float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	//cmdAllocator_->Reset();	
	//cmdList_->Reset(cmdAllocator_, nullptr);
	//cmdList_->OMSetRenderTargets(1, &heapStart, false, nullptr);
	//cmdList_->ClearRenderTargetView(heapStart, clearColor, 0, nullptr);
	//result = cmdList_->Close();
	//assert(SUCCEEDED(result));
	//ID3D12CommandList* cmdLists[] = { cmdList_ };
	//cmdQue_->ExecuteCommandLists(1, cmdLists);
	//result = swapchain_->Present(0, 0);
	//assert(SUCCEEDED(result));

	return true;
}

bool Dx12Wrapper::Update()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_, nullptr);
	// ここに命令(コマンドリストを積んでいく)
	float clsCol[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	auto bbIdx = swapchain_->GetCurrentBackBufferIndex();
	auto rtvHeap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto rtvIncSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHeap.ptr += bbIdx * rtvIncSize;
	// リソースバリアを設定プレゼントからレンダーターゲット
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = bbResouces[bbIdx];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList_->ResourceBarrier(1, &barrier);
	// レンダーターゲットをセット
	cmdList_->OMSetRenderTargets(1, &rtvHeap, false, nullptr);
	// 背景色をクリア(色変える)
	cmdList_->ClearRenderTargetView(rtvHeap, clsCol, 0, nullptr);

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
