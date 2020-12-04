#include "PMDResource.h"
#include <cassert>
#include "../Common.h"

using namespace std;

PMDResource::PMDResource(ID3D12Device& dev) : dev_(&dev)
{
	res_[static_cast<int>(GroopType::TRANSFORM)].types_ = { BuffType::CBV, BuffType::CBV };
	res_[static_cast<int>(GroopType::MATERIAL)].types_ = {BuffType::CBV,BuffType::SRV,BuffType::SRV,BuffType::SRV,BuffType::SRV };
	res_[static_cast<int>(GroopType::DEPTH)].types_ = { BuffType::SRV };
}

void PMDResource::Build(const vector<GroopType>& groopType)
{	
	CreateResouses(groopType);
}

void PMDResource::CreateResouses(const std::vector<GroopType>& groopTypes)
{
	for (auto gType : groopTypes)
	{
		int resIdx = 0;
		auto buffers = res_[static_cast<int>(gType)].resources_;
		//ディスクリプタヒープ
		auto buffType = res_[static_cast<int>(gType)].types_;
		ComPtr<ID3D12DescriptorHeap> descHeap;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = static_cast<UINT>(buffers.size());
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		auto result = dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf()));
		res_[static_cast<int>(gType)].descHeap_ = descHeap;
		assert(SUCCEEDED(result));
		D3D12_GPU_VIRTUAL_ADDRESS gAddress;

		auto heapAddress = descHeap->GetCPUDescriptorHandleForHeapStart();
		auto heapSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		for (int i = 0; i < buffers.size(); ++i)
		{
			resIdx = static_cast<int>((i + buffType.size()) % buffType.size());
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			if (buffType[resIdx] == BuffType::CBV)
			{
				gAddress = buffers[i].resource->GetGPUVirtualAddress() + (buffers[i].size * (i / buffType.size()));
				auto cbDesc = buffers[i].resource->GetDesc();
				cbvDesc.BufferLocation = gAddress;
				cbvDesc.SizeInBytes = static_cast<UINT>(cbDesc.Width) == 256 ? static_cast<UINT>(cbDesc.Width) : static_cast<UINT>(cbDesc.Width / (buffers.size() / buffType.size()));
				dev_->CreateConstantBufferView(&cbvDesc, heapAddress);
				heapAddress.ptr += heapSize;
			}
			else if (buffType[resIdx] == BuffType::SRV)
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.PlaneSlice = 0;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				srvDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
				if (buffers[i].resource != nullptr)
				{
					srvDesc.Format = buffers[i].resource->GetDesc().Format;
				}
				if (srvDesc.Format == DXGI_FORMAT_R32_TYPELESS)
				{
					srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				}
				dev_->CreateShaderResourceView(
					buffers[i].resource,
					&srvDesc,
					heapAddress);
				heapAddress.ptr += heapSize;
			}

		}
	}
}

PMDResourceBinding& PMDResource::GetGroops(GroopType groopType)
{
	return res_[static_cast<int>(groopType)];
}

void PMDResource::SetPMDState(ID3D12GraphicsCommandList& cmdList)
{
}

void PMDResourceBinding::Init(std::vector<BuffType> types)
{
	types_ = types;
}

void PMDResourceBinding::AddBuffers(ID3D12Resource* res, int size)
{
	resources_.push_back({ res, size });
}
