#pragma once
// Effekseer
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>

class EffectManager
{
public:
	EffectManager(ID3D12Device& dev, ID3D12CommandQueue& cmdQue);

	void PlayEffect();

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void UpdateEffekseer(ID3D12GraphicsCommandList& cmdList);

private:
	// Effekseer
	EffekseerRenderer::Renderer* efkRenderer_;
	Effekseer::Manager* efkManager_;
	EffekseerRenderer::SingleFrameMemoryPool* efkSfMemoryPool_;
	EffekseerRenderer::CommandList* efkCmdList_;
	Effekseer::Effect* effect_;
	Effekseer::Handle efkHandle_ = 0;


};

