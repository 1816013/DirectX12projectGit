#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>


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
	/// <returns>true : 成功 false : 失敗</returns>
	bool Init(HWND hwnd);

	/// <summary>
	/// DirectX12の更新を行う
	/// </summary>
	/// <returns>true:OK false:エラー</returns>
	bool Update();

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
	

	std::vector<ID3D12Resource*> bbResouces = {};	// 裏画面リソース

	ID3D12DescriptorHeap* rtvHeap_ = nullptr;
	ID3D12Fence1* fence_ = nullptr;// フェンスオブジェクト(CPUGPU同期に必要)
	uint64_t fenceValue_ = 0;
};

