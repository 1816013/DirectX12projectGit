#include "PMDActor.h"
#include "../PMDLoder/PMDLoder.h"
#include <Windows.h>
#include <DirectXTex.h>
#include "../Common/StrOperater.h"
#include "PMDResource.h"
#include "../Common.h"
#include "TexManager.h"
using namespace DirectX;
using namespace std;

namespace
{
	string texTable[] = { "bmp", "spa", "sph", "toon" };
}

PMDActor::PMDActor(ComPtr<ID3D12Device>&dev, const char* path) : dev_(dev)
{
	texManager_ = make_shared<TexManager>(dev_.Get());
	pmdModel_ = make_shared<PMDLoder>();
	pmdModel_->Load(path);
	pmdResource_ = make_shared<PMDResource>(dev_.Get());
	CreateVertexBufferView();
	CreateIndexBufferView();
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
					
			if (!texManager_->CreateTexture(
				StrOperater::GetWideStringfromString(strToonPath),
				toonBuffers[i]))
			{
				strToonPath = "Resource/PMD/toon/" + toonPaths[i];
				auto result = texManager_->CreateTexture(
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
			auto result = texManager_->CreateTexture(
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

PMDResource& PMDActor::GetPMDResource()
{
	return *pmdResource_;
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

void PMDActor::DrawModel(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	cmdList->SetGraphicsRootSignature(pmdResource_->GetRootSignature().Get());
	cmdList->SetPipelineState(pmdResource_->GetPipelineState().Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView_);
	cmdList->IASetIndexBuffer(&ibView_);

	// 座標変換ヒープセット
	auto resHeap = pmdResource_->GetGroops(GroopType::TRANSFORM).descHeap_.Get();
	ID3D12DescriptorHeap* deskHeaps[] = { resHeap/*resViewHeap_.Get()*/ };
	auto heapPos = /*resViewHeap_*/resHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetDescriptorHeaps(1, deskHeaps);
	cmdList->SetGraphicsRootDescriptorTable(0, heapPos);

	// マテリアル&テクスチャヒープセット
	auto material = GetPMDModel().GetMaterialData();
	uint32_t indexOffset = 0;
	auto matHeap = pmdResource_->GetGroops(GroopType::MATERIAL).descHeap_.Get();
	ID3D12DescriptorHeap* matDeskHeaps[] = { matHeap };
	cmdList->SetDescriptorHeaps(1, matDeskHeaps);
	auto materialHeapPos = matHeap->GetGPUDescriptorHandleForHeapStart();
	const auto heapSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto descNum = pmdResource_->GetGroops(GroopType::MATERIAL).types_.size();
	for (auto& m : material)
	{
		cmdList->SetGraphicsRootDescriptorTable(1, materialHeapPos);
		auto indexNum = m.indexNum;

		cmdList->DrawIndexedInstanced(
			indexNum,		// インデックス数
			1,				// インスタンス数
			indexOffset,	// インデックスオフセット
			0,				// 頂点オフセット
			0);				// インスタンスオフセット
		indexOffset += indexNum;
		materialHeapPos.ptr += static_cast<UINT64>(heapSize) * descNum;
	}
}

void PMDActor::CreateVertexBufferView()
{
	const auto& vertices = pmdModel_->GetVertexData();
	vertexBuffer = CreateBuffer(vertices.size() * sizeof(vertices[0]));
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
	indexBuffer = CreateBuffer(indices.size() * sizeof(indices[0]));
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

bool PMDActor::CreateMaterialBufferView()
{
	// マテリアルバッファの作成
	HRESULT result = S_OK;
	auto& mats = pmdModel_->GetMaterialData();
	auto strideBytes = Common::AligndValue(sizeof(BasicMaterial), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	materialBuffer_ = CreateBuffer(mats.size() * strideBytes);
	auto gAddress = materialBuffer_->GetGPUVirtualAddress();
	uint8_t* mappedMaterial = nullptr;
	result = materialBuffer_->Map(0, nullptr, (void**)&mappedMaterial);
	assert(SUCCEEDED(result));

	auto& transResBind = pmdResource_->GetGroops(GroopType::MATERIAL);
	transResBind.Init({ BuffType::CBV, BuffType::SRV, BuffType::SRV, BuffType::SRV, BuffType::SRV });
	array<pair<string, ID3D12Resource*>, 4>texPairList;
	/*texPairList = { make_pair("bmp",defTextures_[static_cast<int>(ColTexType::White)].Get()),
					make_pair("sph",defTextures_[static_cast<int>(ColTexType::White)].Get()),
					make_pair("spa",defTextures_[static_cast<int>(ColTexType::Black)].Get()),
					make_pair("toon",defTextures_[static_cast<int>(ColTexType::Grad)].Get()) };*/
	texPairList = { make_pair("bmp",nullptr),
					make_pair("sph",nullptr),
					make_pair("spa",nullptr),
					make_pair("toon",nullptr) };
	for (int i = 0; i < mats.size(); ++i)
	{
		// マテリアル定数バッファビュー
		((BasicMaterial*)mappedMaterial)->diffuse = mats[i].diffuse;
		((BasicMaterial*)mappedMaterial)->ambient = mats[i].ambient;
		((BasicMaterial*)mappedMaterial)->speqular = mats[i].speqular;
		((BasicMaterial*)mappedMaterial)->alpha = mats[i].alpha;
		((BasicMaterial*)mappedMaterial)->speqularity = mats[i].speqularity;

		transResBind.AddBuffers(materialBuffer_.Get(), strideBytes);
		mappedMaterial += strideBytes;
		for (auto texpair : texPairList)
		{
			ID3D12Resource* res = textures_[texpair.first][i].Get();
			if (res == nullptr)
			{
				res = texpair.second;
			}
			transResBind.AddBuffers(res);
		}
	}
	materialBuffer_->Unmap(0, nullptr);
	return true;
}
