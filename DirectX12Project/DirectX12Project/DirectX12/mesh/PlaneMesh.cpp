#include "PlaneMesh.h"
#include <array>

using namespace std;
using namespace DirectX;

PlaneMesh::PlaneMesh(ID3D12Device* dev, const DirectX::XMFLOAT3& pos, float width, float depth)
{
	array<PrimVertex, 8>vertices({
		PrimVertex(XMFLOAT3(pos.x - width / 2, pos.y, pos.z + depth / 2),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT2(0,0)), // 左下
		PrimVertex(XMFLOAT3(pos.x - width / 2, pos.y, pos.z - depth / 2),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT2(0,1)), // 左上
		PrimVertex(XMFLOAT3(pos.x + width / 2, pos.y, pos.z + depth / 2),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT2(1,0)), // 右下
		PrimVertex(XMFLOAT3(pos.x + width / 2, pos.y, pos.z - depth / 2),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT2(1,1)), // 右上


		PrimVertex(XMFLOAT3(pos.x + width / 2, pos.y, pos.z + depth / 2),XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT2(1,1)),
		PrimVertex(XMFLOAT3(pos.x + width / 2, pos.y, pos.z - depth / 2),XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT2(1,1)),
		PrimVertex(XMFLOAT3(pos.x + width / 2, pos.y+50, pos.z + depth / 2),XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT2(1,1)),
		PrimVertex(XMFLOAT3(pos.x + width / 2, pos.y+50, pos.z - depth / 2),XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT2(1,1))
		});
	
	CreateVertexBaffer(vertices, dev);
}

void PlaneMesh::CreateVertexBaffer(std::array<PrimVertex, 8>& vertices, ID3D12Device* dev)
{
	unsigned int indices[] = { 0, 1, 2, 1, 3, 2 , 4, 5, 6, 5, 7, 6 };
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
	// 確保する用途(Resource)に関する設定
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(vertices[0]));

	auto result = dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuffer_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	result = dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(indicesBuffer_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	//頂点データ転送
	PrimVertex* mappedData = nullptr;
	result = vertBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(vertices.begin(), vertices.end(), mappedData);
	vertBuffer_->Unmap(0, nullptr);

	// 頂点バッファビュー
	vbView_.BufferLocation = vertBuffer_->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices[0]);
	vbView_.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(vertices[0]));

	//インデックスデータ転送
	auto forType = indices;
	decltype(forType) mappedIdxData = nullptr;
	result = indicesBuffer_->Map(0, nullptr, (void**)&mappedIdxData);
	assert(SUCCEEDED(result));
	std::copy(begin(indices), end(indices), mappedIdxData);
	indicesBuffer_->Unmap(0, nullptr);
	// 頂点バッファビュー
	ibView_.BufferLocation = indicesBuffer_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R32_UINT;
	ibView_.SizeInBytes = static_cast<UINT>(sizeof(indices));
}

PlaneMesh::~PlaneMesh()
{
}

void PlaneMesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView_);
	cmdList->IASetIndexBuffer(&ibView_);
	cmdList->DrawIndexedInstanced(12, 1, 0, 0, 0);
}
