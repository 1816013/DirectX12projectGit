#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <d3dx12.h>
#include <unordered_map>
// Effekseer
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>

using Microsoft::WRL::ComPtr;

struct BoardConstBuffer
{
	DirectX::XMFLOAT2 pos;
	float time;
};

class PMDActor;
class Renderer;
class TexManager;
class PrimitiveManager;
struct BasicMatrix;
struct Size;
struct Color;
struct PMDBone;
enum class ColTexType;
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
	/// 単色テクスチャ作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateMonoColorTexture(ColTexType colType, const Color col);

	/// <summary>
	/// グラデーションテクスチャ作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateGradationTexture( const Size size);

	/// <summary>
	/// GPUにアップロードするための準備
	/// </summary>
	/// <param name="size">大きさ</param>
	/// <param name="tex">テクスチャバッファ</param>
	/// <param name="subResData"></param>
	void SetUploadTexure( D3D12_SUBRESOURCE_DATA& subResData, ColTexType colType);
	
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
	void DrawShadow(BasicMatrix& mat);

	/// <summary>
	/// Effekseer初期化
	/// </summary>
	void InitEffekseer();

	void UpdateEffekseer();

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

	// デフォルトテクスチャ
	std::vector<ComPtr<ID3D12Resource>>defTextures_;

	// テクスチャリソース
	//std::shared_ptr<TexManager> texManager_;
	// PMDモデルデータ関連	
	std::shared_ptr<Renderer>renderer_;
	std::vector<std::shared_ptr<PMDActor>>pmdActor_;

	// 深度バッファ
	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12DescriptorHeap> depthDescHeap_;

	// バックバッファインデックス
	uint32_t bbIdx_;

	// マルチパスレンダリング
	// ポストエフェクトレンダーターゲット用テクスチャ
	ComPtr<ID3D12DescriptorHeap> firstRtvHeap_ = nullptr;	
	ComPtr<ID3D12DescriptorHeap> firstSrvHeap_ = nullptr;
	//ポストエフェクト用テクスチャ
	ComPtr<ID3D12Resource> rtTexture_ = nullptr;

	// 板ポリ頂点
	// TRIANGLE_STRIPで作る
	ComPtr<ID3D12Resource> boardPolyVerts_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW boardVBView_;
	ComPtr<ID3D12PipelineState> boardPipeLine_ = nullptr;
	ComPtr<ID3D12RootSignature> boardSig_ = nullptr;
	// 時間用
	float oldTime = 0;

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

	// Effekseer
	EffekseerRenderer::Renderer* efkRenderer_;
	Effekseer::Manager* efkManager_;
	EffekseerRenderer::SingleFrameMemoryPool* efkSfMemoryPool_;
	EffekseerRenderer::CommandList* efkCmdList_;
	Effekseer::Effect* effect_;
	Effekseer::Handle efkHandle_ = 0;
};