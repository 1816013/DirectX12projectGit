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

//enum class ColTexType
//{
//	White,
//	Black,
//	Grad,
//	Max
//};

class PMDLoder;
class PMDResource;
class TexManager;
class PMDActor
{
public:
	PMDActor(ComPtr<ID3D12Device>& dev, const char* path);
	void CreatePMDModelTexture();
	PMDLoder& GetPMDModel();
	PMDResource& GetPMDResource();
	const Textures& GetTextures(std::string key);

	/// <summary>
	/// モデルを描画する
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void DrawModel(ComPtr<ID3D12GraphicsCommandList>& cmdList);
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
	/// マテリアルバッファビュー作成
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool CreateMaterialBufferView();

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
	std::shared_ptr<TexManager>texManager_;
};

