#pragma once
#include <d3d12.h>
// Effekseer
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
class EffectManager
{
public:
	EffectManager(ID3D12Device* dev, ID3D12CommandQueue* cmdQue);

	void Init(ID3D12Device* dev, ID3D12CommandQueue* cmdQue);
	void Update(float delta, ID3D12GraphicsCommandList* cmdList);
	void AddEffect();
	void PlayEffect();
private:
	// Effekseer
	EffekseerRenderer::Renderer* efkRenderer_;
	Effekseer::Manager* efkManager_;
	EffekseerRenderer::SingleFrameMemoryPool* efkSfMemoryPool_;
	EffekseerRenderer::CommandList* efkCmdList_;
	Effekseer::Effect* effect_;
	Effekseer::Handle efkHandle_ = 0;
};

