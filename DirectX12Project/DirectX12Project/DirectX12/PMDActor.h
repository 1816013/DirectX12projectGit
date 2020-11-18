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
	/// �o�[�e�N�X�o�b�t�@�쐬pmd�ȊO�ł��g���������̎��͐e�N���X����p��
	/// </summary>
	void CreateVertexBufferView();
	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�쐬pmd�ȊO�ł��g���������̎��͐e�N���X����p��
	/// </summary>
	void CreateIndexBufferView();

	/// <summary>
	/// ��{�I�ȃo�b�t�@�쐬
	/// </summary>
	/// <param name="size"></param>
	/// <param name="heapType"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

	// ----�ϐ�----
	ComPtr<ID3D12Device> dev_;
	std::unordered_map<std::wstring, ID3D12Resource*>textureResource_;
	// PMD���f��
	std::shared_ptr<PMDLoder> pmdModel_;
	std::unordered_map<std::string, Textures>textures_;

	// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};	// ���_�o�b�t�@�r���[
	// �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// �C���f�b�N�X�o�b�t�@�r���[

	std::shared_ptr<PMDResource> pmdResource_;
};

