#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <memory>
#include <string>
#include <d3dx12.h>
#include <array>

using Microsoft::WRL::ComPtr;

enum class GroopType
{
	TRANSFORM,
	MATERIAL,
	MAX
};
enum class BuffType
{
	CBV,
	SRV,
	MAX
};
struct PMDResourceBinding
{
	std::vector<BuffType>types_;	// ���\�[�X�̏���
	std::vector<ID3D12Resource*> resources_;
	ComPtr<ID3D12DescriptorHeap>descHeap_;
	void Init(ID3D12Device* dev, std::vector<BuffType>types);
	void AddBuffers(ID3D12Resource* res);
};
using PMDResources = std::array<PMDResourceBinding, static_cast<int>(GroopType::MAX)>;
class PMDResource
{
public:
	PMDResource(ID3D12Device* dev);
	void Reset(GroopType groopType);
	void Build(const std::vector<GroopType> groopType);
	PMDResourceBinding& GetGroops(GroopType groopType);
	void SetPMDState(ID3D12GraphicsCommandList& cmdList);
	ID3D12PipelineState& GetPipelineState();
private:
	/// <summary>
	/// ���[�g�V�O�l�`������
	/// </summary>
	/// <param name="plsDesc">�p�C�v���C���X�e�[�g�f�X�N</param>
	ComPtr<ID3D12RootSignature> CreateRootSignature();

	/// <summary>
	/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreatePipelineState();

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSig_ = nullptr;
	PMDResources res_;
	ComPtr<ID3D12Device> dev_;
};

