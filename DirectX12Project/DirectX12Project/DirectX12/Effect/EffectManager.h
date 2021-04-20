#pragma once
#include <unordered_map>
// Effekseer
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
class EffectManager
{
public:
	EffectManager(ID3D12Device* dev, ID3D12CommandQueue* cmdQue);
	~EffectManager();
	void Init(ID3D12Device* dev, ID3D12CommandQueue* cmdQue);
	void Update(float delta, ID3D12GraphicsCommandList* cmdList);
	void AddEffect(std::string name);
	void PlayEffect(std::string name, Effekseer::Vector3D pos);
private:
	// Effekseer
	EffekseerRenderer::Renderer* efkRenderer_;
	Effekseer::Manager* efkManager_;
	EffekseerRenderer::SingleFrameMemoryPool* efkSfMemoryPool_;
	EffekseerRenderer::CommandList* efkCmdList_;
	std::unordered_map<std::string, Effekseer::Effect*>effect_;
	std::unordered_map <std::string, Effekseer::Handle> efkHandle_;
};

