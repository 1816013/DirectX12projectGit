#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

/// <summary>
/// DirectX12の初期化等の煩雑なところをまとめたクラス
/// </summary>
class Dx12Wrapper
{
public:
	Dx12Wrapper();
	~Dx12Wrapper();
	/// <summary>
	/// DirectX12の初期化を行う
	/// </summary>
	/// <returns>true : 成功, false : 失敗</returns>
	bool Init(HWND hwnd);

	/// <summary>
	/// 後処理(デバイスをリリースをする)
	/// </summary>
	void Terminate();
private:
	/// <summary>
	/// RenderTargetDescriptorHeapの作成
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateRenderTargetDescriptorHeap();

	ID3D12Device* dev_ = nullptr;
	ID3D12CommandAllocator* cmdAllocator_ = nullptr;
	ID3D12GraphicsCommandList* cmdList_ = nullptr;
	ID3D12CommandQueue* cmdQue_ = nullptr;
	IDXGIFactory7* dxgi_ = nullptr;
	IDXGISwapChain4* swapchain_ = nullptr;
	ID3D12Fence* fence_ = nullptr;
	ID3D12DescriptorHeap* rtvHeap_;
	
};

