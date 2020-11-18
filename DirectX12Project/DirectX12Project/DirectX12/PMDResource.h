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
	struct Resource
	{
		ID3D12Resource* resource;
		int size;
	};
	std::vector<Resource> resources_;
	ComPtr<ID3D12DescriptorHeap>descHeap_;
	int strideBytes_;
	void Init(ID3D12Device* dev, std::vector<BuffType>types);
	void AddBuffers(ID3D12Resource* res, int size = 0);
};
using PMDResources = std::array<PMDResourceBinding, static_cast<int>(GroopType::MAX)>;
class PMDResource
{
public:
	PMDResource(ID3D12Device* dev);
	void Build(const std::vector<GroopType> groopType);
	PMDResourceBinding& GetGroops(GroopType groopType);
	void SetPMDState(ID3D12GraphicsCommandList& cmdList);
	ComPtr<ID3D12RootSignature> GetRootSignature();
private:
	/// <summary>
	/// ���[�g�V�O�l�`������
	/// </summary>
	/// <param name="plsDesc">�p�C�v���C���X�e�[�g�f�X�N</param>
	void CreateRootSignature();

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

