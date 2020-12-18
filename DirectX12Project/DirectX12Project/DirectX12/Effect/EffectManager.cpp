#include "EffectManager.h"
#include <cassert>
#include <DirectXMath.h>

using namespace DirectX;

EffectManager::EffectManager(ID3D12Device* dev, ID3D12CommandQueue* cmdQue)
{
	Init(dev, cmdQue);
}

void EffectManager::Init(ID3D12Device* dev, ID3D12CommandQueue* cmdQue)
{
	// �����_���[�쐬
	auto formats = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer_ = EffekseerRendererDX12::Create(dev, cmdQue, 2,
		&formats,
		1,
		DXGI_FORMAT_UNKNOWN,
		false,
		8000);

	// �������v�[���̍쐬
	efkSfMemoryPool_ = EffekseerRendererDX12::CreateSingleFrameMemoryPool(efkRenderer_);

	// �R�}���h���X�g�̍쐬
	efkCmdList_ = EffekseerRendererDX12::CreateCommandList(efkRenderer_, efkSfMemoryPool_);

	// �G�t�F�N�g�}�l�[�W���쐬
	efkManager_ = Effekseer::Manager::Create(8000);

	// �����_���Z�b�g
	efkManager_->SetSpriteRenderer(efkRenderer_->CreateSpriteRenderer());
	efkManager_->SetRibbonRenderer(efkRenderer_->CreateRibbonRenderer());
	efkManager_->SetRingRenderer(efkRenderer_->CreateRingRenderer());
	efkManager_->SetTrackRenderer(efkRenderer_->CreateTrackRenderer());
	efkManager_->SetModelRenderer(efkRenderer_->CreateModelRenderer());

	// ���[�_�[�Z�b�g
	efkManager_->SetTextureLoader(efkRenderer_->CreateTextureLoader());
	efkManager_->SetModelLoader(efkRenderer_->CreateModelLoader());
	efkManager_->SetMaterialLoader(efkRenderer_->CreateMaterialLoader());

	effect_ = Effekseer::Effect::Create(efkManager_, u"Resource/Effect/depthtest.efk");
	//efkHandle_ = efkManager_->Play(effect_, 0, 10, 0);
	assert(effect_ != nullptr);
}

void EffectManager::Update(float delta, ID3D12GraphicsCommandList* cmdList)
{
	// �J�����s��(�r���[�s��)
	XMMATRIX view = XMMatrixLookAtRH(
		{ 0.0f, 10.0f, 30.0f, 1.0f },	// ���_
		{ 0.0f, 10.0f, 0.0f, 1.0f },		// �����X
		{ 0.0f, 1.0f, 0.0f,1.0f });		// ��(���̏�)
	//auto wSize = Application::GetInstance().GetWindowSize();
	// �v���W�F�N�V�����s��(�p�[�X�y�N�e�B�u�s��or�ˉe�s��)
	XMMATRIX proj = XMMatrixPerspectiveFovRH(XM_PIDIV4, // ��p(FOV)
		static_cast<float>(1280) / static_cast<float>(720),
		0.1f,	// �j�A(�߂�)
		1000.0f);	//�@�t�@�[(����)
	Effekseer::Matrix44 cameraMat;
	Effekseer::Matrix44 projMat;

	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			cameraMat.Values[j][i] = view.r[j].m128_f32[i];
			projMat.Values[j][i] = proj.r[j].m128_f32[i];
		}
	}
	efkRenderer_->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)1280 / (float)720, 1.0f, 500.0f));

	// Specify a camera matrix
	// �J�����s���ݒ�
	efkRenderer_->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH({10,5,20}, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	// �J�����s���ݒ�
	efkRenderer_->SetCameraMatrix(cameraMat);
	// ���e�s���ݒ�
	efkRenderer_->SetProjectionMatrix(projMat);

	efkSfMemoryPool_->NewFrame();
	EffekseerRendererDX12::BeginCommandList(efkCmdList_, cmdList);
	efkRenderer_->SetCommandList(efkCmdList_);

	efkManager_->Update(delta);
	efkManager_->AddLocation(efkHandle_, Effekseer::Vector3D(0.2f, 0.0f, 0.0f)* delta);
	efkRenderer_->BeginRendering();
	efkManager_->Draw();
	efkRenderer_->EndRendering();
	efkRenderer_->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(efkCmdList_);
}

void EffectManager::AddEffect()
{
}

void EffectManager::PlayEffect()
{
	if (efkManager_->Exists(efkHandle_))
	{
		efkManager_->StopEffect(efkHandle_);
	}
	efkHandle_ = efkManager_->Play(effect_, 0, 10, 0);
}
