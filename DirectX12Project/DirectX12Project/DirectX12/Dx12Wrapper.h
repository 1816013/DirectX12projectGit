#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include "../BMPLoder/BmpLoder.h"

class PMDModel;
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
	/// DirectXの機能レベルを確認する
	/// </summary>
	/// <returns>true:成功　false:失敗</returns>
	bool CheckFeatureLevel();

	/// <summary>
	/// 各種コマンド類初期化(cmdAllocator_, cmdList_, cmdQue_ )
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool InitCommandSet();

	/// <summary>
	/// スワップチェイン作成
	/// </summary>
	/// <param name="hwnd">ウインドウハンドル</param>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateSwapChain(const HWND hwnd);

	/// <summary>
	/// フェンス作成(スレッドセーフに必要)
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateFence();

	/// <summary>
	/// RenderTargetDescriptorHeapの作成
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateRenderTargetDescriptorHeap();

	/// <summary>
	/// 頂点バッファを生成(してCPU側の頂点情報をコピー)
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// インデックスバッファを生成
	/// </summary>
	void CreateIndexBuffer();

	/// <summary>
	/// パイプラインステートオブジェクト作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreatePipelineState();

	/// <summary>
	/// ビューポートとシザー矩形初期化
	/// </summary>
	/// /// <returns>true:成功 false:失敗</returns>
	bool InitViewRect();

	/// <summary>
	/// テクスチャの作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateTexture();

	/// <summary>
	/// リソースの基本的なデスクリプタ作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateBasicDescriptors();

	/// <summary>
	/// 座標変換バッファの作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateTransformBuffer();

	/// <summary>
	/// 深度バッファビュー作成
	/// </summary>
	/// <returns></returns>
	bool CreateDepthBufferView();

	/// <summary>
	/// マテリアルバッファビュー作成
	/// </summary>
	/// <returns></returns>
	bool CreateMaterialBufferView();

	/// <summary>
	/// エラー情報を出力に表示
	/// </summary>
	/// <param name="errBlob">エラー情報</param>
	void OutputFromErrorBlob(ID3DBlob* errBlob);

	ID3D12Device* dev_ = nullptr;
	ID3D12CommandAllocator* cmdAllocator_ = nullptr;
	ID3D12GraphicsCommandList* cmdList_ = nullptr;	
	ID3D12CommandQueue* cmdQue_ = nullptr;	
	IDXGIFactory7* dxgi_ = nullptr;
	IDXGISwapChain4* swapchain_ = nullptr;
	
	std::vector<ID3D12Resource*> bbResouces = {};	// 裏画面リソース

	ID3D12DescriptorHeap* rtvHeap_ = nullptr;	// レンダーターゲットビューヒープ
	ID3D12Fence1* fence_ = nullptr;// フェンスオブジェクト(CPUGPU同期に必要)
	uint64_t fenceValue_ = 0;

	// 頂点バッファ
	ID3D12Resource* vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};	// 頂点バッファビュー
	// インデックスバッファ
	ID3D12Resource* indexBuffer_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// インデックスバッファビュー

	// パイプライン
	ID3D12PipelineState* pipelineState_ = nullptr;

	// シェーダ
	ID3D10Blob* vertexShader_ = nullptr;
	ID3D10Blob* pixelShader_ = nullptr;

	//ルートシグネチャ
	ID3D12RootSignature* rootSig_ = nullptr;// ルートシグネクチャ

	// ビューポート
	D3D12_VIEWPORT viewPort_ = {};
	// シザー矩形
	D3D12_RECT scissorRect_ = {};

	// リソース
	ID3D12DescriptorHeap* resViewHeap_ = nullptr;	// リソースビュー用デスクリプタヒープ
	// テクスチャ
	ID3D12Resource* texBuffer_;	// テクスチャリソース

	// 基本マテリアル
	struct BasicMatrix
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX viewproj;
		DirectX::XMFLOAT3 diffuse;
	};
	// map中の基本マテリアル
	BasicMatrix* mappedBasicMatrix_;

	// PMDモデル
	std::shared_ptr<PMDModel> pmdModel_;

	// 定数バッファ
	ID3D12Resource* transformBuffer_;	// 定数バッファ

	// 深度バッファ
	ID3D12Resource* depthBuffer_;
	ID3D12DescriptorHeap* depthDescHeap_;

	// マテリアルバッファ
	ID3D12Resource* materialBuffer_;			// マテリアル用バッファ
	ID3D12DescriptorHeap* materialDescHeap_;	// マテリアル用デスクリプタヒープ
	
	
};

