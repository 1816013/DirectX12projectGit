#pragma once
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <d3dx12.h>
#include <unordered_map>

using Microsoft::WRL::ComPtr;

class PMDLoder;
class PMDActor
{
public:
	PMDActor(ID3D12Device* dev);
	void LoadModel(const char* path);
	void CreatePMDModelTexture();
	PMDLoder& GetPMDModel();
private:
	bool CreateTexture(const std::wstring& path, ComPtr<ID3D12Resource>& res);

	ComPtr<ID3D12Device> dev_;

	// PMDモデル
	std::shared_ptr<PMDLoder> pmdModel_;
	std::unordered_map<std::wstring, ID3D12Resource*>textureResource_;

	std::vector<ComPtr<ID3D12Resource>> texBuffers_;	// テクスチャリソース
	std::vector<ComPtr<ID3D12Resource>> sphBuffers_;	// sphテクスチャリソース
	std::vector<ComPtr<ID3D12Resource>> spaBuffers_;	// sphテクスチャリソース
	std::vector<ComPtr<ID3D12Resource>> toonBuffers_;	// toonテクスチャリソース
};

