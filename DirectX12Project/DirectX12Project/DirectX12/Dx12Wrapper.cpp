#include "Dx12Wrapper.h"
#include <Windows.h>
#include <cassert>
#include <d3dcompiler.h>
#include <string>
#include <random>
#include <DirectXTex.h>
#include <stdint.h>
#include "../Application.h"
#include "../PMDLoder/PMDModel.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;
using namespace std;

namespace
{
	wstring GetWideStringfromString(const string& str)
	{
		wstring ret;
		// ���ڂ̓T�C�Y
		auto wstringSize = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			static_cast<int>(str.length()),
			nullptr,
			0);
		ret.resize(wstringSize);
		// ���ڂ͕�����
		MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			static_cast<int>(str.length()),
			&ret[0],
			static_cast<int>(ret.length()));
		return ret;
	}

	/// <summary>
	/// �g���q���o
	/// </summary>
	/// <param name="paths">���o���̃p�X</param>
	/// <returns>���o���ꂽ�g���q</returns>
	string GetExtension(const string& path)
	{
		int idx = path.rfind(".");
		if (idx == string::npos)
		{
			return "";
		}
		idx++;
		return path.substr(idx, path.length() - idx);
	}

	/// <summary>
	/// �e�N�X�`���̃p�X���Z�p���[�^�����ŕ�������
	/// </summary>
	/// <param name="paths">�p�X������</param>
	/// <param name="splitter">�Z�p���[�^����</param>
	/// <returns>������̕�����y�A</returns>
	vector<string> SplitFileName(const string& path, const char splitter = '*')
	{
		int idx = path.find(splitter);
		vector<string>ret;
		if (idx == string::npos)
		{
			ret.push_back(path);

			return ret;
		}

		ret.push_back(path.substr(0, idx));
		idx++;
		ret.push_back(path.substr(idx, path.length() - idx));
		return ret;
	}

	std::string GetTextureFromModelAndTexPath(const std::string& modelPath, const std::string& texPath)
	{
		auto idx1 = modelPath.rfind('/');
		if (idx1 == std::string::npos)
		{
			idx1 = 0;
		}
		auto idx2 = modelPath.rfind('\\');
		if (idx2 == std::string::npos)
		{
			idx2 = 0;
		}
		auto pathIndex = max(idx1, idx2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}
}

/// <summary>
/// ���_�\����
/// </summary>
struct Vertex
{
	XMFLOAT3 vertex;
	XMFLOAT2 uv;
	Vertex(XMFLOAT3 vert, XMFLOAT2 tuv) : vertex(vert), uv(tuv) {};
};
vector<PMDVertex> vertices_;
vector<unsigned short>indices_;

/// <summary>
/// align�̔{���ɂ����l��Ԃ�
/// </summary>
/// <param name="value">�l</param>
/// <param name="align">align�l</param>
/// <returns>align�̔{���ɂ����l</returns>
UINT AligndValue(UINT value, UINT align)
{
	//return (value + (align - 1)) &~ (align-1);
	return value +(align - (value % align)) % align;
};
void CreateVertices()
{
	//// ��O
	//vertices_.push_back(Vertex({ -100.0f, -100.0f,100 }, { 0.0f,1.0f })); //����
	//vertices_.push_back(Vertex({ -100.0f, 100.0f, 100}, { 0.0f,0.0f }));  //����
	//vertices_.push_back(Vertex({ 100.0f,-100.0f,100 }, { 1.0f,1.0f }));	  //�E��
	//vertices_.push_back(Vertex({ 100.0f, 100.0f,100 }, { 1.0f,0.0f }));	  //�E��

	//// ��
	//vertices_.push_back(Vertex({ -100.0f, -100.0f,-100 }, { 1.0f,1.0f }));	//����
	//vertices_.push_back(Vertex({ -100.0f, 100.0f, -100 }, { 1.0f,0.0f }));	//����
	//vertices_.push_back(Vertex({ 100.0f, -100.0f,-100 }, { 0.0f,1.0f }));	//�E��
	//vertices_.push_back(Vertex({ 100.0f, 100.0f,-100 }, { 0.0f,0.0f }));	//�E��


	//	// ��
	//vertices_.push_back(Vertex({ 100.0f, 100.0f,100 }, { 0.0f,1.0f }));	//����
	//vertices_.push_back(Vertex({ -100.0f, 100.0f, 100 }, { 0.0f,0.0f }));	//����
	//vertices_.push_back(Vertex({ 100.0f, 100.0f,-100 }, { 1.0f,1.0f }));	//�E��
	//vertices_.push_back(Vertex({ -100.0f, 100.0f,-100 }, { 1.0f,0.0f }));	//�E��
	//// ��
	//vertices_.push_back(Vertex({ -100.0f, -100.0f,100 }, { 0.0f,1.0f }));	//����
	//vertices_.push_back(Vertex({ 100.0f, -100.0f, 100 }, { 0.0f,0.0f }));	//����
	//vertices_.push_back(Vertex({ -100.0f, -100.0f,-100 }, { 1.0f,1.0f }));	//�E��
	//vertices_.push_back(Vertex({ 100.0f, -100.0f,-100 }, { 1.0f,0.0f }));	//�E��


}

void CreateIndices()
{
	//indices_ = { 0, 1, 2, 2, 1, 3 ,	// �O��
	//			 2, 3, 6, 6, 3, 7,	// �E��
	//			 6, 7, 4, 4, 7, 5,	// ����
	//			 4, 5, 0, 0, 5, 1,	// ����
	//			 8, 9, 10, 10, 9, 11,// ���
	//			 12, 13, 14, 14, 13, 15 // ����
	//			 };
}

void Dx12Wrapper::CreateVertexBuffer()
{
	// �m�ۂ���̈�(heap)�Ɋւ���ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���Ȃ��Ă��悢
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.VisibleNodeMask = 0;
	heapProp.CreationNodeMask = 0;
	// �����܂�
	// �m�ۂ���p�r(Resource)�Ɋւ���ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = vertices_.size() * sizeof(vertices_[0]);	// ���_���̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer_));
	assert(SUCCEEDED(result));

	//���_�f�[�^�]��
	PMDVertex* mappedData = nullptr;
	result = vertexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(vertices_.begin(), vertices_.end(), mappedData);
	vertexBuffer_->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices_[0]);
	vbView_.SizeInBytes = static_cast<UINT>(vertices_.size() * sizeof(vertices_[0]));
}

void Dx12Wrapper::CreateIndexBuffer()
{
	auto indices = pmdModel_->GetIndexData();
	// �m�ۂ���̈�(heap)�Ɋւ���ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	// �m�ۂ���p�r(Resource)�Ɋւ���ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = vertices_.size() * sizeof(vertices_[0]);	// �C���f�b�N�X�z��̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer_));
	assert(SUCCEEDED(result));

	//�C���f�b�N�X�f�[�^�]��
	auto forType = indices.back();
	decltype(forType)* mappedData = nullptr;
	result = indexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(indices.begin(), indices.end(), mappedData);
	indexBuffer_->Unmap(0, nullptr);

	// �C���f�b�N�X�r���[
	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(indices[0]));
}

bool Dx12Wrapper::CreateTexture(const std::wstring& path, P_Resouse_t& res)
{
	HRESULT result = S_OK;
	TexMetadata metaData= {};
	ScratchImage scratchImg = {};
	result = LoadFromWICFile(path.c_str(), WIC_FLAGS_IGNORE_SRGB, &metaData, scratchImg);
	if (FAILED(result))
	{
		return false;
	}
	assert(SUCCEEDED(result));
	auto img = scratchImg.GetImage(0, 0, 0);// ���f�[�^���o

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
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

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
	//// �e�N�X�`���f�[�^�쐬���{���͊O������ǂݍ���
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
		img->pixels,	// ���f�[�^�A�h���X
		static_cast<UINT>(img->rowPitch),	// 1���C���T�C�Y
		static_cast<UINT>(img->slicePitch)	// 1���T�C�Y
	);
	assert(SUCCEEDED(result));

	return true;
}

bool Dx12Wrapper::CreateMonoTexture()
{
	HRESULT result = S_OK;
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resDesc.Width = 4;
	resDesc.Height = 4;
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ���e�N�X�`��
	result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&whiteTex_));
	assert(SUCCEEDED(result));

	std::vector<uint8_t>data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);	// �S��0xff�ŏ�����
	// �f�[�^�]��
	result = whiteTex_->WriteToSubresource(
		0, 
		nullptr,
		data.data(), 
		4 * 4, 
		static_cast<UINT>(data.size()));

	// ���e�N�X�`��
	struct Color
	{
		uint8_t r, g, b, a;
		Color() :r(0), g(0), b(0), a(0) {};
		Color(uint8_t inr, uint8_t ing, uint8_t inb, uint8_t ina) :
			r(inr), g(ing), b(inb), a(inr) {}
	};
	result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&blackTex_));
	assert(SUCCEEDED(result));
	std::vector<Color>blackData(4 * 4);
	std::fill(blackData.begin(), blackData.end(), Color(0, 0, 0, 0xff));	// �S��0x00�ŏ�����
	// �f�[�^�]��
	result = blackTex_->WriteToSubresource(
		0,
		nullptr,
		blackData.data(),
		4 * 4,
		static_cast<UINT>(blackData.size()));

	return true;
}

bool Dx12Wrapper::CreateGradationTexture()
{
	HRESULT result = S_OK;
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resDesc.Width = 4;
	resDesc.Height = 256;
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// �O���f�[�V�����e�N�X�`��(�������A�オ��)
	result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&gradTex_));
	assert(SUCCEEDED(result));
	struct Color
	{
		uint8_t r, g, b, a;
		Color() :r(0), g(0), b(0), a(0) {};
		Color(uint8_t inr, uint8_t ing, uint8_t inb, uint8_t ina) :
			r(inr), g(ing), b(inb), a(inr) {}
	};
	std::vector<Color>gradData(4 * 256);
	for (size_t i = 0; i < 256; ++i)
	{
		fill_n(&gradData[i * 4], 4, Color(255 - i, 255 - i, 255 - i, 0xff));	// �S��0x00�ŏ�����
	}
	
	// �f�[�^�]��
	result = gradTex_->WriteToSubresource(
		0,
		nullptr,
		gradData.data(),
		4 * 4,
		4 * 4 * 256);
	return true;
}

bool Dx12Wrapper::CreateBasicDescriptors()
{
	//// SRV�p�f�B�X�N���v�^�q�[�v�쐬
	// ���W�ϊ��p�f�B�X�N���v�^�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	auto result = dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&resViewHeap_));
	assert(SUCCEEDED(result));

	auto cbDesc = transformBuffer_->GetDesc();
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = transformBuffer_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(cbDesc.Width);
	dev_->CreateConstantBufferView(&cbvDesc, resViewHeap_->GetCPUDescriptorHandleForHeapStart());
	return true;
}

bool Dx12Wrapper::CreateTransformBuffer()
{
	// �m�ۂ���̈�(heap)�Ɋւ���ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.VisibleNodeMask = 1;
	heapProp.CreationNodeMask = 1;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = AligndValue(sizeof(XMMATRIX), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);	// �萔�s��̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&transformBuffer_));
	assert(SUCCEEDED(result));

	auto wSize = Application::GetInstance().GetWindowSize();
	/*XMFLOAT4X4 tempMat = {};
	tempMat._11 = 1.0f / (static_cast<float>(wSize.width) / 2.0f);
	tempMat._22 = 1.0f / (static_cast<float>(wSize.height) / 2.0f);
	tempMat._33 = 1.0f;
	tempMat._44 = 1.0f;
	tempMat._41 = -1.0f;
	tempMat._42 = 1.0f;*/

	XMMATRIX world = XMMatrixIdentity();

	// 2D�\��
	//tmpMat.r[0].m128_f32[0] = 1.0f / (static_cast<float>(wSize.width) / 2.0f);
	//tmpMat.r[1].m128_f32[1] = -1.0f / (static_cast<float>(wSize.height) / 2.0f);
	//tmpMat.r[3].m128_f32[0] = -1.0f;
	//tmpMat.r[3].m128_f32[1] = 1.0f;
	// �����܂�2D�\��

	// 3D�\��
	// ���[���h�s��(���f�����g�ɑ΂���ϊ�)
	world *= XMMatrixRotationY(XM_PIDIV4);

	// �J�����s��(�r���[�s��)
	XMMATRIX viewproj = XMMatrixLookAtRH(
		{ 0.0f, 10.0f, 30.0f, 1.0f },	// ���_
		{ 0.0f, 10.0f, 0.0f, 1.0f },		// �����X
		{ 0.0f, 1.0f, 0.0f,1.0f });		// ��(���̏�)

	// �v���W�F�N�V�����s��(�p�[�X�y�N�e�B�u�s��or�ˉe�s��)
	viewproj *= XMMatrixPerspectiveFovRH(XM_PIDIV4 , // ��p(FOV)
		static_cast<float>(wSize.width) / static_cast<float>(wSize.height), 
		0.1f,	// �j�A(�߂�)
		300.0f);	//�@�t�@�[(����)

	// ��ł����邽�߂ɊJ���������ɂ��Ă���
	transformBuffer_->Map(0, nullptr, (void**)&mappedBasicMatrix_);
	
	mappedBasicMatrix_->viewproj = viewproj;
	mappedBasicMatrix_->world = world;
	return true;
}

bool Dx12Wrapper::CreateDepthBufferView()
{
	D3D12_HEAP_PROPERTIES heapProp = { };
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	auto rtvDesc = bbResouces[0]->GetDesc();

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_D32_FLOAT;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.DepthOrArraySize = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resDesc.MipLevels = 1;
	resDesc.Width = rtvDesc.Width;
	resDesc.Height = rtvDesc.Height;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	auto result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&depthBuffer_));
	assert(SUCCEEDED(result));


	D3D12_DESCRIPTOR_HEAP_DESC desDesc = {};
	desDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desDesc.NumDescriptors = 1;
	desDesc.NodeMask = 0;
	desDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = dev_->CreateDescriptorHeap(&desDesc,
		IID_PPV_ARGS(&depthDescHeap_));
	assert(SUCCEEDED(result));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	dev_->CreateDepthStencilView(depthBuffer_,
		&dsvDesc,
		depthDescHeap_->GetCPUDescriptorHandleForHeapStart());
	return true;
}

bool Dx12Wrapper::CreateMaterialBufferView()
{
	HRESULT result = S_OK;
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

	auto& mats = pmdModel_->GetMaterialData();
	auto strideBytes = AligndValue(sizeof(BasicMaterial), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Width = mats.size() * strideBytes + texBuffers_.size();
	resDesc.Height = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.MipLevels = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.DepthOrArraySize = 1;
	
	result = dev_->CreateCommittedResource(&heapProp, 
		D3D12_HEAP_FLAG_NONE, 
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&materialBuffer_));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NodeMask = 0;
	// �}�e���A��CBV + �ʏ�SRV + ��Z�X�t�B�A�}�b�vSRV 
	// +  ���Z�X�t�B�A�}�b�vSRV + toon�}�b�v 
	heapDesc.NumDescriptors = static_cast<UINT>(mats.size() * 5); 
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = dev_->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&materialDescHeap_));
	assert(SUCCEEDED(result));
	auto gAddress = materialBuffer_->GetGPUVirtualAddress();
	auto heapSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto heapAddress = materialDescHeap_->GetCPUDescriptorHandleForHeapStart();

	// �}�e���A���萔�o�b�t�@�p�f�B�X�N
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.SizeInBytes = strideBytes;
	cbvDesc.BufferLocation = gAddress;

	// �e�N�X�`���p�f�B�X�N
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);

	uint8_t* mappedMaterial = nullptr;
	result = materialBuffer_->Map(0, nullptr, (void**)&mappedMaterial);
		assert(SUCCEEDED(result));
	for (int i = 0; i < mats.size(); ++i)
	{		
		// �}�e���A���萔�o�b�t�@�r���[
		((BasicMaterial*)mappedMaterial)->diffuse = mats[i].diffuse;
		((BasicMaterial*)mappedMaterial)->ambient = mats[i].ambient;
		((BasicMaterial*)mappedMaterial)->speqular = mats[i].speqular;
		((BasicMaterial*)mappedMaterial)->alpha = mats[i].alpha;
		((BasicMaterial*)mappedMaterial)->speqularity = mats[i].speqularity;

		cbvDesc.BufferLocation = gAddress;
		dev_->CreateConstantBufferView(
			&cbvDesc,
			heapAddress
		);
		mappedMaterial += strideBytes;
		gAddress += strideBytes;
		heapAddress.ptr += heapSize;

		// �e�N�X�`���r���[
		// �t�H�[�}�b�g�������Y��Ȃ��悤��
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (texBuffers_[i] != nullptr)
		{
			srvDesc.Format = texBuffers_[i]->GetDesc().Format;
		}
		dev_->CreateShaderResourceView(
			texBuffers_[i] != nullptr? texBuffers_[i] : whiteTex_,
			&srvDesc,
			heapAddress);
		heapAddress.ptr += heapSize;

		// ��Z�X�t�B�A�}�b�v
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (sphBuffers_[i] != nullptr)
		{
			srvDesc.Format = sphBuffers_[i]->GetDesc().Format;
		}		
		dev_->CreateShaderResourceView(
			sphBuffers_[i] != nullptr ? sphBuffers_[i] : whiteTex_,
			&srvDesc,
			heapAddress);
		heapAddress.ptr += heapSize;
		
		// ���Z�X�t�B�A�}�b�v
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (spaBuffers_[i] != nullptr)
		{
			srvDesc.Format = spaBuffers_[i]->GetDesc().Format;
		}		
		dev_->CreateShaderResourceView(
			spaBuffers_[i] != nullptr ? spaBuffers_[i] : blackTex_,
			&srvDesc,
			heapAddress);
		heapAddress.ptr += heapSize;

		// ���Z�X�t�B�A�}�b�v
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (toonBuffers_[i] != nullptr)
		{
			srvDesc.Format = toonBuffers_[i]->GetDesc().Format;
		}
		dev_->CreateShaderResourceView(
			toonBuffers_[i] != nullptr ? toonBuffers_[i] : gradTex_,
			&srvDesc,
			heapAddress);
		heapAddress.ptr += heapSize;
	}
	materialBuffer_->Unmap(0, nullptr);
	return true;
}

bool Dx12Wrapper::CreatePipelineState()
{
	HRESULT result = S_OK;
	// �p�C�v���C���X�e�[�g�f�X�N
	D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	// IA(InputAssembler)
	// ���̓��C�A�E�g
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		// ���_���
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// ���_���
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// UV���
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���_�V�F�[�_
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/vs.hlsl",
		nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_1",
		0,
		0, &vsBlob, &errBlob);
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));	
	plsDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	plsDesc.VS.BytecodeLength = vsBlob->GetBufferSize();

	// ���X�^���C�U�ݒ�
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	plsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	plsDesc.RasterizerState.DepthClipEnable = false;
	plsDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
	plsDesc.RasterizerState.MultisampleEnable = false;
	

	// �s�N�Z���V�F�[�_
	ID3DBlob* psBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/ps.hlsl", 
		nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_1",
		0,
		0, &psBlob, &errBlob);
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));

	plsDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	plsDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// ���̑��ݒ�
	// �f�v�X�ƃX�e���V���ݒ�
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState.DepthEnable = true;
	plsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	plsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	
	plsDesc.DepthStencilState.StencilEnable = false;

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// �o�͐ݒ�
	plsDesc.NumRenderTargets = 1;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �u�����h
	plsDesc.BlendState.AlphaToCoverageEnable = false;
	plsDesc.BlendState.IndependentBlendEnable = false;
	plsDesc.BlendState.RenderTarget[0].BlendEnable = false;
	plsDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	
	// ���[�g�V�O�l�`������
	CreateRootSignature(plsDesc);

	vsBlob->Release();
	psBlob->Release();
	return true;
}

void Dx12Wrapper::CreateRootSignature(D3D12_GRAPHICS_PIPELINE_STATE_DESC& plsDesc)
{
	HRESULT result = S_OK;
	// ���[�g�V�O�l�`��
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsDesc.NumParameters = 1;

	D3D12_ROOT_PARAMETER rp[2] = {};
	D3D12_DESCRIPTOR_RANGE range[3] = {};
	// �s��萔�o�b�t�@
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//b
	range[0].BaseShaderRegister = 0;	// 0 b0��\��
	range[0].NumDescriptors = 1;		//b0�`b0�܂�
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// �}�e���A��
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//b
	range[1].BaseShaderRegister = 1;	// 0 b1��\��
	range[1].NumDescriptors = 1;		//b1�`b1�܂�
	range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// �e�N�X�`��
	range[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[2].BaseShaderRegister = 0;	// 0 t0��\��
	range[2].NumDescriptors = 4;		//t0�`t3�܂�
	range[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���W�ϊ�
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];
	rp[0].DescriptorTable.NumDescriptorRanges = 1;
	// �}�e���A��&�e�N�X�`��
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];
	rp[1].DescriptorTable.NumDescriptorRanges = 2;

	rsDesc.pParameters = rp;
	rsDesc.NumParameters = _countof(rp);

	// s0���`
	// �T���v���̒�`�A�T���v����uv��0������1�z���Ƃ��̎���
	// UV�����ƂɐF���Ƃ��Ă���Ƃ��̃��[�����w�肷�����
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc[0].ShaderRegister = 0;
	samplerDesc[0].RegisterSpace = 0;
	// WRAP�͌J��Ԃ���\��
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].MaxAnisotropy = 0;
	samplerDesc[0].MaxLOD = 0.0f;
	samplerDesc[0].MinLOD = 0.0f;
	samplerDesc[0].MipLODBias = 0.0f;
	samplerDesc[1] = samplerDesc[0];	// �܂��R�s�[
	samplerDesc[1].ShaderRegister = 1;
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;


	rsDesc.pStaticSamplers = samplerDesc;
	rsDesc.NumStaticSamplers = _countof(samplerDesc);

	// �V�O�l�`���ݒ�
	ID3DBlob* errBlob = nullptr;
	ID3DBlob* sigBlob = nullptr;
	D3D12SerializeRootSignature(&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sigBlob,
		&errBlob);
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));

	// ���[�g�V�O�l�`���̐���
	result = dev_->CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSig_));
	assert(SUCCEEDED(result));
	plsDesc.pRootSignature = rootSig_;

	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

	sigBlob->Release();
}

void Dx12Wrapper::OutputFromErrorBlob(ID3DBlob* errBlob)
{
	if (errBlob != nullptr)
	{
		string errStr = "";
		auto errSize = errBlob->GetBufferSize();
		errStr.resize(errSize);
		copy_n((char*)errBlob->GetBufferPointer(), errSize, errStr.begin());
		OutputDebugStringA(errStr.c_str());
		errBlob->Release();
	}
}

Dx12Wrapper::Dx12Wrapper()
{
	
}

Dx12Wrapper::~Dx12Wrapper()
{
}

bool Dx12Wrapper::Init(HWND hwnd)
{
	HRESULT result = S_OK;
#if _DEBUG
	ID3D12Debug3* debug = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	debug->EnableDebugLayer();
	debug->Release();
#endif
	CheckFeatureLevel();
#if _DEBUG
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgi_));
#else 
	result = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgi_));
#endif
	assert(SUCCEEDED(result));

	InitCommandSet();
	
	pmdModel_ = make_shared<PMDModel>();
	const char* modelPath = "Resource/PMD/��ߔe��v1.0/��ߔe��v1.pmd";
	pmdModel_->Load(modelPath);
	vertices_ = pmdModel_->GetVertexData();
	CreateSwapChain(hwnd);
	
	CreateFence();

	// �����_�[�^�[�Q�b�g���쐬
	CreateRenderTargetDescriptorHeap();
	// �[�x�o�b�t�@�r���[�쐬
	CreateDepthBufferView();

	// ���_�o�b�t�@���쐬
	CreateVertices();
	CreateVertexBuffer();

	// �C���f�b�N�X�o�b�t�@���쐬
	CreateIndices();
	CreateIndexBuffer();

	// �萔�o�b�t�@�쐬
	CreateTransformBuffer();

	// �e�N�X�`���쐬
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
			string strToonPath = "Resource/PMD/toon/" + toonPaths[i];
			CreateTexture(
				GetWideStringfromString(strToonPath),
				toonBuffers_[i]);
		}
		if (paths[i] == "")
		{
			continue;
		}
		
		auto pathVec = SplitFileName(paths[i]);
		for (auto path : pathVec)
		{
			auto ext = GetExtension(path);
			if (ext == "spa")
			{
				auto str = GetTextureFromModelAndTexPath(modelPath, path);
				CreateTexture(
					GetWideStringfromString(str),
					spaBuffers_[i]);
				continue;
			}
			if (ext == "sph")
			{
				auto str = GetTextureFromModelAndTexPath(modelPath, path);
				CreateTexture(
					GetWideStringfromString(str),
					sphBuffers_[i]);
				continue;
			}
			auto str = GetTextureFromModelAndTexPath(modelPath, path);
			CreateTexture(
				GetWideStringfromString(str),
				texBuffers_[i]);
			
		}
	}
	// �����e�N�X�`���쐬
	CreateMonoTexture();

	CreateGradationTexture();
	//CreateMonoTexture(blackTex_, true);

	// �}�e���A���o�b�t�@�̍쐬
	CreateMaterialBufferView();
	
	// ���W�ϊ�SRV�p�f�B�X�N���v�^�q�[�v�쐬
	CreateBasicDescriptors();

	if (!CreatePipelineState())
	{
		return false;
	}
	InitViewRect();

	return true;
}

bool Dx12Wrapper::InitViewRect()
{
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	// �r���[�|�[�g
	viewPort_.TopLeftX = 0;
	viewPort_.TopLeftY = 0;
	viewPort_.Width = static_cast<FLOAT>(wSize.width);
	viewPort_.Height = static_cast<FLOAT>(wSize.height);
	viewPort_.MaxDepth = 1.0f;
	viewPort_.MinDepth = 0.0f;

	// �V�U�[��`�̐ݒ�
	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = static_cast<LONG>(wSize.width);
	scissorRect_.bottom = static_cast<LONG>(wSize.height);

	return true;
}

bool Dx12Wrapper::Update()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_, pipelineState_);
	// �����ɖ���(�R�}���h���X�g��ς�ł���)	
	

	static float modelY = 0.0f;
	static float angle = 0.0f;
	BYTE keyState[256];
	auto result = GetKeyboardState(keyState);
	if (keyState[VK_UP] & 0x80)
	{
		modelY += 0.1f;
	}
	if( keyState[VK_DOWN] & 0x80)
	{
		modelY -= 0.1f;
	}
	angle += 0.02f;
	mappedBasicMatrix_->world = XMMatrixRotationY(angle);
	//mappedBasicMatrix_->world *= XMMatrixRotationX(angle);
	mappedBasicMatrix_->world *= XMMatrixTranslation(0, modelY, 0);

	// ���\�[�X�o���A��ݒ�v���[���g���烌���_�[�^�[�Q�b�g
	auto bbIdx = swapchain_->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = bbResouces[bbIdx];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList_->ResourceBarrier(1, &barrier);

	// �����_�[�^�[�Q�b�g���Z�b�g	
	auto rtvHeap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto rtvIncSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHeap.ptr += static_cast<SIZE_T>(bbIdx) * rtvIncSize;
	cmdList_->OMSetRenderTargets(1, &rtvHeap, false, &depthDescHeap_->GetCPUDescriptorHandleForHeapStart());
	// ��ʂ��N���A(�F�ς���)
	float clsCol[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	cmdList_->ClearRenderTargetView(rtvHeap, clsCol, 0, nullptr);
	cmdList_->ClearDepthStencilView(
		depthDescHeap_->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);

	cmdList_->SetGraphicsRootSignature(rootSig_);
	// �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	cmdList_->RSSetViewports(1, &viewPort_);
	cmdList_->RSSetScissorRects(1, &scissorRect_);

	ID3D12DescriptorHeap* deskHeaps[] = {resViewHeap_};
	auto heapPos = resViewHeap_->GetGPUDescriptorHandleForHeapStart();
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, heapPos);
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	//cmdList_->DrawInstanced(vertices_.size(), 1, 0, 0);
	cmdList_->IASetIndexBuffer(&ibView_);
	auto material = pmdModel_->GetMaterialData();
	uint32_t indexOffset = 0;

	ID3D12DescriptorHeap* matDeskHeaps[] = { materialDescHeap_ };
	cmdList_->SetDescriptorHeaps(1, matDeskHeaps);
	auto materialHeapPos = materialDescHeap_->GetGPUDescriptorHandleForHeapStart();
	const auto heapSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (auto& m : material)
	{	
		cmdList_->SetGraphicsRootDescriptorTable(1, materialHeapPos);
		auto indexNum = m.indexNum;

		cmdList_->DrawIndexedInstanced(
			indexNum,		// �C���f�b�N�X��
			1,				// �C���X�^���X��
			indexOffset,	// �C���f�b�N�X�I�t�Z�b�g
			0,				// ���_�I�t�Z�b�g
			0);				// �C���X�^���X�I�t�Z�b�g
		indexOffset += indexNum;
		materialHeapPos.ptr += static_cast<UINT64>(heapSize) * 5;
	}
	

	// ���\�[�X�o���A��ݒ背���_�[�^�[�Q�b�g����v���[���g
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList_->ResourceBarrier(1, &barrier);

	cmdList_->Close();

	ID3D12CommandList* cmdLists[] = { cmdList_ };
	cmdQue_->ExecuteCommandLists(1, cmdLists);
	cmdQue_->Signal(fence_,++fenceValue_);
	// Execute�����܂ő҂���
	while (true)
	{
		if (fence_->GetCompletedValue() == fenceValue_)
		{
			break;
		}
	}
	swapchain_->Present(1, 0);
	return true;
}

void Dx12Wrapper::Terminate()
{
	dev_->Release();
}

bool Dx12Wrapper::CheckFeatureLevel()
{
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_1_0_CORE;

	for (auto lv : levels)
	{
		auto result = D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&dev_));
		if (SUCCEEDED(result))
		{
			level = lv;
			break;
		}
	}
	if (level == D3D_FEATURE_LEVEL_1_0_CORE)
	{
		OutputDebugString(L"feature level not found");
		return false;
	}
	return true;
}

bool Dx12Wrapper::InitCommandSet()
{
	// �R�}���h�L���[�쐬
	D3D12_COMMAND_QUEUE_DESC cmdQDesc = {};
	cmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQDesc.NodeMask = 0;
	cmdQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	auto result = dev_->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(&cmdQue_));
	assert(SUCCEEDED(result));

	// �R�}���h�A���P�[�^�[�̍쐬
	dev_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator_));
	assert(SUCCEEDED(result));

	// �R�}���h���X�g�̍쐬
	dev_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_, nullptr,
		IID_PPV_ARGS(&cmdList_));
	assert(SUCCEEDED(result));
	cmdList_->Close();
	return true;
}

bool Dx12Wrapper::CreateSwapChain(const HWND hwnd)
{
	// swapchainDesc�쐬
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = static_cast<UINT>(wSize.width);
	scDesc.Height = static_cast<UINT>(wSize.height);
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferCount = 2;	// �\�Ɨ����
	// ���o�b�N�o�b�t�@�ł�������
	// �@�Ⴂ������Ȃ����ߗv����
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.Flags = 0/*DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH*/;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Stereo = false;	// VR�̎�true
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// swapchain�쐬
	auto result = dxgi_->CreateSwapChainForHwnd(cmdQue_,
		hwnd,
		&scDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&swapchain_);
	assert(SUCCEEDED(result));
	return true;
}

bool Dx12Wrapper::CreateFence()
{
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	// �t�F���X�����(�X���b�h�Z�[�t�ɕK�v)
	dev_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	fenceValue_ = fence_->GetCompletedValue();
	return true;
}

bool Dx12Wrapper::CreateRenderTargetDescriptorHeap()
{
	HRESULT result = S_OK;
	// �\����ʗp�������m��
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	desc.NodeMask = 0;
	desc.NumDescriptors = 2;// �\�Ɨ���ʗp
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap_));
	assert(SUCCEEDED(result));

	// �����_�[�^�[�Q�b�g��ݒ�
	DXGI_SWAP_CHAIN_DESC1 swDesc;
	result = swapchain_->GetDesc1(&swDesc);
	assert(SUCCEEDED(result));
	int num_rtvs = swDesc.BufferCount;	
	bbResouces.resize(num_rtvs);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = swDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	auto heap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto incSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (int i = 0; i < num_rtvs; i++)
	{
		swapchain_->GetBuffer(i, IID_PPV_ARGS(&bbResouces[i]));//�u�L�����o�X�v���擾
		dev_->CreateRenderTargetView(bbResouces[i], &rtvDesc, heap);	// �L�����p�X�ƐE�l��R�Â���
		heap.ptr += incSize;// �E�l�ƃL�����o�X�̃y�A�̂Ԃ񎟂̂Ƃ���܂ŃI�t�Z�b�g
	}

	return SUCCEEDED(result);
}
