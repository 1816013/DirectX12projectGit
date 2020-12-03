#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>

using Microsoft::WRL::ComPtr;
class PrimitiveMesh;
class PlaneMesh;
class PrimitiveManager
{
public:
	PrimitiveManager(ComPtr<ID3D12Device>dev);
	void CreatePipeline();
	~PrimitiveManager();
	
	std::shared_ptr<PlaneMesh>CreatePlane(const DirectX::XMFLOAT3& pos, float width, float depth);

	void Draw(ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* descHeap);
private:
	std::vector<std::shared_ptr<PrimitiveMesh>>primitives_;
	ComPtr<ID3D12PipelineState>pipeline_;
	ComPtr<ID3D12RootSignature>rootSig_;
	ComPtr<ID3D12Device>dev_;
};

