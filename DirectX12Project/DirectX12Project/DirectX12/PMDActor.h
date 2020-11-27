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
	/// ���f����`�悷��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void DrawModel(ComPtr<ID3D12GraphicsCommandList>& cmdList);
private:
	// ��{�}�e���A��
	struct BasicMaterial
	{
		DirectX::XMFLOAT3 diffuse;
		float alpha;
		DirectX::XMFLOAT3 speqular;
		float speqularity;
		DirectX::XMFLOAT3 ambient;
	};
	/// <summary>
	/// ���_�o�b�t�@�𐶐�(����CPU���̒��_�����R�s�[)
	/// </summary>
	void CreateVertexBufferView();

	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�𐶐�
	/// </summary>
	void CreateIndexBufferView();

	/// <summary>
	/// ��{�I�ȃo�b�t�@�쐬
	/// </summary>
	/// <param name="size"></param>
	/// <param name="heapType"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

	/// <summary>
	/// �}�e���A���o�b�t�@�r���[�쐬
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateMaterialBufferView();

	// ----�ϐ�----
	ComPtr<ID3D12Device> dev_;
	
	// PMD���f��
	std::shared_ptr<PMDLoder> pmdModel_;
	std::unordered_map<std::string, Textures>textures_;

	ComPtr<ID3D12Resource>vertexBuffer;
	ComPtr<ID3D12Resource>indexBuffer;

	// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};	// ���_�o�b�t�@�r���[
	// �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// �C���f�b�N�X�o�b�t�@�r���[

	// �}�e���A��
	ComPtr<ID3D12Resource> materialBuffer_;			// �}�e���A���p�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> materialDescHeap_;	// �}�e���A���p�f�B�X�N���v�^�q�[�v

	// ���\�[�X�Ǘ�
	std::shared_ptr<PMDResource> pmdResource_;

	// �e�N�X�`���Ǘ�
	std::shared_ptr<TexManager>texManager_;
};

