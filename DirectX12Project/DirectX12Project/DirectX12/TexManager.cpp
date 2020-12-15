#include "TexManager.h"
#include <DirectXTex.h>
#include "../Common/StrOperater.h"

using namespace DirectX;
using namespace std;


ComPtr<ID3D12Resource> TexManager::GetDefTex(const ColTexType& colTexType)
{
	return defTextures_[static_cast<int>(colTexType)];
}

TexManager::TexManager()
{

}

bool TexManager::CreateTexture(const std::wstring& path, ComPtr<ID3D12Resource>& res)
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

void TexManager::SetDevice(ID3D12Device* dev)
{
	dev_ = dev;
}


bool TexManager::CreateMonoColorTexture(ID3D12GraphicsCommandList* cmdList, ColTexType colType, const Color col)
{
	HRESULT result = S_OK;
	Size size = { 4, 4 };
	std::vector<Color>texData(size.width * size.height);

	std::fill(texData.begin(), texData.end(), col);	// 全部0xffで初期化

	D3D12_SUBRESOURCE_DATA subResData = {};
	subResData.pData = texData.data();
	subResData.RowPitch = sizeof(texData[0]) * size.width;
	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
	SetUploadTexure(cmdList, subResData, colType);

	return true;
}

bool TexManager::CreateGradationTexture(ID3D12GraphicsCommandList* cmdList, const Size size)
{
	std::vector<Color>texData(size.width * size.height);
	for (size_t i = 0; i < 256; ++i)
	{
		fill_n(&texData[i * 4], 4, Color(static_cast<uint8_t>(255 - i)));	// rgb全部0x00で初期化
	}
	D3D12_SUBRESOURCE_DATA subResData = {};
	subResData.pData = texData.data();
	subResData.RowPitch = sizeof(texData[0]) * size.width;
	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
	SetUploadTexure(cmdList, subResData, ColTexType::Grad);
	return true;
}

void TexManager::SetUploadTexure(ID3D12GraphicsCommandList* cmdList, D3D12_SUBRESOURCE_DATA& subResData, ColTexType colType)
{
	defTextures_.resize(static_cast<int>(ColTexType::Max));
	auto& texture = defTextures_[static_cast<int>(colType)];
	// 転送先
	auto width = subResData.RowPitch / sizeof(Color);
	auto height = subResData.SlicePitch / subResData.RowPitch;

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
			width,
			static_cast<UINT>(height));
	auto result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(texture.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// 転送元
	ComPtr<ID3D12Resource>intermediateBuff;	// 中間バッファ
	auto buffSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);
	result = dev_->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(intermediateBuff.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	intermediateBuffList_.push_back(intermediateBuff);

	// コマンドリストに登録
	// 中でCopyTextureRegionが走っているため
	// コマンドキュー待ちが必要
	UpdateSubresources(cmdList, texture.Get(),
		intermediateBuff.Get(), 0, 0, 1, &subResData);
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}
