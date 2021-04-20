#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <d3dx12.h>
#include <unordered_map>
#include <array>

// 後で何とかする
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>


using Microsoft::WRL::ComPtr;

struct BoardConstBuffer
{
	DirectX::XMMATRIX proj; // プロジェクション(3D→2D)64
	DirectX::XMMATRIX invProj; // 逆プロジェクション(2D→3D)64
	DirectX::XMFLOAT2 pos;	// クリックされた座標 8
	float time;	// time 4
	// 下の効果を適用するかどうか
	uint32_t ssaoActive;	// SSAO
	uint32_t bloomActive;	// 光る
	uint32_t dofActive;	// 被写界深度
	uint32_t outLineN;	// 法線アウトライン
	uint32_t outLineD;	// 深度アウトライン
	uint32_t debug;	// 深度アウトライン
	float bloomCol[3];
};

// 前方宣言
class PMDActor;
class Renderer;
class PrimitiveManager;
struct BasicMatrix;
class EffectManager;
class CameraCtr;
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

	void CreatePrimitiveBufferView();

	/// <summary>
	/// DirectX12の更新を行う
	/// </summary>
	/// <returns>true:OK false:エラー</returns>
	bool Update();

	/// <summary>
	/// スクリーンをクリアして書けるようにする
	/// </summary>
	void ClearDrawScreen();

	/// <summary>
	/// バリアを張りコマンドリストを閉じてバックバッファに対して
	/// 処理を行いpresentする
	/// </summary>
	void DrawExcute();

	/// <summary>
	/// 処理を行って終了まで待つ
	/// </summary>
	void ExecuteAndWait();

	/// <summary>
	/// 後処理(デバイスをリリースをする)
	/// </summary>
	void Terminate();

private:	
	/// <summary>
	/// DirectXの機能レベルを確認する
	/// </summary>
	/// <returns>true:成功　false:失敗</returns>
	bool CreateDevice(IDXGIAdapter* adapter);

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
	bool CreateSwapChain(const HWND& hwnd);

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
	/// ビューポートとシザー矩形初期化
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool InitViewRect();

	/// <summary>
	/// デフォルトテクスチャ作成
	/// </summary>
	/// <returns></returns>
	bool CreateDefaultTextures();

	/// <summary>
	/// 深度バッファビュー作成
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateDepthBufferView();

	/// <summary>
	/// エラー情報を出力に表示
	/// </summary>
	/// <param name="errBlob">エラー情報</param>
	void OutputFromErrorBlob(ID3DBlob* errBlob);

	/// <summary>
	/// 基本的なバッファ作成
	/// </summary>
	/// <param name="size"></param>
	/// <param name="heapType"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

	/// <summary>
	/// ポストエフェクトの1パスめをレンダリングする
	/// テクスチャを作る
	/// </summary>
	void CreateRenderTargetTexture();

	/// <summary>
	/// 板ポリの頂点情報作成
	/// </summary>
	void CreateBoardPolyVerts();

	/// <summary>
	/// 板ポリパイプライン作成
	/// </summary>
	void CreateBoardPipeline();

	/// <summary>
	/// 影用バッファ作成
	/// </summary>
	void CreateShadowMapBufferAndView();

	/// <summary>
	/// 影用パイプライン生成
	/// </summary>
	void CreateShadowPipeline();

	/// <summary>
	/// 影描画
	/// </summary>
	/// <param name="mat"></param>
	void DrawShadow(BasicMatrix& mat);


	bool CreateSSAOPipeLine();

	ComPtr<ID3D12DescriptorHeap> CreateImguiDescriptorHeap();

	void CreateFontDescriptorHeap();

	HWND hwnd_;

	ComPtr<ID3D12Device> dev_ = nullptr;
	ComPtr<ID3D12CommandAllocator> cmdAllocator_ = nullptr;
	ComPtr<ID3D12GraphicsCommandList> cmdList_ = nullptr;
	ComPtr<ID3D12CommandQueue> cmdQue_ = nullptr;
	ComPtr<IDXGIFactory7> dxgi_ = nullptr;
	ComPtr<IDXGISwapChain4> swapchain_ = nullptr;
	
	std::vector<ComPtr<ID3D12Resource>> bbResouces = {};	// 裏画面リソース

	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;	// レンダーターゲットビューヒープ
	ComPtr<ID3D12Fence1> fence_ = nullptr;// フェンスオブジェクト(CPUGPU同期に必要)
	uint64_t fenceValue_ = 0;

	// ビューポート
	D3D12_VIEWPORT viewPort_ = {};
	// シザー矩形
	D3D12_RECT scissorRect_ = {};

	// PMDモデルデータ関連	
	std::shared_ptr<Renderer>renderer_;
	std::vector<std::shared_ptr<PMDActor>>pmdActor_;

	// 深度バッファ
	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12DescriptorHeap> depthDescHeap_;

	// バックバッファインデックス
	uint32_t bbIdx_;

	float clearCol[4] = { 0,0,0,0 };
	// マルチパスレンダリング
	// ポストエフェクトレンダーターゲット用ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> firstRtvHeap_ = nullptr;	
	ComPtr<ID3D12DescriptorHeap> firstSrvHeap_ = nullptr;
	//ポストエフェクト用テクスチャ
	std::array<ComPtr<ID3D12Resource>,3> rtTextures_;	// 0:描画用,1:法線 , 2:高輝度(hdr)

	// 通常の幅が半分になったバッファ
	ComPtr<ID3D12PipelineState> shrinkPipeLine_ = nullptr;
	ComPtr<ID3D12Resource> rtShrinkForBloom_;	// 縮小バッファ(ブルームぼかし用)
	ComPtr<ID3D12Resource> rtShrinkForDof_;	// 縮小バッファ(被写界深度用)

	// アンビエントオクルージョン(SSAO)
	ComPtr<ID3D12Resource>ssaoBuffer_;
	ComPtr<ID3D12PipelineState> ssaoPipeline_;


	// 板ポリ頂点
	// TRIANGLE_STRIPで作る
	ComPtr<ID3D12Resource> boardPolyVerts_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW boardVBView_;
	ComPtr<ID3D12PipelineState> boardPipeLine_ = nullptr;
	ComPtr<ID3D12RootSignature> boardSig_ = nullptr;
	

	// ノーマルマップ用
	ComPtr<ID3D12Resource> normalMapTex_ = nullptr;
	BoardConstBuffer* mappedBoardBuffer_;
	ComPtr<ID3D12Resource> boardConstBuffer_;	// 定数バッファ

	// シャドウマップ用
	ComPtr<ID3D12Resource> shadowDepthBuffer_;
	ComPtr<ID3D12DescriptorHeap> shadowDSVHeap_;
	ComPtr<ID3D12DescriptorHeap> shadowSRVHeap_;
	ComPtr<ID3D12PipelineState> shadowPipeline_ = nullptr;
	ComPtr<ID3D12RootSignature> shadowSig_ = nullptr;

	// 床
	std::shared_ptr<PrimitiveManager> primManager_;
	ComPtr<ID3D12DescriptorHeap>primitiveDescHeap_;

	// エフェクト
	std::shared_ptr<EffectManager>efcMng_;

	// カメラ　
	std::unique_ptr<CameraCtr>cameraCtr_;

	// imgui関連
	ComPtr<ID3D12DescriptorHeap>imguiHeap_;
	float fov_ = 45.0f;		// 被写界深度
	float cNear_ = 0.1f;	// クリップ用
	float cFar_ = 1000.0f;	// クリップ用

	// DirectXTK関連
	// フォント表示
	ComPtr<ID3D12DescriptorHeap> fontDescHeap_;
	DirectX::GraphicsMemory* gMemory_;
	DirectX::SpriteFont* spriteFonts_;
	DirectX::SpriteBatch* spriteBatch_;

	// 時間用
	float oldTime = 0;
};