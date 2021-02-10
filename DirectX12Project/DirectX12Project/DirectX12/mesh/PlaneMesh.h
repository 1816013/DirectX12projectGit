#pragma once
#include "PrimitiveMesh.h"
using Microsoft::WRL::ComPtr;
class PlaneMesh : public PrimitiveMesh
{
public:
	PlaneMesh(ID3D12Device* dev, const DirectX::XMFLOAT3& pos, float width, float depth);
	~PlaneMesh();
	void Draw(ID3D12GraphicsCommandList* cmdList)override;
private:
	void CreateVertexBaffer(std::array<PrimVertex, 8>& vertices, ID3D12Device* dev);
	ComPtr<ID3D12Resource>vertBuffer_;
	ComPtr<ID3D12Resource>indicesBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	D3D12_INDEX_BUFFER_VIEW ibView_;

};

