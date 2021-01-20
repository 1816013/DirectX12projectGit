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

// ��{�s��
struct BasicMatrix
{
	DirectX::XMMATRIX world;	// ���[���h�s��(���f�����W)
	DirectX::XMMATRIX viewproj;	// �r���[�v���W�F�N�V����(�J����)
	DirectX::XMMATRIX shadow;	// �e
	DirectX::XMMATRIX lightVP;	// �r���[�v���W�F�N�V����(����)
	DirectX::XMVECTOR lightPos;	// �������W
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
	/// ���f����`�悷��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void DrawModel(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	/// <summary>
	/// �}�e���A���o�b�t�@�r���[�쐬@�f�t�H���g�e�N�X�`�����ǂ��ɂ�������ύX
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateMaterialBufferView();

	/// <summary>
	/// �{�[���o�b�t�@�쐬
	/// </summary>
	bool CreateBoneBuffer();

	/// <summary>
	/// ���\�[�X�̊�{�I�ȃf�B�X�N���v�^�쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateBasicDescriptors();

	/// <summary>
	/// ���W�ϊ��o�b�t�@�̍쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateTransformBuffer();
	/// <summary>
	/// �{�[�����X�V
	/// </summary>
	void UpdateBones(int currentFrameNo);

	void Update(float delta);

	BasicMatrix& GetBasicMarix();

	D3D12_VERTEX_BUFFER_VIEW& GetVbView();	// ���_�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW& GetIbView();	// �C���f�b�N�X�o�b�t�@�r���[

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
	/// �x�W�F��Ԃ��g����x����y�𓾂�
	/// �}��ϐ�t��x���狁�߂�
	/// y�����߂�
	/// �x�W�F�f�[�^����Ȃ͎̂n�_�I�_�����Ƃ�0������
	/// </summary>
	/// <param name="x">���͒lX</param>
	/// <returns>�o�͒lY</returns>
	float CalucurateFromBezier(float x, const DirectX::XMFLOAT2 bz[2], size_t n = 8);

	/// <summary>
	/// �{�[���̕Ԋ҂𖖒[�܂ŉe��������ċA�֐�
	/// </summary>
	/// <param name="bones">�{�[�����</param>
	/// <param name="mats"></param>
	/// <param name="idx"></param>
	void RecursiveCalucurate(const std::vector<PMDBone>& bones, std::vector<DirectX::XMMATRIX>& mats, int idx);

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
	//std::shared_ptr<TexManager>texManager_;

	// �{�[���o�b�t�@
	ComPtr<ID3D12Resource> boneBuffer_;			// �{�[���p�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> boneDescHeap_;			// �{�[���p�f�B�X�N���v�^�q�[�v
	std::unordered_map<std::string, uint16_t>boneTable_;
	DirectX::XMMATRIX* mappedBone_ = nullptr;

	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> transformBuffer_;	// �萔�o�b�t�@
	std::shared_ptr<BasicMatrix> mappedBasicMatrix_;	// map���̊�{�萔�o�b�t�@
	// ���[�V�����f�[�^
	std::shared_ptr<VMDLoder>vmdMotion_;

	DirectX::XMFLOAT3 pos_;
	float angle_;

	float frame_;
};