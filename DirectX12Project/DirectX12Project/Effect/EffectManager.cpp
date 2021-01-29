#include "EffectManager.h"
#include "../Application.h"
#include <DirectXMath.h>

using namespace DirectX;

EffectManager::EffectManager(ID3D12Device& dev, ID3D12CommandQueue& cmdQue)
{
	// レンダラー作成
	auto formats = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer_ = EffekseerRendererDX12::Create(&dev, &cmdQue, 2,
		&formats,
		1,
		DXGI_FORMAT_UNKNOWN,
		false,
		8000);

	// メモリプールの作成
	efkSfMemoryPool_ = EffekseerRendererDX12::CreateSingleFrameMemoryPool(efkRenderer_);

	// コマンドリストの作成
	efkCmdList_ = EffekseerRendererDX12::CreateCommandList(efkRenderer_, efkSfMemoryPool_);

	// エフェクトマネージャ作成
	efkManager_ = Effekseer::Manager::Create(8000);

	// レンダラセット
	efkManager_->SetSpriteRenderer(efkRenderer_->CreateSpriteRenderer());
	efkManager_->SetRibbonRenderer(efkRenderer_->CreateRibbonRenderer());
	efkManager_->SetRingRenderer(efkRenderer_->CreateRingRenderer());
	efkManager_->SetTrackRenderer(efkRenderer_->CreateTrackRenderer());
	efkManager_->SetModelRenderer(efkRenderer_->CreateModelRenderer());

	// ローダーセット
	efkManager_->SetTextureLoader(efkRenderer_->CreateTextureLoader());
	efkManager_->SetModelLoader(efkRenderer_->CreateModelLoader());
	efkManager_->SetMaterialLoader(efkRenderer_->CreateMaterialLoader());

	effect_ = Effekseer::Effect::Create(efkManager_, u"Resource/Effect/Laser01.efk");

	assert(effect_ != nullptr);
}

void EffectManager::PlayEffect()
{
	efkManager_->Play(effect_, 0, 0, 0);
}

void EffectManager::UpdateEffekseer(ID3D12GraphicsCommandList& cmdList)
{
	// カメラ行列(ビュー行列)
	XMMATRIX view = XMMatrixLookAtRH(
		{ 0.0f, 10.0f, 30.0f, 1.0f },	// 視点
		{ 0.0f, 10.0f, 0.0f, 1.0f },		// 注視店
		{ 0.0f, 1.0f, 0.0f,1.0f });		// 上(仮の上)
	
	auto wSize = Application::GetInstance().GetWindowSize();
	// プロジェクション行列(パースペクティブ行列or射影行列)
	XMMATRIX proj = XMMatrixPerspectiveFovRH(XM_PIDIV4, // 画角(FOV)
		static_cast<float>(wSize.width) / static_cast<float>(wSize.height),
		0.1f,	// ニア(近い)
		1000.0f);	//　ファー(遠い)
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

	//efkRenderer_->SetProjectionMatrix(
	//	::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)wSize.width / (float)wSize.height, 1.0f, 500.0f));

	//// Specify a camera matrix
	//// カメラ行列を設定
	//efkRenderer_->SetCameraMatrix(
	//	::Effekseer::Matrix44().LookAtRH({ 10,5,20 }, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	// カメラ行列を設定
	efkRenderer_->SetCameraMatrix(cameraMat);
	// 投影行列を設定
	efkRenderer_->SetProjectionMatrix(projMat);

	efkSfMemoryPool_->NewFrame();
	EffekseerRendererDX12::BeginCommandList(efkCmdList_, &cmdList);
	efkRenderer_->SetCommandList(efkCmdList_);

	efkManager_->Update();
	efkRenderer_->BeginRendering();
	efkManager_->Draw();
	efkRenderer_->EndRendering();
	efkRenderer_->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(efkCmdList_);
}