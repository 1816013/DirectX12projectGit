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
	/// �X�N���[�����N���A���ď�����悤�ɂ���
	/// </summary>
	void ClearDrawScreen();


	/// <summary>
	/// �R�}���h���X�g����ăo�b�N�o�b�t�@�ɑ΂���
	/// �揈�����s���I���܂ő҂�
	/// </summary>
	void DrawExcute();


	void DrawPMDModel();

	void ExecuteAndWait();

	/// <summary>
	/// �㏈��(�f�o�C�X�������[�X������)
	/// </summary>
	void Terminate();


	ID3D12Device* GetDevice();
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
	/// �r���[�|�[�g�ƃV�U�[��`������
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool InitViewRect();

	
	/// <summary>
	/// �P�F�e�N�X�`���쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateMonoColorTexture(ColTexType colType, const Color col);

	/// <summary>
	/// �O���f�[�V�����e�N�X�`���쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreateGradationTexture( const Size size);

	/// <summary>
	/// GPU�ɃA�b�v���[�h���邽�߂̏���
	/// </summary>
	/// <param name="size">�傫��</param>
	/// <param name="tex">�e�N�X�`���o�b�t�@</param>
	/// <param name="subResData"></param>
	void SetUploadTexure( D3D12_SUBRESOURCE_DATA& subResData, ColTexType colType);
	

	/// <summary>
	/// �f�t�H���g�e�N�X�`���쐬
	/// </summary>
	/// <returns></returns>
	bool CreateDefaultTextures();
	

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
	/// �{�[���o�b�t�@�쐬
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateBoneBuffer();

	/// <summary>
	/// �{�[�����X�V
	/// </summary>
	void UpdateBones(int currentFrameNo);

	/// <summary>
	/// �G���[�����o�͂ɕ\��
	/// </summary>
	/// <param name="errBlob">�G���[���</param>
	void OutputFromErrorBlob(ID3DBlob* errBlob);

	/// <summary>
	/// ��{�I�ȃo�b�t�@�쐬
	/// </summary>
	/// <param name="size"></param>
	/// <param name="heapType"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD);

	
	
	/// <summary>
	/// �{�[���̕Ԋ҂𖖒[�܂ŉe��������ċA�֐�
	/// </summary>
	/// <param name="bones">�{�[�����</param>
	/// <param name="mats"></param>
	/// <param name="idx"></param>
	void RecursiveCalucurate(const std::vector<PMDBone>& bones, std::vector<DirectX::XMMATRIX>& mats, int idx);

	/// <summary>
	/// �x�W�F��Ԃ��g����x����y�𓾂�
	/// �}��ϐ�t��x���狁�߂�
	/// y�����߂�
	/// �x�W�F�f�[�^����Ȃ͎̂n�_�I�_�����Ƃ�0������
	/// </summary>
	/// <param name="x">���͒lX</param>
	/// <returns>�o�͒lY</returns>
	float CalucurateFromBezier(float x, DirectX::XMFLOAT2 bz[2]);
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

	// �V�F�[�_
	ComPtr<ID3D10Blob> vertexShader_ = nullptr;
	ComPtr<ID3D10Blob> pixelShader_ = nullptr;

	// �r���[�|�[�g
	D3D12_VIEWPORT viewPort_ = {};
	// �V�U�[��`
	D3D12_RECT scissorRect_ = {};

	// ���\�[�X
	ComPtr<ID3D12DescriptorHeap> resViewHeap_ = nullptr;	// ���\�[�X�r���[�p�f�B�X�N���v�^�q�[�v
	// �e�N�X�`��
	std::vector<ComPtr<ID3D12Resource>>defTextures_;

	// map���̊�{�}�e���A��
	std::shared_ptr<BasicMatrix> mappedBasicMatrix_;

	//// PMD���f���f�[�^�֘A
	// �ꎞ�I�ɂ����ɒu��
	std::shared_ptr<PMDActor>pmdActor_;
	std::shared_ptr<PMDResource> pmdResource_;
	std::shared_ptr<VMDLoder>vmdMotion_;
	std::unordered_map<std::string, uint16_t>boneTable_;
	DirectX::XMMATRIX* mappedBone_ = nullptr;

	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> transformBuffer_;	// �萔�o�b�t�@

	// �[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12DescriptorHeap> depthDescHeap_;

	// �}�e���A���o�b�t�@
	ComPtr<ID3D12Resource> materialBuffer_;			// �}�e���A���p�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> materialDescHeap_;	// �}�e���A���p�f�B�X�N���v�^�q�[�v

	// �o�b�N�o�b�t�@�C���f�b�N�X
	uint32_t bbIdx_;

	// �{�[���o�b�t�@
	ComPtr<ID3D12Resource> boneBuffer_;			// �{�[���p�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> boneDescHeap_;			// �{�[���p�f�B�X�N���v�^�q�[�v

};