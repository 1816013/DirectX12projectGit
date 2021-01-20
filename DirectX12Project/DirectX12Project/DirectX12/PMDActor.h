#pragma once
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <unordered_map>

using Microsoft::WRL::ComPtr;
using Textures = std::vector<ComPtr<ID3D12Resource>>;

// 基本行列
struct BasicMatrix
{
	DirectX::XMMATRIX world;	// ワールド行列(モデル座標)
	DirectX::XMMATRIX viewproj;	// ビュープロジェクション(カメラ)
	DirectX::XMMATRIX shadow;	// 影
	DirectX::XMMATRIX lightVP;	// ビュープロジェクション(光源)
	DirectX::XMVECTOR lightPos;	// 光源座標
};
class PMDLoder;
class PMDResource;
class VMDLoder;
struct PMDBone;
class TexManager;
class PMDActor
{
public:
	PMDActor(ComPtr<ID3D12Device>& dev, const char* path, DirectX::XMFLOAT3 pos);
	void CreatePMDModelTexture();
	PMDLoder& GetPMDModel();
	PMDResource& GetPMDResource();
	const Textures& GetTextures(std::string key);

	/// <summary>
	/// モデルを描画する
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void DrawModel(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	/// <summary>
	/// マテリアルバッファビュー作成@デフォルトテクスチャをどうにかしたら変更
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateMaterialBufferView();

	/// <summary>
	/// ボーンバッファ作成
	/// </summary>
	bool CreateBoneBuffer();

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
	/// ボーンを更新
	/// </summary>
	void UpdateBones(int currentFrameNo);

	void Update(float delta);

	BasicMatrix& GetBasicMarix();

	D3D12_VERTEX_BUFFER_VIEW& GetVbView();	// 頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW& GetIbView();	// インデックスバッファビュー

private:
	
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
	/// 頂点バッファを生成(してCPU側の頂点情報をコピー)
	/// </summary>
	void CreateVertexBufferView();

	/// <summary>
	/// インデックスバッファを生成
	/// </summary>
	void CreateIndexBufferView();

	/// <summary>
	/// 基本的なバッファ作成
	/// </summary>
	/// <param name="size"></param>
	/// <param name="heapType"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

	/// <summary>
	/// ベジェ補間を使ってxからyを得る
	/// 媒介変数tをxから求めて
	/// yを求める
	/// ベジェデータが二つなのは始点終点両方とも0だから
	/// </summary>
	/// <param name="x">入力値X</param>
	/// <returns>出力値Y</returns>
	float CalucurateFromBezier(float x, const DirectX::XMFLOAT2 bz[2], size_t n = 8);

	/// <summary>
	/// ボーンの返還を末端まで影響させる再帰関数
	/// </summary>
	/// <param name="bones">ボーン情報</param>
	/// <param name="mats"></param>
	/// <param name="idx"></param>
	void RecursiveCalucurate(const std::vector<PMDBone>& bones, std::vector<DirectX::XMMATRIX>& mats, int idx);

	// ----変数----
	ComPtr<ID3D12Device> dev_;
	
	// PMDモデル
	std::shared_ptr<PMDLoder> pmdModel_;
	std::unordered_map<std::string, Textures>textures_;

	ComPtr<ID3D12Resource>vertexBuffer;
	ComPtr<ID3D12Resource>indexBuffer;

	// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};	// 頂点バッファビュー
	// インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// インデックスバッファビュー

	// マテリアル
	ComPtr<ID3D12Resource> materialBuffer_;			// マテリアル用バッファ
	ComPtr<ID3D12DescriptorHeap> materialDescHeap_;	// マテリアル用ディスクリプタヒープ

	// リソース管理
	std::shared_ptr<PMDResource> pmdResource_;

	// テクスチャ管理
	//std::shared_ptr<TexManager>texManager_;

	// ボーンバッファ
	ComPtr<ID3D12Resource> boneBuffer_;			// ボーン用バッファ
	ComPtr<ID3D12DescriptorHeap> boneDescHeap_;			// ボーン用ディスクリプタヒープ
	std::unordered_map<std::string, uint16_t>boneTable_;
	DirectX::XMMATRIX* mappedBone_ = nullptr;

	// 定数バッファ
	ComPtr<ID3D12Resource> transformBuffer_;	// 定数バッファ
	std::shared_ptr<BasicMatrix> mappedBasicMatrix_;	// map中の基本定数バッファ
	// モーションデータ
	std::shared_ptr<VMDLoder>vmdMotion_;

	DirectX::XMFLOAT3 pos_;
	float angle_;

	float frame_;
};