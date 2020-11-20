#include "PMDActor.h"
#include "../PMDLoder/PMDLoder.h"
#include <Windows.h>
#include <DirectXTex.h>
#include "../Common/StrOperater.h"
#include "PMDResource.h"
using namespace DirectX;
using namespace std;

namespace
{
	string texTable[] = { "bmp", "spa", "sph", "toon" };
}

PMDActor::PMDActor(ID3D12Device* dev) : dev_(dev)
{
}

void PMDActor::LoadModel(const char* path)
{
	pmdModel_ = make_shared<PMDLoder>();
	
	pmdModel_->Load(path);
	pmdResource_ = make_shared<PMDResource>(dev_.Get());
	//CreateVertexBufferView();
	//CreateIndexBufferView();
}

bool PMDActor::CreateTexture(const std::wstring& path, ComPtr<ID3D12Resource>& res)
{
	auto it = textureResource_.find(path);
	if (it != textureResource_.end())
	{
		res = it->second;
		return true;
	}
	HRESULT result = S_OK;
	TexMetadata metaData = {};
	ScratchImage scratchImg = {};
	if (StrOperater::GetExtension(path) != L"tga")
	{
		result = LoadFromWICFile(path.c_str(), WIC_FLAGS_IGNORE_SRGB, &metaData, scratchImg);
	}
	else
	{
		result = LoadFromTGAFile(path.c_str(), TGA_FLAGS_IGNORE_SRGB, &metaData, scratchImg);
	}
	if (FAILED(result))
	{
		return false;
	}
	assert(SUCCEEDED(result));
	auto img = scratchImg.GetImage(0, 0, 0);// 生データ抽出

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = metaData.format;
	resDesc.Width = metaData.width;
	resDesc.Height = static_cast<UINT>(metaData.height);
	resDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	resDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;

	result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&res));
	assert(SUCCEEDED(result));

	result = res->WriteToSubresource(0,
		nullptr,
		img->pixels,	// 元データアドレス
		static_cast<UINT>(img->rowPitch),	// 1ラインサイズ
		static_cast<UINT>(img->slicePitch)	// 1枚サイズ
	);
	assert(SUCCEEDED(result));
	textureResource_[path] = res.Get();
	return true;
}

void PMDActor::CreatePMDModelTexture()
{
	auto modelPath = pmdModel_->GetModelPath();
	auto& paths = pmdModel_->GetTexturePaths();
	auto& toonPaths = pmdModel_->GetToonPaths();
	for (auto tex : texTable)
	{
		if (textures_[tex].size() == 0)
		{
			textures_[tex].resize(paths.size());
		}
	}
	Textures toonBuffers(paths.size());
	Textures matBuffers(paths.size());

	for (int i = 0; i < paths.size(); ++i)
	{
		if (toonPaths[i] != "")
		{
			string strToonPath = StrOperater::GetTextureFromModelAndTexPath(modelPath, toonPaths[i]);
					
			if (!CreateTexture(
				StrOperater::GetWideStringfromString(strToonPath),
				toonBuffers[i]))
			{
				strToonPath = "Resource/PMD/toon/" + toonPaths[i];
				auto result = CreateTexture(
					StrOperater::GetWideStringfromString(strToonPath),
					toonBuffers[i]);
				assert(result);
			}
			textures_["toon"][i] = toonBuffers[i];
			
		}
		if (paths[i] == "")
		{
			continue;
		}

		auto pathVec = StrOperater::SplitFileName(paths[i]);
		for (auto path : pathVec)
		{
			auto ext = StrOperater::GetExtension(path);
			if (ext != "spa" && ext != "sph")
			{
				ext = "bmp";
			}
			auto str = StrOperater::GetTextureFromModelAndTexPath(modelPath, path);
			auto result = CreateTexture(
				StrOperater::GetWideStringfromString(str),
				matBuffers[i]);
			
			assert(result);
			textures_[ext][i] = matBuffers[i];
		}
		
	}
	
}

PMDLoder& PMDActor::GetPMDModel()
{
	return *pmdModel_;
}

const Textures& PMDActor::GetTextures(std::string key)
{
	Textures* ret = nullptr;
	if (textures_[key].size() != 0)
	{
		ret = &textures_[key];
	}
	return *ret;
}

void PMDActor::DrawModel(ID3D12GraphicsCommandList& cmdList)
{
	//cmdList.SetPipelineState(pipelineState_.Get());
	cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList.IASetVertexBuffers(0, 1, &vbView_);
	cmdList.IASetIndexBuffer(&ibView_);

	//// 座標変換ヒープセット
	//auto resHeap = pmdResource_->GetGroops(GroopType::TRANSFORM).descHeap_.Get();
	//ID3D12DescriptorHeap* deskHeaps[] = { resHeap/*resViewHeap_.Get()*/ };
	//auto heapPos = /*resViewHeap_*/resHeap->GetGPUDescriptorHandleForHeapStart();
	//cmdList_->SetDescriptorHeaps(1, deskHeaps);
	//cmdList_->SetGraphicsRootDescriptorTable(0, heapPos);

	//// マテリアル&テクスチャヒープセット
	//auto material = GetPMDModel().GetMaterialData();
	//uint32_t indexOffset = 0;
	//auto matHeap = pmdResource_->GetGroops(GroopType::MATERIAL).descHeap_.Get();
	//ID3D12DescriptorHeap* matDeskHeaps[] = { matHeap };
	//cmdList_->SetDescriptorHeaps(1, matDeskHeaps);
	//auto materialHeapPos = matHeap->GetGPUDescriptorHandleForHeapStart();
	//const auto heapSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//for (auto& m : material)
	//{
	//	cmdList_->SetGraphicsRootDescriptorTable(1, materialHeapPos);
	//	auto indexNum = m.indexNum;

	//	cmdList_->DrawIndexedInstanced(
	//		indexNum,		// インデックス数
	//		1,				// インスタンス数
	//		indexOffset,	// インデックスオフセット
	//		0,				// 頂点オフセット
	//		0);				// インスタンスオフセット
	//	indexOffset += indexNum;
	//	materialHeapPos.ptr += static_cast<UINT64>(heapSize) * 5;
	//}
}

void PMDActor::CreateVertexBufferView()
{
	const auto& vertices = pmdModel_->GetVertexData();
	auto vertexBuffer = CreateBuffer(vertices.size() * sizeof(vertices[0]));
	//頂点データ転送
	PMDVertex* mappedData = nullptr;
	auto result = vertexBuffer->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(vertices.begin(), vertices.end(), mappedData);
	vertexBuffer->Unmap(0, nullptr);

	// 頂点バッファビュー
	vbView_.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices[0]);
	vbView_.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(vertices[0]));
}

void PMDActor::CreateIndexBufferView()
{
	const auto& indices = pmdModel_->GetIndexData();
	auto indexBuffer = CreateBuffer(indices.size() * sizeof(indices[0]));
	//インデックスデータ転送
	auto forType = indices.back();
	decltype(forType)* mappedData = nullptr;
	auto result = indexBuffer->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(indices.begin(), indices.end(), mappedData);
	indexBuffer->Unmap(0, nullptr);

	// インデックスビュー
	ibView_.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(indices[0]));
}

ComPtr<ID3D12Resource> PMDActor::CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType)
{
	ComPtr<ID3D12Resource>ret;
	CD3DX12_HEAP_PROPERTIES heapProp(heapType);

	// 確保する用途(Resource)に関する設定
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	auto result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	return ret;
}

