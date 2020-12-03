#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

struct PrimVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	PrimVertex() : pos(DirectX::XMFLOAT3(0,0,0)), 
		normal(DirectX::XMFLOAT3(0, 0, 0)),
		uv(DirectX::XMFLOAT2(0, 0)) {};
		
	PrimVertex(DirectX::XMFLOAT3 inp,
		DirectX::XMFLOAT3 innorm,
		DirectX::XMFLOAT2 inuv) :
		pos(inp), normal(innorm), uv(inuv) {};
};

class PrimitiveMesh
{
public:
	PrimitiveMesh() = default;
	virtual ~PrimitiveMesh() = default;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) = 0;	
};

