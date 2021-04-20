#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <d3dx12.h>
#include <unordered_map>
#include <array>

// ��ŉ��Ƃ�����
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>


using Microsoft::WRL::ComPtr;

struct BoardConstBuffer
{
	DirectX::XMMATRIX proj; // �v���W�F�N�V����(3D��2D)64
	DirectX::XMMATRIX invProj; // �t�v���W�F�N�V����(2D��3D)64
	DirectX::XMFLOAT2 pos;	// �N���b�N���ꂽ���W 8
	float time;	// time 4
	// ���̌��ʂ�K�p���邩�ǂ���
	uint32_t ssaoActive;	// SSAO
	uint32_t bloomActive;	// ����
	uint32_t dofActive;	// ��ʊE�[�x
	uint32_t outLineN;	// �@���A�E�g���C��
	uint32_t outLineD;	// �[�x�A�E�g���C��
	uint32_t debug;	// �[�x�A�E�g���C��
	float bloomCol[3];
};

// �O���錾
class PMDActor;
class Renderer;
class PrimitiveManager;
struct BasicMatrix;
class EffectManager;
class CameraCtr;
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

	void CreatePrimitiveBufferView();

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
	/// �o���A�𒣂�R�}���h���X�g����ăo�b�N�o�b�t�@�ɑ΂���
	/// �������s��present����
	/// </summary>
	void DrawExcute();

	/// <summary>
	/// �������s���ďI���܂ő҂�
	/// </summary>
	void ExecuteAndWait();

	/// <summary>
	/// �㏈��(�f�o�C�X�������[�X������)
	/// </summary>
	void Terminate();

private:	
	/// <summary>
	/// DirectX�̋@�\���x�����m�F����
	/// </summary>
	/// <returns>true:�����@false:���s</returns>
	bool CreateDevice(IDXGIAdapter* adapter);

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
	/// �r���[�|�[�g�ƃV�U�[��`������
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool InitViewRect();

	/// <summary>
	/// �f�t�H���g�e�N�X�`���쐬
	/// </summary>
	/// <returns></returns>
	bool CreateDefaultTextures();

	/// <summary>
	/// �[�x�o�b�t�@�r���[�쐬
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool CreateDepthBufferView();

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
	/// �|�X�g�G�t�F�N�g��1�p�X�߂������_�����O����
	/// �e�N�X�`�������
	/// </summary>
	void CreateRenderTargetTexture();

	/// <summary>
	/// �|���̒��_���쐬
	/// </summary>
	void CreateBoardPolyVerts();

	/// <summary>
	/// �|���p�C�v���C���쐬
	/// </summary>
	void CreateBoardPipeline();

	/// <summary>
	/// �e�p�o�b�t�@�쐬
	/// </summary>
	void CreateShadowMapBufferAndView();

	/// <summary>
	/// �e�p�p�C�v���C������
	/// </summary>
	void CreateShadowPipeline();

	/// <summary>
	/// �e�`��
	/// </summary>
	/// <param name="mat"></param>
	void DrawShadow(BasicMatrix& mat);


	bool CreateSSAOPipeLine();

	ComPtr<ID3D12DescriptorHeap> CreateImguiDescriptorHeap();

	void CreateFontDescriptorHeap();

	HWND hwnd_;

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

	// �r���[�|�[�g
	D3D12_VIEWPORT viewPort_ = {};
	// �V�U�[��`
	D3D12_RECT scissorRect_ = {};

	// PMD���f���f�[�^�֘A	
	std::shared_ptr<Renderer>renderer_;
	std::vector<std::shared_ptr<PMDActor>>pmdActor_;

	// �[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuffer_;
	ComPtr<ID3D12DescriptorHeap> depthDescHeap_;

	// �o�b�N�o�b�t�@�C���f�b�N�X
	uint32_t bbIdx_;

	float clearCol[4] = { 0,0,0,0 };
	// �}���`�p�X�����_�����O
	// �|�X�g�G�t�F�N�g�����_�[�^�[�Q�b�g�p�f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> firstRtvHeap_ = nullptr;	
	ComPtr<ID3D12DescriptorHeap> firstSrvHeap_ = nullptr;
	//�|�X�g�G�t�F�N�g�p�e�N�X�`��
	std::array<ComPtr<ID3D12Resource>,3> rtTextures_;	// 0:�`��p,1:�@�� , 2:���P�x(hdr)

	// �ʏ�̕��������ɂȂ����o�b�t�@
	ComPtr<ID3D12PipelineState> shrinkPipeLine_ = nullptr;
	ComPtr<ID3D12Resource> rtShrinkForBloom_;	// �k���o�b�t�@(�u���[���ڂ����p)
	ComPtr<ID3D12Resource> rtShrinkForDof_;	// �k���o�b�t�@(��ʊE�[�x�p)

	// �A���r�G���g�I�N���[�W����(SSAO)
	ComPtr<ID3D12Resource>ssaoBuffer_;
	ComPtr<ID3D12PipelineState> ssaoPipeline_;


	// �|�����_
	// TRIANGLE_STRIP�ō��
	ComPtr<ID3D12Resource> boardPolyVerts_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW boardVBView_;
	ComPtr<ID3D12PipelineState> boardPipeLine_ = nullptr;
	ComPtr<ID3D12RootSignature> boardSig_ = nullptr;
	

	// �m�[�}���}�b�v�p
	ComPtr<ID3D12Resource> normalMapTex_ = nullptr;
	BoardConstBuffer* mappedBoardBuffer_;
	ComPtr<ID3D12Resource> boardConstBuffer_;	// �萔�o�b�t�@

	// �V���h�E�}�b�v�p
	ComPtr<ID3D12Resource> shadowDepthBuffer_;
	ComPtr<ID3D12DescriptorHeap> shadowDSVHeap_;
	ComPtr<ID3D12DescriptorHeap> shadowSRVHeap_;
	ComPtr<ID3D12PipelineState> shadowPipeline_ = nullptr;
	ComPtr<ID3D12RootSignature> shadowSig_ = nullptr;

	// ��
	std::shared_ptr<PrimitiveManager> primManager_;
	ComPtr<ID3D12DescriptorHeap>primitiveDescHeap_;

	// �G�t�F�N�g
	std::shared_ptr<EffectManager>efcMng_;

	// �J�����@
	std::unique_ptr<CameraCtr>cameraCtr_;

	// imgui�֘A
	ComPtr<ID3D12DescriptorHeap>imguiHeap_;
	float fov_ = 45.0f;		// ��ʊE�[�x
	float cNear_ = 0.1f;	// �N���b�v�p
	float cFar_ = 1000.0f;	// �N���b�v�p

	// DirectXTK�֘A
	// �t�H���g�\��
	ComPtr<ID3D12DescriptorHeap> fontDescHeap_;
	DirectX::GraphicsMemory* gMemory_;
	DirectX::SpriteFont* spriteFonts_;
	DirectX::SpriteBatch* spriteBatch_;

	// ���ԗp
	float oldTime = 0;
};