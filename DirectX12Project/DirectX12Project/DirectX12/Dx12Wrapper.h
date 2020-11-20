#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <d3dx12.h>
#include <unordered_map>

using Microsoft::WRL::ComPtr;

struct Color
{
	uint8_t r, g, b, a;
	Color() :r(0), g(0), b(0), a(255) {};
	Color(uint8_t inr, uint8_t ing, uint8_t inb, uint8_t ina) :
		r(inr), g(ing), b(inb), a(inr) {}
	Color(uint8_t inc) :
		r(inc), g(inc), b(inc), a(255) {}
};

enum class ColTexType
{
	White,
	Black,
	Grad,
	Max
};

class PMDResource;
class PMDLoder;
class PMDActor;
class VMDLoder;
struct Size;
struct PMDBone;
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
	/// スクリーンをクリアして書けるようにする
	/// </summary>
	void ClearDrawScreen();


	/// <summary>
	/// コマンドリストを閉じてバックバッファに対して
	/// 画処理を行い終了まで待つ
	/// </summary>
	void DrawExcute();


	void DrawPMDModel();

	void ExecuteAndWait();

	/// <summary>
	/// 後処理(デバイスをリリースをする)
	/// </summary>
	void Terminate();


	ID3D12Device* GetDevice();
private:	
	// 基本行列
	struct BasicMatrix
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX viewproj;

	};
	// 基本マテリアル
	struct BasicMaterial
	{
		DirectX::XMFLOAT3 diffuse;
		float alpha;
		DirectX::XMFLOAT3 speqular;
		float speqularity;
		DirectX::XMFLOAT3 ambient;
	};

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
	/// 頂点バッファを生成(してCPU側の頂点情報をコピー)
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// インデックスバッファを生成
	/// </summary>
	void CreateIndexBuffer();

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
	/// リソースの基本的なディスクリプタ作成
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
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateDepthBufferView();

	/// <summary>
	/// マテリアルバッファビュー作成
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateMaterialBufferView();


	/// <summary>
	/// ボーンバッファ作成
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateBoneBuffer();

	/// <summary>
	/// ボーンを更新
	/// </summary>
	void UpdateBones(int currentFrameNo);

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
	/// ボーンの返還を末端まで影響させる再帰関数
	/// </summary>
	/// <param name="bones">ボーン情報</param>
	/// <param name="mats"></param>
	/// <param name="idx"></param>
	void RecursiveCalucurate(const std::vector<PMDBone>& bones, std::vector<DirectX::XMMATRIX>& mats, int idx);

	/// <summary>
	/// ベジェ補間を使ってxからyを得る
	/// 媒介変数tをxから求めて
	/// yを求める
	/// ベジェデータが二つなのは始点終点両方とも0だから
	/// </summary>
	/// <param name="x">入力値X</param>
	/// <returns>出力値Y</returns>
	float CalucurateFromBezier(float x, DirectX::XMFLOAT2 bz[2]);
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

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};	// 頂点バッファビュー
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// インデックスバッファビュー

	// シェーダ
	ComPtr<ID3D10Blob> vertexShader_ = nullptr;
	ComPtr<ID3D10Blob> pixelShader_ = nullptr;

	// ビューポート
	D3D12_VIEWPORT viewPort_ = {};
	// シザー矩形
	D3D12_RECT scissorRect_ = {};

	// リソース
	ComPtr<ID3D12DescriptorHeap> resViewHeap_ = nullptr;	// リソースビュー用ディスクリプタヒープ
	// テクスチャ
	std::vector<ComPtr<ID3D12Resource>>defTextures_;

	// map中の基本マテリアル
	std::shared_ptr<BasicMatrix> mappedBasicMatrix_;

	//// PMDモデルデータ関連
	// 一時的にここに置く
	std::shared_ptr<PMDActor>pmdActor_;
	std::shared_ptr<PMDResource> pmdResource_;
	std::shared_ptr<VMDLoder>vmdMotion_;
	std::unordered_map<std::string, uint16_t>boneTable_;
	DirectX::XMMATRIX* mappedBone_ = nullptr;

	// 定数バッファ
	ComPtr<ID3D12Resource> transformBuffer_;	// 定数バッファ

	// 深度バッファ
	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12DescriptorHeap> depthDescHeap_;

	// マテリアルバッファ
	ComPtr<ID3D12Resource> materialBuffer_;			// マテリアル用バッファ
	ComPtr<ID3D12DescriptorHeap> materialDescHeap_;	// マテリアル用ディスクリプタヒープ

	// バックバッファインデックス
	uint32_t bbIdx_;

	// ボーンバッファ
	ComPtr<ID3D12Resource> boneBuffer_;			// ボーン用バッファ
	ComPtr<ID3D12DescriptorHeap> boneDescHeap_;			// ボーン用ディスクリプタヒープ

};