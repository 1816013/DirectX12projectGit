#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <d3dx12.h>

using Microsoft::WRL::ComPtr;

class PMDModel;
/// <summary>
/// DirectX12�̏��������̔ώG�ȂƂ�����܂Ƃ߂��N���X
/// </summary>
class Dx12Wrapper
{
public:
	Dx12Wrapper();
	~Dx12Wrapper();
	/// <summary>
	/// DirectX12�̏��������s��
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool Init(HWND hwnd);

	/// <summary>
	/// DirectX12�̍X�V���s��
	/// </summary>
	/// <returns>true:OK false:�G���[</returns>
	bool Update();

	/// <summary>
	/// �㏈��(�f�o�C�X�������[�X������)
	/// </summary>
	void Terminate();
private:


	
	// ��{�s��
	struct BasicMatrix
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX viewproj;

	};
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
	/// DirectX�̋@�\���x�����m�F����
	/// </summary>
	/// <returns>true:�����@false:���s</returns>
	bool CheckFeatureLevel();

	/// <summary>
	/// �e��R�}���h�ޏ�����(cmdAllocator_, cmdList_, cmdQue_ )
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool InitCommandSet();

	/// <summary>
	/// �X���b�v�`�F�C���쐬
	/// </summary>
	/// <param name="hwnd">�E�C���h�E�n���h��</param>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateSwapChain(const HWND& hwnd);

	/// <summary>
	/// �t�F���X�쐬(�X���b�h�Z�[�t�ɕK�v)
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateFence();

	/// <summary>
	/// RenderTargetDescriptorHeap�̍쐬
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateRenderTargetDescriptorHeap();

	/// <summary>
	/// ���_�o�b�t�@�𐶐�(����CPU���̒��_�����R�s�[)
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�𐶐�
	/// </summary>
	void CreateIndexBuffer();

	/// <summary>
	/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreatePipelineState();

	/// <summary>
	/// ���[�g�V�O�l�`������
	/// </summary>
	/// <param name="plsDesc">�p�C�v���C���X�e�[�g�f�X�N</param>
	void CreateRootSignature(D3D12_GRAPHICS_PIPELINE_STATE_DESC& plsDesc);

	/// <summary>
	/// �r���[�|�[�g�ƃV�U�[��`������
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool InitViewRect();

	using P_Resouse_t = ID3D12Resource*;

	/// <summary>
	/// �e�N�X�`���̍쐬(DXLIB�ł���LoadGraph())
	/// </summary>
	/// <param name="path"></param>
	/// <param name="res"></param>
	/// <returns>true:���� false:���s</returns>
	bool CreateTexture(const std::wstring& path, P_Resouse_t& res);

	/// <summary>
	/// ���ƍ��̃e�N�X�`���쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateMonoTexture();
	
	/// <summary>
	/// �O���f�[�V�����e�N�X�`���쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateGradationTexture();

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
	/// �[�x�o�b�t�@�r���[�쐬
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateDepthBufferView();

	/// <summary>
	/// �}�e���A���o�b�t�@�r���[�쐬
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateMaterialBufferView();

	/// <summary>
	/// �G���[�����o�͂ɕ\��
	/// </summary>
	/// <param name="errBlob">�G���[���</param>
	void OutputFromErrorBlob(ID3DBlob* errBlob);

	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

	ComPtr<ID3D12Device> dev_ = nullptr;
	ComPtr<ID3D12CommandAllocator> cmdAllocator_ = nullptr;
	ComPtr<ID3D12GraphicsCommandList> cmdList_ = nullptr;
	ComPtr<ID3D12CommandQueue> cmdQue_ = nullptr;
	ComPtr<IDXGIFactory7> dxgi_ = nullptr;
	ComPtr<IDXGISwapChain4> swapchain_ = nullptr;
	
	std::vector<ComPtr<ID3D12Resource>> bbResouces = {};	// ����ʃ��\�[�X

	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;	// �����_�[�^�[�Q�b�g�r���[�q�[�v
	ComPtr<ID3D12Fence1> fence_ = nullptr;// �t�F���X�I�u�W�F�N�g(CPUGPU�����ɕK�v)
	uint64_t fenceValue_ = 0;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};	// ���_�o�b�t�@�r���[
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// �C���f�b�N�X�o�b�t�@�r���[

	// �p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	// �V�F�[�_
	ComPtr<ID3D10Blob> vertexShader_ = nullptr;
	ComPtr<ID3D10Blob> pixelShader_ = nullptr;

	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSig_ = nullptr;// ���[�g�V�O�l�N�`��

	// �r���[�|�[�g
	D3D12_VIEWPORT viewPort_ = {};
	// �V�U�[��`
	D3D12_RECT scissorRect_ = {};

	// ���\�[�X
	ComPtr<ID3D12DescriptorHeap> resViewHeap_ = nullptr;	// ���\�[�X�r���[�p�f�B�X�N���v�^�q�[�v
	// �e�N�X�`��
	std::vector<ComPtr<ID3D12Resource>> texBuffers_;	// �e�N�X�`�����\�[�X
	std::vector<ComPtr<ID3D12Resource>> sphBuffers_;	// sph�e�N�X�`�����\�[�X
	std::vector<ComPtr<ID3D12Resource>> spaBuffers_;	// sph�e�N�X�`�����\�[�X
	std::vector<ComPtr<ID3D12Resource>> toonBuffers_;	// toon�e�N�X�`�����\�[�X
	ComPtr<ID3D12Resource> whiteTex_; // ���e�N�X�`��
	ComPtr<ID3D12Resource> blackTex_; // ���e�N�X�`��
	ComPtr<ID3D12Resource> gradTex_; // �O���f�[�V�����e�N�X�`��


	
	// map���̊�{�}�e���A��
	std::shared_ptr<BasicMatrix> mappedBasicMatrix_;

	// PMD���f��
	std::shared_ptr<PMDModel> pmdModel_;

	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> transformBuffer_;	// �萔�o�b�t�@

	// �[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12DescriptorHeap> depthDescHeap_;

	// �}�e���A���o�b�t�@
	ComPtr<ID3D12Resource> materialBuffer_;			// �}�e���A���p�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> materialDescHeap_;	// �}�e���A���p�f�B�X�N���v�^�q�[�v
	
	
};