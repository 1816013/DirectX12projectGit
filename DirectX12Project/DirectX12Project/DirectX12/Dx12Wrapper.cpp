#include "Dx12Wrapper.h"
#include <Windows.h>
#include <cassert>
#include <d3dcompiler.h>
#include <string>
#include <random>
#include <DirectXTex.h>
#include "../Application.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;
using namespace std;

/// <summary>
/// ���_�\����
/// </summary>
struct Vertex
{
	XMFLOAT3 vertex;
	XMFLOAT2 uv;
	Vertex(XMFLOAT3 vert, XMFLOAT2 tuv) : vertex(vert), uv(tuv) {};
};
vector<Vertex> vertices_;
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
	// ��O
	vertices_.push_back(Vertex({ -100.0f, -100.0f,100 }, { 0.0f,1.0f })); //����
	vertices_.push_back(Vertex({ -100.0f, 100.0f, 100}, { 0.0f,0.0f }));  //����
	vertices_.push_back(Vertex({ 100.0f,-100.0f,100 }, { 1.0f,1.0f }));	  //�E��
	vertices_.push_back(Vertex({ 100.0f, 100.0f,100 }, { 1.0f,0.0f }));	  //�E��

	// ��
	vertices_.push_back(Vertex({ -100.0f, -100.0f,-100 }, { 1.0f,1.0f }));	//����
	vertices_.push_back(Vertex({ -100.0f, 100.0f, -100 }, { 1.0f,0.0f }));	//����
	vertices_.push_back(Vertex({ 100.0f, -100.0f,-100 }, { 0.0f,1.0f }));	//�E��
	vertices_.push_back(Vertex({ 100.0f, 100.0f,-100 }, { 0.0f,0.0f }));	//�E��


		// ��
	vertices_.push_back(Vertex({ 100.0f, 100.0f,100 }, { 0.0f,1.0f }));	//����
	vertices_.push_back(Vertex({ -100.0f, 100.0f, 100 }, { 0.0f,0.0f }));	//����
	vertices_.push_back(Vertex({ 100.0f, 100.0f,-100 }, { 1.0f,1.0f }));	//�E��
	vertices_.push_back(Vertex({ -100.0f, 100.0f,-100 }, { 1.0f,0.0f }));	//�E��
	// ��
	vertices_.push_back(Vertex({ -100.0f, -100.0f,100 }, { 0.0f,1.0f }));	//����
	vertices_.push_back(Vertex({ 100.0f, -100.0f, 100 }, { 0.0f,0.0f }));	//����
	vertices_.push_back(Vertex({ -100.0f, -100.0f,-100 }, { 1.0f,1.0f }));	//�E��
	vertices_.push_back(Vertex({ 100.0f, -100.0f,-100 }, { 1.0f,0.0f }));	//�E��


}

void CreateIndices()
{
	indices_ = { 0, 1, 2, 2, 1, 3 ,	// �O��
				 2, 3, 6, 6, 3, 7,	// �E��
				 6, 7, 4, 4, 7, 5,	// ����
				 4, 5, 0, 0, 5, 1,	// ����
				 8, 9, 10, 10, 9, 11,// ���
				 12, 13, 14, 14, 13, 15 // ����
				 };
	/*indices_ = { 0, 1, 5, 5, 1, 6 };
	indices_ = { 0, 1, 2, 2, 1, 3 };
	indices_ = { 0, 1, 2, 2, 1, 3 };*/
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
	Vertex* mappedData = nullptr;
	result = vertexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(vertices_.begin(), vertices_.end(), mappedData);
	vertexBuffer_->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices_[0]);
	vbView_.SizeInBytes = vertices_.size() * sizeof(vertices_[0]);
}

void Dx12Wrapper::CreateIndexBuffer()
{
	// �m�ۂ���̈�(heap)�Ɋւ���ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	// �m�ۂ���p�r(Resource)�Ɋւ���ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = indices_.size() * sizeof(indices_[0]);	// �C���f�b�N�X�z��̃T�C�Y
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
	unsigned short* mappedData = nullptr;
	result = indexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(indices_.begin(), indices_.end(), mappedData);
	indexBuffer_->Unmap(0, nullptr);

	// �C���f�b�N�X�r���[
	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = indices_.size() * sizeof(indices_[0]);
}

bool Dx12Wrapper::CreateTexture()
{
	HRESULT result = S_OK;

	result = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(result));
	TexMetadata metaData= {};
	ScratchImage scratchImg = {};
	result = LoadFromWICFile(L"image/kyouryuu.png", WIC_FLAGS_IGNORE_SRGB, &metaData, scratchImg);
	assert(SUCCEEDED(result));
	auto img = scratchImg.GetImage(0, 0, 0);// ���f�[�^���o

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = metaData.format;
	resDesc.Width = metaData.width;
	resDesc.Height = metaData.height;
	resDesc.DepthOrArraySize = metaData.arraySize;
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	resDesc.MipLevels = metaData.mipLevels;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuffer_));
	assert(SUCCEEDED(result));
	
	BmpLoder bmp("image/sample.bmp");
	auto bSize = bmp.GetBmpSize();
	auto& rawData = bmp.GetRawData();
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
	result = texBuffer_->WriteToSubresource(0,
		nullptr,
		img->pixels,	// ���f�[�^�A�h���X
		img->rowPitch,	// 1���C���T�C�Y
		img->slicePitch	// 1���T�C�Y
	);
	assert(SUCCEEDED(result));

	return true;
}

bool Dx12Wrapper::CreateBasicDescriptors()
{
	// SRV�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 2;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	auto result = dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&resViewHeap_));
	assert(SUCCEEDED(result));

	auto metaData = texBuffer_->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metaData.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);

	auto heapPos = resViewHeap_->GetCPUDescriptorHandleForHeapStart();
	dev_->CreateShaderResourceView(
		texBuffer_,
		&srvDesc,
		heapPos);

	heapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto cbDesc = constantBuffer_->GetDesc();
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constantBuffer_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = cbDesc.Width;
	dev_->CreateConstantBufferView(&cbvDesc, heapPos);
	return true;
}

bool Dx12Wrapper::CreateConstantBuffer()
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
		IID_PPV_ARGS(&constantBuffer_));
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
	/*auto eye = XMFLOAT3(0, 0, 10);
	auto target = XMFLOAT3(0, 0, 0);
	auto up = XMFLOAT3(0, 1, 0);
	tmpMat *= XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));*/
	XMMATRIX viewproj = XMMatrixLookAtRH(
		{ 0.0f, 100.0f, 300.0f, 1.0f },	// ���_
		{ 0.0f, 0.0f, 0.0f, 1.0f },		// �����X
		{ 0.0f, 1.0f, 0.0f,1.0f });		// ��(���̏�)

	// �v���W�F�N�V�����s��(�p�[�X�y�N�e�B�u�s��or�ˉe�s��)
	//tmpMat *= XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(wSize.width) / static_cast<float>(wSize.height), 0.1f, 300.0f);
	viewproj *= XMMatrixPerspectiveFovRH(XM_PIDIV2, // ��p(FOV)
		static_cast<float>(wSize.width) / static_cast<float>(wSize.height), 
		0.1f,	// �j�A(�߂�)
		300.0f);	//�@�t�@�[(����)

	constantBuffer_->Map(0, nullptr, (void**)&mappedBasicMatrix_);
	
	mappedBasicMatrix_->viewproj = viewproj;
	mappedBasicMatrix_->world = world;
	
	//XMStoreFloat4x4(mappedBasicMat, tmpMat);
	//*mat = tmpMat ;
	//constantBuffer_->Unmap(0, nullptr);

	
	return true;
}

bool Dx12Wrapper::CreatePipelineState()
{
	HRESULT result = S_OK;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	// IA(InputAssembler)
	// ���̓��C�A�E�g
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		// ���_���
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		0,
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
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
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
	plsDesc.DepthStencilState.DepthEnable = false;
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
	

	// ���[�g�V�O�l�`��
	D3D12_ROOT_SIGNATURE_DESC rsd = {};
	rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsd.NumParameters = 1;

	D3D12_ROOT_PARAMETER rp[1] = {};
	D3D12_DESCRIPTOR_RANGE range[2] = {};
	range[0].BaseShaderRegister = 0;	// 0 t0��\��
	range[0].NumDescriptors = 1;		//t0~t0�܂�
	range[0].OffsetInDescriptorsFromTableStart = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range[1].BaseShaderRegister = 0;	// 0 t0��\��
	range[1].NumDescriptors = 1;		//t0~t0�܂�
	range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[0].DescriptorTable.pDescriptorRanges = range;
	rp[0].DescriptorTable.NumDescriptorRanges = _countof(range);

	rsd.pParameters = rp;
	rsd.NumParameters = _countof(rp);

	// s0���`
	// �T���v���̒�`�A�T���v����uv��0������1�z���Ƃ��̎���
	// UV�����ƂɐF���Ƃ��Ă���Ƃ��̃��[�����w�肷�����
	D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc[0].ShaderRegister = 0;
	samplerDesc[0].RegisterSpace = 0;
	// WRAP�͌J��Ԃ���\��
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	
	samplerDesc[0].MaxAnisotropy = 0.0f;
	samplerDesc[0].MaxLOD = 0.0f;
	samplerDesc[0].MinLOD = 0.0f;
	samplerDesc[0].MipLODBias = 0.0f;


	rsd.pStaticSamplers = samplerDesc;
	rsd.NumStaticSamplers = _countof(samplerDesc);

	// �V�O�l�`���ݒ�
	ID3DBlob* sigBlob = nullptr;
	D3D12SerializeRootSignature(&rsd,
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
	vsBlob->Release();
	psBlob->Release();
	return true;
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

	// �R�}���h�L���[�쐬
	D3D12_COMMAND_QUEUE_DESC cmdQDesc = {};
	cmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQDesc.NodeMask = 0;
	cmdQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = dev_->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(&cmdQue_));
	assert(SUCCEEDED(result));

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
	result = dxgi_->CreateSwapChainForHwnd(cmdQue_,
		hwnd,
		&scDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&swapchain_);
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
	

	// �t�F���X�����(�X���b�h�Z�[�t�ɕK�v)
	dev_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	fenceValue_ = fence_->GetCompletedValue();

	// �����_�[�^�[�Q�b�g���쐬
	CreateRenderTargetDescriptorHeap();

	//// ���_�o�b�t�@���쐬
	CreateVertices();
	CreateVertexBuffer();
	// �C���f�b�N�X�o�b�t�@���쐬
	CreateIndices();
	CreateIndexBuffer();

	// �萔�o�b�t�@�쐬
	CreateConstantBuffer();
	// �e�N�X�`���쐬
	CreateTexture();
	CreateBasicDescriptors();


	


	if (!CreatePipelineState())
	{
		return false;
	}

	// �r���[�|�[�g
	viewPort_.TopLeftX = 0;
	viewPort_.TopLeftY = 0;
	viewPort_.Width = wSize.width;
	viewPort_.Height = wSize.height;
	viewPort_.MaxDepth = 1.0f;
	viewPort_.MinDepth = 0.0f;
	
	// �V�U�[��`�̐ݒ�
	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = wSize.width;
	scissorRect_.bottom = wSize.height;

	return true;
}

bool Dx12Wrapper::Update()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_, pipelineState_);
	// �����ɖ���(�R�}���h���X�g��ς�ł���)	
	auto bbIdx = swapchain_->GetCurrentBackBufferIndex();

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

	
	angle += 0.02;
	mappedBasicMatrix_->world = XMMatrixRotationY(angle);
	mappedBasicMatrix_->world *= XMMatrixRotationX(angle);
	//mappedBasicMatrix_->world *= XMMatrixTranslation(0, modelY, 0);
	// ���\�[�X�o���A��ݒ�v���[���g���烌���_�[�^�[�Q�b�g
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
	rtvHeap.ptr += bbIdx * rtvIncSize;
	cmdList_->OMSetRenderTargets(1, &rtvHeap, false, nullptr);
	// �w�i�F���N���A(�F�ς���)
	float clsCol[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	cmdList_->ClearRenderTargetView(rtvHeap, clsCol, 0, nullptr);

	cmdList_->SetGraphicsRootSignature(rootSig_);
	// �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	cmdList_->RSSetViewports(1, &viewPort_);
	cmdList_->RSSetScissorRects(1, &scissorRect_);

	ID3D12DescriptorHeap* deskHeaps[] = {resViewHeap_};
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, resViewHeap_->GetGPUDescriptorHandleForHeapStart());
	// �O�p�`�`��
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	//cmdList_->DrawInstanced(4, 1, 0, 0);
	cmdList_->IASetIndexBuffer(&ibView_);
	cmdList_->DrawIndexedInstanced(indices_.size(), 1, 0, 0, 0);

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
	// enum class���g���ĂȂ����ߌx�����o�邽�߃L���X�g
	D3D_FEATURE_LEVEL level = static_cast<D3D_FEATURE_LEVEL>(D3D_FEATURE_LEVEL_1_0_CORE);

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
