#include "PMDActor.h"
#include "../PMDLoder/PMDLoder.h"
#include <Windows.h>
#include <DirectXTex.h>
#include "../Common/StrOperater.h"
using namespace DirectX;
using namespace std;

PMDActor::PMDActor(ID3D12Device* dev) : dev_(dev)
{
}

void PMDActor::LoadModel(const char* path)
{
	pmdModel_ = make_shared<PMDLoder>();
	
	pmdModel_->Load(path);
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

	//BmpLoder bmp("Resource/image/sample.bmp");
	//auto bSize = bmp.GetBmpSize();
	//auto& rawData = bmp.GetRawData();
	//// テクスチャデータ作成※本来は外部から読み込む
	//vector<uint8_t>texData(4 * bSize.width * bSize.height);
	//int texIdx = 0;
	//for (int y = bSize.height - 1; y > 0; --y)
	//{
	//	for (int x = 0; x < bSize.width; ++x)
	//	{
	//		texData[texIdx++] = rawData[(x + y * bSize.width) * 3 + 0];
	//		texData[texIdx++] = rawData[(x + y * bSize.width) * 3 + 1];
	//		texData[texIdx++] = rawData[(x + y * bSize.width) * 3 + 2];
	//		texData[texIdx++] = 0xff;
	//	}
	//}
	//D3D12_BOX box;
	//box.left = 0;
	//box.right = metaData.width;
	//box.top = 0;
	//box.bottom = metaData.height;
	//box.front = 0;
	//box.back = 1;
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
	texBuffers_.resize(paths.size());
	sphBuffers_.resize(paths.size());
	spaBuffers_.resize(paths.size());
	toonBuffers_.resize(toonPaths.size());

	for (int i = 0; i < paths.size(); ++i)
	{
		if (toonPaths[i] != "")
		{
			string strToonPath = StrOperater::GetTextureFromModelAndTexPath(modelPath, toonPaths[i]);

			if (!CreateTexture(
				StrOperater::GetWideStringfromString(strToonPath),
				toonBuffers_[i]))
			{
				strToonPath = "Resource/PMD/toon/" + toonPaths[i];
				CreateTexture(
					StrOperater::GetWideStringfromString(strToonPath),
					toonBuffers_[i]);
			}
		}
		if (paths[i] == "")
		{
			continue;
		}

		auto pathVec = StrOperater::SplitFileName(paths[i]);
		for (auto path : pathVec)
		{
			auto ext = StrOperater::GetExtension(path);
			if (ext == "spa")
			{
				auto str = StrOperater::GetTextureFromModelAndTexPath(modelPath, path);
				CreateTexture(
					StrOperater::GetWideStringfromString(str),
					spaBuffers_[i]);
				continue;
			}
			if (ext == "sph")
			{
				auto str = StrOperater::GetTextureFromModelAndTexPath(modelPath, path);
				CreateTexture(
					StrOperater::GetWideStringfromString(str),
					sphBuffers_[i]);
				continue;
			}
			auto str = StrOperater::GetTextureFromModelAndTexPath(modelPath, path);
			auto result = CreateTexture(
				StrOperater::GetWideStringfromString(str),
				texBuffers_[i]);
			assert(result);

		}
	}
}

PMDLoder& PMDActor::GetPMDModel()
{
	return *pmdModel_;
}
