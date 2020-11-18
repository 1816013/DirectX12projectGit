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
	std::vector<BuffType>types_;	// リソースの順番
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
	/// ルートシグネチャ生成
	/// </summary>
	/// <param name="plsDesc">パイプラインステートデスク</param>
	void CreateRootSignature();

	/// <summary>
	/// パイプラインステートオブジェクト作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreatePipelineState();

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSig_ = nullptr;
	PMDResources res_;
	ComPtr<ID3D12Device> dev_;
};

