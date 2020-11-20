#include "Dx12Wrapper.h"
#include <Windows.h>
#include <cassert>
#include <d3dcompiler.h>
#include <string>
#include <random>
#include <DirectXTex.h>
#include <stdint.h>
#include <algorithm>
#include "../Application.h"
#include "../PMDLoder/PMDLoder.h"
#include "../Common/StrOperater.h"
#include "../Common.h"
#include "PMDResource.h"
#include "PMDActor.h"
#include "../PMDLoder/VMDMotion.h"
//#include "../BMPLoder/BmpLoder.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DxGuid.lib")

using namespace DirectX;
using namespace std;

namespace
{
	/// <summary>
	/// ���_�\����
	/// </summary>
	/*struct Vertex
	{
		XMFLOAT3 vertex;
		XMFLOAT2 uv;
		Vertex(XMFLOAT3 vert, XMFLOAT2 tuv) : vertex(vert), uv(tuv) {};
	};*/
	//vector<PMDVertex> vertices_;
	//vector<unsigned short>indices_;

	constexpr int minTexSize = 4;
	constexpr int maxTexHeight = 256;
	
	/// <summary>
	/// align�̔{���ɂ����l��Ԃ�
	/// </summary>
	/// <param name="value">�l</param>
	/// <param name="align">align�l</param>
	/// <returns>align�̔{���ɂ����l</returns>
	UINT AligndValue(UINT value, UINT align)
	{
		//return (value + (align - 1)) &~ (align-1);
		return value + (align - (value % align)) % align;
	};
	// ���ԃo�b�t�@�ꎞ�ێ��p
	vector<ComPtr<ID3D12Resource>>intermediateBuffList;

}

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
	const auto& vertices = pmdActor_->GetPMDModel().GetVertexData();
	vertexBuffer_ = CreateBuffer(vertices.size() * sizeof(vertices[0]));
	//���_�f�[�^�]��
	PMDVertex* mappedData = nullptr;
	auto result = vertexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(vertices.begin(), vertices.end(), mappedData);
	vertexBuffer_->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices[0]);
	vbView_.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(vertices[0]));
}

void Dx12Wrapper::CreateIndexBuffer()
{
	const auto& indices = pmdActor_->GetPMDModel().GetIndexData();
	indexBuffer_ = CreateBuffer(indices.size() * sizeof(indices[0]));
	//�C���f�b�N�X�f�[�^�]��
	auto forType = indices.back();
	decltype(forType)* mappedData = nullptr;
	auto result = indexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(indices.begin(), indices.end(), mappedData);
	indexBuffer_->Unmap(0, nullptr);

	// �C���f�b�N�X�r���[
	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = static_cast<UINT>(indices.size() * sizeof(indices[0]));
}

bool Dx12Wrapper::CreateMonoColorTexture(ColTexType colType, const Color col)
{	
	HRESULT result = S_OK;
	Size size = { minTexSize, minTexSize };
	std::vector<Color>texData(size.width * size.height);
	
	std::fill(texData.begin(), texData.end(), col);	// �S��0xff�ŏ�����
	
	D3D12_SUBRESOURCE_DATA subResData = {};
	subResData.pData = texData.data();
	subResData.RowPitch = sizeof(texData[0]) * size.width;
	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
	SetUploadTexure(subResData, colType);
	
	return true;
}

bool Dx12Wrapper::CreateGradationTexture(const Size size)
{
	std::vector<Color>texData(size.width * size.height);
	for (size_t i = 0; i < 256; ++i)
	{
		fill_n(&texData[i * 4], 4, Color(255 - i));	// rgb�S��0x00�ŏ�����
	}
	D3D12_SUBRESOURCE_DATA subResData = {};
	subResData.pData = texData.data();
	subResData.RowPitch = sizeof(texData[0]) * size.width;
	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
	SetUploadTexure(subResData, ColTexType::Grad);
	return true;
}

void Dx12Wrapper::SetUploadTexure(D3D12_SUBRESOURCE_DATA& subResData, ColTexType colType)
{
	auto& texture = defTextures_[static_cast<int>(colType)];
	// �]����
	auto width = subResData.RowPitch / sizeof(Color);
	auto height = subResData.SlicePitch / subResData.RowPitch;
	
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	auto result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(texture.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// �]����
	ComPtr<ID3D12Resource>intermediateBuff;	// ���ԃo�b�t�@
	auto buffSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);
	result = dev_->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(intermediateBuff.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	intermediateBuffList.push_back(intermediateBuff);
	
	// �R�}���h���X�g�ɓo�^
	// ����CopyTextureRegion�������Ă��邽��
	// �R�}���h�L���[�҂����K�v
	UpdateSubresources(cmdList_.Get(), texture.Get(),
		intermediateBuff.Get(), 0, 0, 1, &subResData);
	cmdList_->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

bool Dx12Wrapper::CreateDefaultTextures()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);
	defTextures_.resize(static_cast<int>(ColTexType::Max));
	// cmdList�Ƀe�N�X�`���f�[�^��ς�
	CreateMonoColorTexture(ColTexType::White,Color(0xff));	// ��
	CreateMonoColorTexture(ColTexType::Black,Color(0x00));	// ��
	CreateGradationTexture({ minTexSize ,256 });	// �O���f
	cmdList_->Close();
	ExecuteAndWait();
	intermediateBuffList.clear();
	return true;
}

bool Dx12Wrapper::CreateBasicDescriptors()
{
	auto& transResBind = pmdResource_->GetGroops(GroopType::TRANSFORM);
	transResBind.Init({ BuffType::CBV, BuffType::CBV });
	transResBind.AddBuffers(transformBuffer_.Get());
	transResBind.AddBuffers(boneBuffer_.Get());

	return true;
}

bool Dx12Wrapper::CreateTransformBuffer()
{
	transformBuffer_ = CreateBuffer(AligndValue(sizeof(BasicMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
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
	world *= XMMatrixRotationY(XM_PI);

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

	mappedBasicMatrix_ = make_shared<BasicMatrix>();
	// ��ł����邽�߂ɊJ���������ɂ��Ă���
	transformBuffer_->Map(0, nullptr, (void**)&mappedBasicMatrix_);
	
	mappedBasicMatrix_->viewproj = viewproj;
	mappedBasicMatrix_->world = world;
	return true;
}

bool Dx12Wrapper::CreateDepthBufferView()
{
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	auto rtvDesc = bbResouces[0]->GetDesc();

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		rtvDesc.Width,
		rtvDesc.Height);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	CD3DX12_CLEAR_VALUE clearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

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

	dev_->CreateDepthStencilView(depthBuffer_.Get(),
		&dsvDesc,
		depthDescHeap_->GetCPUDescriptorHandleForHeapStart());
	return true;
}

bool Dx12Wrapper::CreateMaterialBufferView()
{
	// �}�e���A���o�b�t�@�̍쐬
	HRESULT result = S_OK;
	auto& mats = pmdActor_->GetPMDModel().GetMaterialData();
	auto strideBytes = AligndValue(sizeof(BasicMaterial), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	materialBuffer_ = CreateBuffer(mats.size() * strideBytes);
	auto gAddress = materialBuffer_->GetGPUVirtualAddress();
	uint8_t* mappedMaterial = nullptr;
	result = materialBuffer_->Map(0, nullptr, (void**)&mappedMaterial);
		assert(SUCCEEDED(result));

	auto& transResBind = pmdResource_->GetGroops(GroopType::MATERIAL);
	transResBind.Init({ BuffType::CBV, BuffType::SRV, BuffType::SRV, BuffType::SRV, BuffType::SRV });
	array<pair<string, ID3D12Resource*>, 4>texPairList;
	texPairList = { make_pair("bmp",defTextures_[static_cast<int>(ColTexType::White)].Get()),
					make_pair("sph",defTextures_[static_cast<int>(ColTexType::White)].Get()),
					make_pair("spa",defTextures_[static_cast<int>(ColTexType::Black)].Get()),
					make_pair("toon",defTextures_[static_cast<int>(ColTexType::Grad)].Get()) };
	for (int i = 0; i < mats.size(); ++i)
	{
		// �}�e���A���萔�o�b�t�@�r���[
		((BasicMaterial*)mappedMaterial)->diffuse = mats[i].diffuse;
		((BasicMaterial*)mappedMaterial)->ambient = mats[i].ambient;
		((BasicMaterial*)mappedMaterial)->speqular = mats[i].speqular;
		((BasicMaterial*)mappedMaterial)->alpha = mats[i].alpha;
		((BasicMaterial*)mappedMaterial)->speqularity = mats[i].speqularity;

		transResBind.AddBuffers(materialBuffer_.Get(), strideBytes);
		mappedMaterial += strideBytes;
		for (auto texpair : texPairList)
		{
			ID3D12Resource* res = pmdActor_->GetTextures(texpair.first)[i].Get();
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

bool Dx12Wrapper::CreateBoneBuffer()
{
	HRESULT result = S_OK;
	auto size = AligndValue(sizeof(XMFLOAT4X4) * 512, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	boneBuffer_ = CreateBuffer(size);
	result = boneBuffer_->Map(0, nullptr, (void**)&mappedBone_);
	assert(SUCCEEDED(result));
	const auto& bData = pmdActor_->GetPMDModel().GetBoneData();
	for (int i = 0; i < bData.size(); ++i)
	{
		boneTable_.emplace(bData[i].name, i);
	}
	// �S���P�ʍs��ɏ�����
	fill_n(mappedBone_, 512, XMMatrixIdentity());	
	
	//vector<string> nameTbl = { "���Ђ�","���r","�E�Ђ�","�E�r" };
	//float rotatetable[] = { XM_PIDIV2, XM_PIDIV4,-XM_PIDIV2, -XM_PIDIV4 };
	//UpdateBones(0);
	// �l��map���Ă�����̂ɃR�s�[
	
	return true;
}

void Dx12Wrapper::UpdateBones(int currentFrameNo)
{
	const auto& bData = pmdActor_->GetPMDModel().GetBoneData();
	auto mats = pmdActor_->GetPMDModel().GetBoneMat();
	mats.resize(bData.size());
	fill(mats.begin(), mats.end(), XMMatrixIdentity());

	auto a = vmdMotion_->GetVMDData().data;
	for (auto& motion : vmdMotion_->GetVMDData().data)
	{		
		if (boneTable_.find(motion.first) == boneTable_.end())
		{
			continue;
		}
		auto bidx = boneTable_[motion.first];
		auto& bpos = bData[bidx].pos;
		auto rit = find_if(motion.second.rbegin(), motion.second.rend(),
			[currentFrameNo](const auto& v)
			{
				return v.frameNo <= currentFrameNo;
			});

		auto q = XMLoadFloat4(&motion.second[0].quaternion);
		XMFLOAT3 mov(0, 0, 0);
		if (rit != motion.second.rend())
		{		
			mov = rit->pos;
			q = XMLoadFloat4(&rit->quaternion);
			auto it = rit.base();
			if (it != motion.second.end())
			{
				auto t = static_cast<float>((currentFrameNo - rit->frameNo)) / 
					static_cast<float>((it->frameNo - rit->frameNo));
				//q = (1.0f - t) * q + t * XMLoadFloat4(&it->quaternion);
				q = XMQuaternionSlerp(q, XMLoadFloat4(&it->quaternion), t);
				/*auto vPos = XMVectorScale(XMLoadFloat3(&mov), (1.0f - t)) + 
							XMVectorScale(XMLoadFloat3(&it->pos),  t);*/
				auto vPos = XMVectorLerp(XMLoadFloat3(&mov), XMLoadFloat3(&it->pos), t);
				XMStoreFloat3(&mov,vPos);
			}	
		}
		mats[bidx] = XMMatrixIdentity();
		mats[bidx] *= XMMatrixTranslation(-bpos.x, -bpos.y, -bpos.z);
		// ��]
		mats[bidx] *= XMMatrixRotationQuaternion(q);
	
		mats[bidx] *= XMMatrixTranslation(bpos.x, bpos.y , bpos.z);
		// �ړ�
		mats[bidx] *= XMMatrixTranslation(mov.x, mov.y, mov.z);
	}
	RecursiveCalucurate(bData, mats, 0);
	copy(mats.begin(), mats.end(), mappedBone_);
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
	}
}

ComPtr<ID3D12Resource> Dx12Wrapper::CreateBuffer(size_t size, D3D12_HEAP_TYPE heapType)
{
	ComPtr<ID3D12Resource>ret;
	CD3DX12_HEAP_PROPERTIES heapProp(heapType);

	// �m�ۂ���p�r(Resource)�Ɋւ���ݒ�
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

void Dx12Wrapper::RecursiveCalucurate(const std::vector<PMDBone>& bones, std::vector<DirectX::XMMATRIX>& mats, int idx)
{
	auto& mat = mats[idx];
	auto& bone = bones[idx];
	for (auto child : bone.children)
	{
		mats[child] *= mat;
		RecursiveCalucurate(bones,mats, child);
	}
}

float Dx12Wrapper::CalucurateFromBezier(float x, DirectX::XMFLOAT2 bz[2])
{
	// �x�W�F��������������x��Ԃ�
	if (bz[0].x == bz[1].x && bz[0].y == bz[1].y)
	{
		return x;
	}
	// P0(0.0)*(1-t)^3 + 3*P1*(1-t)^2*t + 3* P2*(1-t)*t^2 + P3(1,1)*(1-t)^3
	// ---t = f(x);
	// 3*P1.x*(1-t)^2*t + 3 * P2.x*(1-t)*t^2 + (1-t)^3
	// 3*P1.x*(t - 2t^2 + t^3) + 3*P2.x*(t^2 - t^3) + t^3
	// �����ŕ�����
	// t^3 = 3*P1.x + 3*P2.x +1
	// t^2 = -6*P1.x + 3*P2.x;
	// t = 3*P1.x

	return 0.0f;
}

float Dx12Wrapper::CalucurateFromBezier(float x, DirectX::XMFLOAT2 bezier[4])
{
	return 0.0f;
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
	ComPtr<ID3D12Debug3> debug = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf()));
	debug->EnableDebugLayer();
#endif
	CheckFeatureLevel();
#if _DEBUG
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgi_.ReleaseAndGetAddressOf()));
#else 
	result = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgi_));
#endif
	assert(SUCCEEDED(result));

	InitCommandSet();
	
	pmdActor_ = make_shared<PMDActor>(dev_.Get());
	//const char* modelPath = "Resource/PMD/���~�N/mikuXS���~�N.pmd";
	//const char* modelPath = "Resource/PMD/�_��/�_��Ver1.10SW.pmd";
	const char* modelPath = "Resource/PMD/model/�����~�N.pmd";
	//const char* modelPath = "Resource/PMD/model/�������J.pmd";
	//const char* modelPath = "Resource/PMD/��ߔe��v1.0/��ߔe��v1.pmd";
	//const char* modelPath = "Resource/PMD/�Ö��n���Ƃ�/�Ö��n���Ƃ�152Normal.pmd";
	//const char* modelPath = "Resource/PMD/�얲/reimu_F02.pmd";
	pmdActor_->LoadModel(modelPath);
	vmdMotion_ = make_shared<VMDLoder>();
	vmdMotion_->Load("Resource/VMD/swing2.vmd");
	
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
	pmdResource_ = make_shared<PMDResource>(dev_.Get());
	// �萔�o�b�t�@�쐬
	CreateTransformBuffer();
	
	// �e�N�X�`���쐬
	pmdActor_->CreatePMDModelTexture();
	// ��{�e�N�X�`���쐬
	CreateDefaultTextures();

	// �}�e���A���o�b�t�@�̍쐬
	CreateMaterialBufferView();
	
	// �{�[���o�b�t�@�쐬
	CreateBoneBuffer();

	// ���W�ϊ�SRV�p�f�B�X�N���v�^�q�[�v�쐬
	CreateBasicDescriptors();
	
	// ���\�[�X�f�[�^���r���h
	pmdResource_->Build({ GroopType::TRANSFORM, GroopType::MATERIAL });
	// �r���[�|�[�g�ƃV�U�[��`������
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
	static auto lastTime = GetTickCount();
	static size_t frame;
	
	//transResBind.AddBuffers(boneBuffer_.Get());
	// ���f���𓮂���
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
	angle = 0.02f;
	mappedBasicMatrix_->world *= XMMatrixRotationY(angle);
	mappedBasicMatrix_->world *= XMMatrixTranslation(0, modelY, 0);
	UpdateBones(frame);
	// ��ʃN���A
	//ClearDrawScreen();
	
	// �`�揈��
	DrawPMDModel();

	// �o�b�t�@�t���b�v
	//DrawExcute();
	frame = static_cast<float>((GetTickCount() - lastTime)) / (1000.0f / 30.0f);
	frame = frame % vmdMotion_->GetVMDData().duration;
	return true;
}

void Dx12Wrapper::DrawExcute()
{
	// ���\�[�X�o���A��ݒ背���_�[�^�[�Q�b�g����v���[���g
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		bbResouces[bbIdx_].Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_RENDER_TARGET,	// �O�^�[�Q�b�g
		D3D12_RESOURCE_STATE_PRESENT	// ���^�[�Q�b�g
	);
	cmdList_->ResourceBarrier(1, &barrier);
	cmdList_->Close();

	ExecuteAndWait();
	swapchain_->Present(1, 0);
}

void Dx12Wrapper::ClearDrawScreen()
{
	// �o�b�N�o�b�t�@�C���f�b�N�X�ݒ�
	bbIdx_ = swapchain_->GetCurrentBackBufferIndex();

	// �R�}���h���Z�b�g
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);
	// ���\�[�X�o���A��ݒ�v���[���g���烌���_�[�^�[�Q�b�g
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		bbResouces[bbIdx_].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	cmdList_->ResourceBarrier(1, &barrier);

	// �����_�[�^�[�Q�b�g���Z�b�g	
	auto rtvHeap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto rtvIncSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHeap.ptr += static_cast<SIZE_T>(bbIdx_)* rtvIncSize;
	cmdList_->OMSetRenderTargets(1, &rtvHeap, false, &depthDescHeap_->GetCPUDescriptorHandleForHeapStart());
	// ��ʂ��N���A(�F�ς���)
	float clsCol[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	cmdList_->ClearRenderTargetView(rtvHeap, clsCol, 0, nullptr);
	cmdList_->ClearDepthStencilView(
		depthDescHeap_->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);

	// �V�O�l�`���Z�b�g
	cmdList_->SetGraphicsRootSignature(pmdResource_->GetRootSignature().Get());
	// �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	CD3DX12_VIEWPORT vp(bbResouces[bbIdx_].Get());	// ����łł��邪�����ł��Ȃ�
	cmdList_->RSSetViewports(1, &vp);
	cmdList_->RSSetScissorRects(1, &scissorRect_);
}

void Dx12Wrapper::DrawPMDModel()
{

	cmdList_->SetPipelineState(pmdResource_->GetPipelineState().Get());
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	cmdList_->IASetIndexBuffer(&ibView_);

	// ���W�ϊ��q�[�v�Z�b�g
	auto resHeap = pmdResource_->GetGroops(GroopType::TRANSFORM).descHeap_.Get();
	ID3D12DescriptorHeap* deskHeaps[] = { resHeap/*resViewHeap_.Get()*/ };
	auto heapPos = /*resViewHeap_*/resHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, heapPos);

	// �}�e���A��&�e�N�X�`���q�[�v�Z�b�g
	auto material = pmdActor_->GetPMDModel().GetMaterialData();
	uint32_t indexOffset = 0;
	auto matHeap = pmdResource_->GetGroops(GroopType::MATERIAL).descHeap_.Get();
	ID3D12DescriptorHeap* matDeskHeaps[] = { matHeap };
	cmdList_->SetDescriptorHeaps(1, matDeskHeaps);
	auto materialHeapPos = matHeap->GetGPUDescriptorHandleForHeapStart();
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

	
}

void Dx12Wrapper::ExecuteAndWait()
{
	ID3D12CommandList* cmdLists[] = { cmdList_.Get() };
	cmdQue_->ExecuteCommandLists(1, cmdLists);
	cmdQue_->Signal(fence_.Get(), ++fenceValue_);
	// Execute�����܂ő҂���
	while (true)
	{
		if (fence_->GetCompletedValue() == fenceValue_)
		{
			break;
		}
	}
}

void Dx12Wrapper::Terminate()
{
}

ID3D12Device* Dx12Wrapper::GetDevice()
{
	return dev_.Get();
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
		auto result = D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf()));
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
	auto result = dev_->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(cmdQue_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// �R�}���h�A���P�[�^�[�̍쐬
	dev_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdAllocator_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// �R�}���h���X�g�̍쐬
	dev_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator_.Get(), nullptr,
		IID_PPV_ARGS(cmdList_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	cmdList_->Close();
	return true;
}

bool Dx12Wrapper::CreateSwapChain(const HWND& hwnd)
{
	// swapchainDesc�쐬
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = static_cast<UINT>(wSize.width);
	scDesc.Height = static_cast<UINT>(wSize.height);
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferCount = 2;	// �\�Ɨ����
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.Flags = 0/*DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH*/;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Stereo = false;	// VR�̎�true
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ComPtr<IDXGISwapChain1>swapchain;
	// swapchain�쐬
	auto result = dxgi_->CreateSwapChainForHwnd(cmdQue_.Get(),
		hwnd,
		&scDesc,
		nullptr,
		nullptr,
		&swapchain);
	assert(SUCCEEDED(result));

	result = swapchain.As(&swapchain_);
	assert(SUCCEEDED(result));
	return true;
}

bool Dx12Wrapper::CreateFence()
{
	auto& app = Application::GetInstance();
	auto wSize = app.GetWindowSize();
	// �t�F���X�����(�X���b�h�Z�[�t�ɕK�v)
	dev_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()));
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
		dev_->CreateRenderTargetView(bbResouces[i].Get(), &rtvDesc, heap);	// �L�����p�X�ƐE�l��R�Â���
		heap.ptr += incSize;// �E�l�ƃL�����o�X�̃y�A�̂Ԃ񎟂̂Ƃ���܂ŃI�t�Z�b�g
	}

	return SUCCEEDED(result);
}
