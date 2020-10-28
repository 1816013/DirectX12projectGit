#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include "../BMPLoder/BmpLoder.h"

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
	bool CreateSwapChain(const HWND hwnd);

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
	/// �r���[�|�[�g�ƃV�U�[��`������
	/// </summary>
	/// /// <returns>true:���� false:���s</returns>
	bool InitViewRect();

	/// <summary>
	/// �e�N�X�`���̍쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateTexture();

	/// <summary>
	/// ���\�[�X�̊�{�I�ȃf�X�N���v�^�쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateBasicDescriptors();

	/// <summary>
	/// �萔�o�b�t�@�̍쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateConstantBuffer();

	/// <summary>
	/// �G���[�����o�͂ɕ\��
	/// </summary>
	/// <param name="errBlob">�G���[���</param>
	void OutputFromErrorBlob(ID3DBlob* errBlob);

	ID3D12Device* dev_ = nullptr;
	ID3D12CommandAllocator* cmdAllocator_ = nullptr;
	ID3D12GraphicsCommandList* cmdList_ = nullptr;	
	ID3D12CommandQueue* cmdQue_ = nullptr;	
	IDXGIFactory7* dxgi_ = nullptr;
	IDXGISwapChain4* swapchain_ = nullptr;
	
	std::vector<ID3D12Resource*> bbResouces = {};	// ����ʃ��\�[�X

	ID3D12DescriptorHeap* rtvHeap_ = nullptr;
	ID3D12Fence1* fence_ = nullptr;// �t�F���X�I�u�W�F�N�g(CPUGPU�����ɕK�v)
	uint64_t fenceValue_ = 0;

	// �o�b�t�@
	ID3D12Resource* vertexBuffer_ = nullptr;	// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};	// ���_�o�b�t�@�r���[
	ID3D12Resource* indexBuffer_ = nullptr;	// �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// �C���f�b�N�X�o�b�t�@�r���[

	// �p�C�v���C��
	ID3D12PipelineState* pipelineState_ = nullptr;

	// �V�F�[�_
	ID3D10Blob* vertexShader_ = nullptr;
	ID3D10Blob* pixelShader_ = nullptr;

	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootSig_ = nullptr;// ���[�g�V�O�l�N�`��

	// �r���[�|�[�g
	D3D12_VIEWPORT viewPort_ = {};
	// �V�U�[��`
	D3D12_RECT scissorRect_ = {};

	// ���\�[�X
	ID3D12DescriptorHeap* resViewHeap_ = nullptr;	// ���\�[�X�r���[�p�f�X�N���v�^�q�[�v
	// �e�N�X�`��
	ID3D12Resource* texBuffer_;	// �e�N�X�`�����\�[�X
	// �萔�o�b�t�@
	ID3D12Resource* constantBuffer_;	// �萔�o�b�t�@

	struct BasicMatrix
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX viewproj;
	};
	BasicMatrix* mappedBasicMatrix_;


	std::shared_ptr<PMDModel> pmdModel_;
};

