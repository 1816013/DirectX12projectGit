#pragma once
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <d3dx12.h>
#include <unordered_map>

using Microsoft::WRL::ComPtr;
using Textures = std::vector<ComPtr<ID3D12Resource>>;

class PMDLoder;
class PMDResource;
class PMDActor
{
public:
	PMDActor(ID3D12Device* dev);
	void LoadModel(const char* path);
	void CreatePMDModelTexture();
	PMDLoder& GetPMDModel();
	const Textures& GetTextures(std::string key);
	void DrawModel(ID3D12GraphicsCommandList& cmdList);
private:
	bool CreateTexture(const std::wstring& path, ComPtr<ID3D12Resource>& res);

	/// <summary>
	/// バーテクスバッファ作成pmd以外でも使うかもその時は親クラスから継承
	/// </summary>
	void CreateVertexBufferView();
	/// <summary>
	/// インデックスバッファ作成pmd以外でも使うかもその時は親クラスから継承
	/// </summary>
	void CreateIndexBufferView();

	/// <summary>
	/// 基本的なバッファ作成
	/// </summary>
	/// <param name="size"></param>
	/// <param name="heapType"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

	// ----変数----
	ComPtr<ID3D12Device> dev_;
	std::unordered_map<std::wstring, ID3D12Resource*>textureResource_;
	// PMDモデル
	std::shared_ptr<PMDLoder> pmdModel_;
	std::unordered_map<std::string, Textures>textures_;

	// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};	// 頂点バッファビュー
	// インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// インデックスバッファビュー

	std::shared_ptr<PMDResource> pmdResource_;
};

