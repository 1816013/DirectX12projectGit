#include "Dx12Wrapper.h"
#include <cassert>
#include <d3dcompiler.h>
#include "../Application.h"
#include "../Common/StrOperater.h"
#include "../Common.h"
#include "PMDResource.h"
#include "PMDActor.h"
#include "../PMDLoder/PMDLoder.h"
#include "TexManager.h"
#include "Renderer.h"
#include "mesh/PrimitiveManager.h"
#include "Effect/EffectManager.h"
#include "Camera/cameraCtr.h"
#include "Camera/Camera.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"

//#include "../BMPLoder/BmpLoder.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DxGuid.lib")

using namespace DirectX;
using namespace std;

namespace
{
	//vector<PMDVertex> vertices_;
	//vector<unsigned short>indices_;

	constexpr int minTexSize = 4;
	constexpr int maxTexHeight = 256;
		
	// ���ԃo�b�t�@�ꎞ�ێ��p
	vector<ComPtr<ID3D12Resource>>intermediateBuffList_;
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

//bool Dx12Wrapper::CreateMonoColorTexture(ColTexType colType, const Color col)
//{	
//	HRESULT result = S_OK;
//	Size size = { minTexSize, minTexSize };
//	std::vector<Color>texData(size.width * size.height);
//	
//	std::fill(texData.begin(), texData.end(), col);	// �S��0xff�ŏ�����
//	
//	D3D12_SUBRESOURCE_DATA subResData = {};
//	subResData.pData = texData.data();
//	subResData.RowPitch = sizeof(texData[0]) * size.width;
//	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
//	SetUploadTexure(subResData, colType);
//	
//	return true;
//}
//
//bool Dx12Wrapper::CreateGradationTexture(const Size size)
//{
//	std::vector<Color>texData(size.width * size.height);
//	for (size_t i = 0; i < 256; ++i)
//	{
//		fill_n(&texData[i * 4], 4, Color(static_cast<uint8_t>(255 - i)));	// rgb�S��0x00�ŏ�����
//	}
//	D3D12_SUBRESOURCE_DATA subResData = {};
//	subResData.pData = texData.data();
//	subResData.RowPitch = sizeof(texData[0]) * size.width;
//	subResData.SlicePitch = sizeof(texData[0]) * size.width * size.height;
//	SetUploadTexure(subResData, ColTexType::Grad);
//	return true;
//}
//
//void Dx12Wrapper::SetUploadTexure(D3D12_SUBRESOURCE_DATA& subResData, ColTexType colType)
//{
//	defTextures_.resize(static_cast<int>(ColTexType::Max));
//	auto& texture = defTextures_[static_cast<int>(colType)];
//	// �]����
//	auto width = subResData.RowPitch / sizeof(Color);
//	auto height = subResData.SlicePitch / subResData.RowPitch;
//	
//	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
//	D3D12_RESOURCE_DESC resDesc = 
//		CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 
//			width,
//			static_cast<UINT>( height));
//	auto result = dev_->CreateCommittedResource(&heapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&resDesc,
//		D3D12_RESOURCE_STATE_COPY_DEST,
//		nullptr,
//		IID_PPV_ARGS(texture.ReleaseAndGetAddressOf()));
//	assert(SUCCEEDED(result));
//
//	// �]����
//	ComPtr<ID3D12Resource>intermediateBuff;	// ���ԃo�b�t�@
//	auto buffSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);
//	auto intermediateHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
//	auto intermediateResDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
//
//	result = dev_->CreateCommittedResource(&intermediateHeapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&intermediateResDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(intermediateBuff.ReleaseAndGetAddressOf()));
//	assert(SUCCEEDED(result));
//	intermediateBuffList_.push_back(intermediateBuff);
//	
//	// �R�}���h���X�g�ɓo�^
//	// ����CopyTextureRegion�������Ă��邽��
//	// �R�}���h�L���[�҂����K�v
//	UpdateSubresources(cmdList_.Get(), texture.Get(),
//		intermediateBuff.Get(), 0, 0, 1, &subResData);
//	cmdList_->ResourceBarrier(1,
//		&CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(),
//			D3D12_RESOURCE_STATE_COPY_DEST,
//			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
//}

bool Dx12Wrapper::CreateDefaultTextures()
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);
	// cmdList�Ƀe�N�X�`���f�[�^��ς�
	auto& texMng = TexManager::GetInstance();
	texMng.CreateMonoColorTexture(cmdList_.Get(),ColTexType::White,Color(0xff));	// ��
	texMng.CreateMonoColorTexture(cmdList_.Get(), ColTexType::Black,Color(0x00));	// ��
	texMng.CreateGradationTexture(cmdList_.Get(), { minTexSize ,256 });	// �O���f
	cmdList_->Close();
	ExecuteAndWait();
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
		IID_PPV_ARGS(depthBuffer_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC desDesc = {};
	desDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desDesc.NumDescriptors = 1;
	desDesc.NodeMask = 0;
	desDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = dev_->CreateDescriptorHeap(&desDesc,
		IID_PPV_ARGS(depthDescHeap_.ReleaseAndGetAddressOf()));
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

void Dx12Wrapper::CreateRenderTargetTexture()
{
	HRESULT result = S_OK;
	// �o�b�t�@�̍쐬
	auto bbResDesc = bbResouces[0]->GetDesc();
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(bbResDesc.Format, bbResDesc.Width, bbResDesc.Height);
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	
	D3D12_CLEAR_VALUE clearValue = {};
	copy_n(clearCol, 4, clearValue.Color);
	clearValue.Format = resDesc.Format;
	for (auto& rtTex : rtTextures_)
	{
		result = dev_->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&clearValue,
			IID_PPV_ARGS(rtTex.ReleaseAndGetAddressOf())
		);
		assert(SUCCEEDED(result));
	}
	// SSAO�̃o�b�t�@
	auto ssaoResDesc = resDesc;
	ssaoResDesc.Format = DXGI_FORMAT_R32_FLOAT;
	auto ssaoClearValue = clearValue;
	ssaoClearValue.Format = DXGI_FORMAT_R32_FLOAT;
	result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&ssaoResDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&ssaoClearValue,
		IID_PPV_ARGS(ssaoBuffer_.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));
	resDesc.Width >>= 1;
	result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(rtShrinkForBloom_.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));
	// ��ʊE�[�x�p�k���o�b�t�@
	result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(rtShrinkForDof_.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@
	boardConstBuffer_ = CreateBuffer(Common::AligndValue(sizeof(BoardConstBuffer), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
	boardConstBuffer_->Map(0, nullptr, (void**)&mappedBoardBuffer_);
	const auto& proj = cameraCtr_->GetCamera().GetCameaProj();
	mappedBoardBuffer_->proj = proj;
	XMVECTOR det;

	mappedBoardBuffer_->invProj = XMMatrixInverse(&det, proj);
	mappedBoardBuffer_->pos = XMFLOAT2(0, 0);
	mappedBoardBuffer_->bloomActive = true;
	mappedBoardBuffer_->dofActive = true;
	mappedBoardBuffer_->outLineD = true;
	mappedBoardBuffer_->outLineN = true;
	mappedBoardBuffer_->ssaoActive = true;
	mappedBoardBuffer_->bloomCol[0] = 1;
	mappedBoardBuffer_->bloomCol[1] = 1;
	mappedBoardBuffer_->bloomCol[2] = 1;
	// 3�̃����_�[�^�[�Q�b�g�̍쐬
	// �����_�[�^�[�Q�b�g�r���[
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = 6;// �}���`�p�X�����_�[�^�[�Q�b�g
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = dev_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(firstRtvHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = resDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	auto rtvHeapPos = firstRtvHeap_->GetCPUDescriptorHandleForHeapStart();
	for (auto& rtTex : rtTextures_)
	{
		dev_->CreateRenderTargetView(rtTex.Get(), &rtvDesc, rtvHeapPos);
		rtvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	// �k�������_�[�^�[�Q�b�g(Bulr�p)
	dev_->CreateRenderTargetView(rtShrinkForBloom_.Get(), &rtvDesc, rtvHeapPos);
	rtvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// �k�������_�[�^�[�Q�b�g(Dof�p)
	dev_->CreateRenderTargetView(rtShrinkForDof_.Get(), &rtvDesc, rtvHeapPos);
	rtvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// SSAO�����_�[�^�[�Q�b�g
	auto ssaoRtvDesc = rtvDesc;
	ssaoRtvDesc.Format = ssaoResDesc.Format;
	dev_->CreateRenderTargetView(ssaoBuffer_.Get(), &ssaoRtvDesc, rtvHeapPos);
	rtvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	// �V�F�[�_�[���\�[�X�r���[
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; 
	srvHeapDesc.NodeMask = 0;
	srvHeapDesc.NumDescriptors = 10;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = dev_->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(firstSrvHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	auto srvHeapPos = firstSrvHeap_->GetCPUDescriptorHandleForHeapStart();

	// 1�p�X�ڂ̃����_�����O����(t1:0)
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = resDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dev_->CreateShaderResourceView(rtTextures_[0].Get(), &srvDesc, srvHeapPos);
	// �m�[�}���}�b�v(t1:2)
	srvDesc.Format = normalMapTex_->GetDesc().Format;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(normalMapTex_.Get(), &srvDesc, srvHeapPos);
	// ���C�g�[�x(t2:3)
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(shadowDepthBuffer_.Get(), &srvDesc, srvHeapPos);
	// �[�x�r���[(t3:4)
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(depthBuffer_.Get(), &srvDesc, srvHeapPos);
	// 1�p�X�ڂ̖@��(t4, 5)
	srvDesc.Format = resDesc.Format;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(rtTextures_[1].Get(), &srvDesc, srvHeapPos);

	// ���P�x�`��(t5,6)
	srvDesc.Format = resDesc.Format;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(rtTextures_[2].Get(), &srvDesc, srvHeapPos);

	// ���P�x�k���o�b�t�@(t6,7)
	srvDesc.Format = resDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(rtShrinkForBloom_.Get(), &srvDesc, srvHeapPos);

	// ��ʊE�[�x�k���o�b�t�@(t7,8)
	srvDesc.Format = resDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(rtShrinkForDof_.Get(), &srvDesc, srvHeapPos);

	// SSAO�o�b�t�@(t8,9)
	srvDesc.Format = ssaoResDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(ssaoBuffer_.Get(), &srvDesc, srvHeapPos);

	// �萔�o�b�t�@(�{�[�h�g�����X�t�H�[��)(d0, 10)
	
	mappedBoardBuffer_->time = 1; 
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	auto cbDesc = boardConstBuffer_->GetDesc();
	cbvDesc.BufferLocation = boardConstBuffer_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(cbDesc.Width);
	srvHeapPos.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateConstantBufferView(&cbvDesc, srvHeapPos);
	
}

void Dx12Wrapper::CreateBoardPolyVerts()
{
	XMFLOAT3 verts[] = { {-1,-1,0.1f},
	{-1,1,0.1f},
	{1,-1,0.1f},
	{1,1,0.1f} };
	boardPolyVerts_ = CreateBuffer(sizeof(verts));
	XMFLOAT3* mappedData = nullptr;
	boardPolyVerts_->Map(0, nullptr, (void**)&mappedData);
	copy(begin(verts), end(verts), mappedData);
	boardPolyVerts_->Unmap(0, nullptr);

	// �r���[�쐬
	boardVBView_.BufferLocation = boardPolyVerts_->GetGPUVirtualAddress();
	boardVBView_.StrideInBytes = sizeof(verts[0]);
	boardVBView_.SizeInBytes = static_cast<UINT>(sizeof(verts));

}

void Dx12Wrapper::CreateBoardPipeline()
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
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}		
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���_�V�F�[�_
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/boardVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_1",
		0,
		0, vsBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));
	plsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	// �s�N�Z���V�F�[�_
	ComPtr<ID3DBlob> psBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/boardPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_1",
		0,
		0, psBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	plsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// ���X�^���C�U�ݒ�
	plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	// ���̑��ݒ�
	// �f�v�X�ƃX�e���V���ݒ�
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState.DepthEnable = false;
	//plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// �o�͐ݒ�
	plsDesc.NumRenderTargets = 2;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	plsDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �u�����h
	plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// ���[�g�V�O�l�`������
	// ���[�g�V�O�l�`��
	D3D12_ROOT_PARAMETER rp[1] = {};
	D3D12_DESCRIPTOR_RANGE range[2] = {};

	// �����W
	// �s��萔�o�b�t�@
	range[0] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // �����W�^�C�v t
		9,// �f�X�N���v�^��	t0�`t5�܂�	1:1�p�X��,2:�c�ݗp,3:���C�g�p�X, 4: �[�x�e�N�X�`��, 5: 1�p�X�ږ@��, 6: ���P�x 7:���P�x�k�� 8: �k�� 9: SSAO
		0);// �x�[�X���W�X�^�ԍ� t0	

	range[1] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // �����W�^�C�v d
		1,// �f�X�N���v�^��	d0�`d0�܂�
		0);// �x�[�X���W�X�^�ԍ� d0	

	// ���[�g�p�����[�^
	// ���W�ϊ�
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[0],	// ���[�g�p�����[�^
		2,		// �����W��
		&range[0],// �����W�擪�A�h���X
		D3D12_SHADER_VISIBILITY_ALL);	

	D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
	samplerDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	/*samplerDesc->AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc->AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;*/

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc(1, rp, 1, samplerDesc);
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// �V�O�l�`���ݒ�
	//ComPtr<ID3DBlob> errBlob = nullptr;
	ComPtr<ID3DBlob> sigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sigBlob,
		&errBlob);
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	// ���[�g�V�O�l�`���̐���
	result = dev_->CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(boardSig_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	plsDesc.pRootSignature = boardSig_.Get();

	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(boardPipeLine_.ReleaseAndGetAddressOf()));

	// �k���o�b�t�@
	psBlob->Release();
	// �s�N�Z���V�F�[�_
	result = D3DCompileFromFile(L"Shader/boardPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"ShrinkPSForBloom", "ps_5_1",
		0,
		0, psBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	plsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(shrinkPipeLine_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void Dx12Wrapper::CreateShadowMapBufferAndView()
{
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32_TYPELESS,
		1024,
		1024);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	CD3DX12_CLEAR_VALUE clearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	auto result = dev_->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(shadowDepthBuffer_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC desDesc = {};
	desDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desDesc.NumDescriptors = 1;
	desDesc.NodeMask = 0;
	desDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = dev_->CreateDescriptorHeap(&desDesc,
		IID_PPV_ARGS(shadowDSVHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	dev_->CreateDepthStencilView(shadowDepthBuffer_.Get(),
		&dsvDesc,
		shadowDSVHeap_->GetCPUDescriptorHandleForHeapStart());

	D3D12_DESCRIPTOR_HEAP_DESC srvDesDesc = {};
	srvDesDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesDesc.NumDescriptors = 1;
	srvDesDesc.NodeMask = 0;
	srvDesDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = dev_->CreateDescriptorHeap(&srvDesDesc,
		IID_PPV_ARGS(shadowSRVHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// ���f���ɐ[�x�l���Z�b�g
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	for (auto& actor : pmdActor_)
	{
		dev_->CreateShaderResourceView(shadowDepthBuffer_.Get(),
			&srvDesc,
			shadowSRVHeap_->GetCPUDescriptorHandleForHeapStart());
		auto& shadowResBind = actor->GetPMDResource().GetGroops(GroopType::DEPTH);
		shadowResBind.AddBuffers(shadowDepthBuffer_.Get());
	}

}

void Dx12Wrapper::CreateShadowPipeline()
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
		// �@�����
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// UV���
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// �{�[���ԍ� 2�o�C�g�����^
		{"BONE_NO", 0, DXGI_FORMAT_R16G16_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		//  2�o�C�g�����^
		{"WEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���_�V�F�[�_
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/ShadowVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"ShadowVS", "vs_5_1",
		0,
		0, vsBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));
	plsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());

	ComPtr<ID3DBlob> psBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/ShadowVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"ShadowPS", "ps_5_1",
		0,
		0, psBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));
	plsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// ���X�^���C�U�ݒ�
	plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plsDesc.RasterizerState.FrontCounterClockwise = true;
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// �o�͐ݒ�
	plsDesc.NumRenderTargets = 0;
	//plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ���̑��ݒ�
	// �f�v�X�ƃX�e���V���ݒ�
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// �u�����h
	plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// �V�O�l�`���ݒ�
	ComPtr<ID3DBlob> sigBlob = nullptr;
	result = D3DGetBlobPart(vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		D3D_BLOB_ROOT_SIGNATURE, 0, &sigBlob);
	assert(SUCCEEDED(result));
	// ���[�g�V�O�l�`���̐���
	result = dev_->CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(shadowSig_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	plsDesc.pRootSignature = shadowSig_.Get();
	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(shadowPipeline_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void Dx12Wrapper::DrawShadow(BasicMatrix& mat)
{
	cmdList_->SetPipelineState(shadowPipeline_.Get());
	cmdList_->SetGraphicsRootSignature(shadowSig_.Get());
	
	auto dsvHandle = shadowDSVHeap_->GetCPUDescriptorHandleForHeapStart();
	cmdList_->OMSetRenderTargets(0, nullptr, false, &dsvHandle);
	cmdList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	XMVECTOR camPos = XMVectorScale( mat.lightPos, 20)/*{ -20,20,20 }*/;
	XMVECTOR direction = -mat.lightPos;//{ 1,-1,-1 };
	mat.lightVP = XMMatrixLookToRH(camPos, direction, { 0,1,0,0 });
	mat.lightVP *= XMMatrixOrthographicRH(50.0f, 50.0f, 1.0f, 500.0f);

	// �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	auto depthDesk = shadowDepthBuffer_->GetDesc();
	CD3DX12_VIEWPORT vp(shadowDepthBuffer_.Get());
	cmdList_->RSSetViewports(1, &vp);
	auto scissorRect = CD3DX12_RECT(0, 0, static_cast<UINT>(depthDesk.Width), depthDesk.Height);
	cmdList_->RSSetScissorRects(1, &scissorRect);

	// �`�施��
	for (auto& actor : pmdActor_)
	{
		const auto& instID = actor->GetInstID();
		auto descHeap = actor->GetPMDResource().GetGroops(GroopType::TRANSFORM).descHeap_.Get();
		cmdList_->SetDescriptorHeaps(1, &descHeap);
		cmdList_->SetGraphicsRootDescriptorTable(0, descHeap->GetGPUDescriptorHandleForHeapStart());
		cmdList_->DrawIndexedInstanced(static_cast<UINT>(actor->GetPMDModel().GetIndexData().size()), instID.x * instID.y, 0, 0, 0);
	}
	
	//// ���\�[�X�o���A��ݒ�f�v�X����s�N�Z���V�F�[�_
	//auto depthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
	//	depthBuffer_.Get(),	// ���\�[�X
	//	D3D12_RESOURCE_STATE_DEPTH_WRITE,	// �O�^�[�Q�b�g
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// ���^�[�Q�b�g
	//);
	//cmdList_->ResourceBarrier(1, &depthBarrier);
}

bool Dx12Wrapper::CreateSSAOPipeLine()
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
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���_�V�F�[�_
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/boardVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_1",
		0,
		0, vsBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));
	plsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	// �s�N�Z���V�F�[�_
	ComPtr<ID3DBlob> psBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/SsaoPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"SsaoPS", "ps_5_1",
		0,
		0, psBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	plsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// ���X�^���C�U�ݒ�
	plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	// ���̑��ݒ�
	// �f�v�X�ƃX�e���V���ݒ�
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState.DepthEnable = false;
	//plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// �o�͐ݒ�
	plsDesc.NumRenderTargets = 1;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
	// �u�����h
	plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	plsDesc.pRootSignature = boardSig_.Get();
	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(ssaoPipeline_.ReleaseAndGetAddressOf()));

	assert(SUCCEEDED(result));
	return false;
}

ComPtr<ID3D12DescriptorHeap> Dx12Wrapper::CreateImguiDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	auto result = dev_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(imguiHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	return imguiHeap_;
}

void Dx12Wrapper::CreateFontDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	auto result = dev_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(fontDescHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

}

Dx12Wrapper::Dx12Wrapper()
{
	
}

Dx12Wrapper::~Dx12Wrapper()
{
}

bool Dx12Wrapper::Init(HWND hwnd)
{
	hwnd_ = hwnd;
	HRESULT result = S_OK;
#if _DEBUG
	ComPtr<ID3D12Debug3> debug = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf()));
	debug->EnableDebugLayer();
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgi_.ReleaseAndGetAddressOf()));
#else 
	result = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgi_));
#endif
	assert(SUCCEEDED(result));
	vector<IDXGIAdapter*>adapters;
	IDXGIAdapter* adapter = nullptr;
	for (int i = 0; dxgi_->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND;++i)
	{
		adapters.push_back(adapter);
	}
	for (auto& adpt : adapters)
	{
		DXGI_ADAPTER_DESC adptDesc = {};
		result = adpt->GetDesc(&adptDesc);
		assert(SUCCEEDED(result));
		std::wstring strDesc = adptDesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			adapter = adpt;
			break;
		}
		
	}
	CreateDevice(adapter);

	TexManager::GetInstance().SetDevice(dev_.Get());

	InitCommandSet();
	
	CreateSwapChain(hwnd);
	
	CreateFence();


	// �t�H���g�\��������
	gMemory_ = new GraphicsMemory(dev_.Get());
	auto rb = ResourceUploadBatch(dev_.Get());
	rb.Begin();
	RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
	SpriteBatchPipelineStateDescription sbpStateDesc(rtState);
	spriteBatch_ = new SpriteBatch(
		dev_.Get(),
		rb,
		sbpStateDesc);
	CreateFontDescriptorHeap();
	spriteFonts_ = new SpriteFont(dev_.Get(),
		rb,
		L"Resource/Font/meiryo.spritefont",
		fontDescHeap_->GetCPUDescriptorHandleForHeapStart(),
		fontDescHeap_->GetGPUDescriptorHandleForHeapStart());
	auto future = rb.End(cmdQue_.Get());
	cmdQue_->Signal(fence_.Get(), ++fenceValue_);
	// Execute�����܂ő҂���
	while (true)
	{
		if (fence_->GetCompletedValue() == fenceValue_)
		{
			break;
		}
	}
	future.wait();

	CreateImguiDescriptorHeap();
	if (ImGui::CreateContext() == nullptr)
	{
		assert(false);
	}

	if (!ImGui_ImplWin32_Init(hwnd))
	{
		assert(false);
	}
	if (!ImGui_ImplDX12_Init(dev_.Get(),
		3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		imguiHeap_.Get(),
		imguiHeap_->GetCPUDescriptorHandleForHeapStart(),
		imguiHeap_->GetGPUDescriptorHandleForHeapStart()))
	{
		assert(false);
	}
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	cameraCtr_ = make_unique<CameraCtr>();

	renderer_ = make_shared<Renderer>(*dev_.Get());
	//const char* modelPath = "Resource/PMD/���~�N/mikuXS���~�N.pmd";
	//const char* modelPath = "Resource/PMD/�_��/�_��Ver1.10SW.pmd";
	//const char* modelPath = "Resource/PMD/model/�����~�N.pmd";
	//const char* modelPath = "Resource/PMD/model/��������.pmd";
	//const char* modelPath = "Resource/PMD/model/�����~�Nmetal.pmd";
	//const char* modelPath = "Resource/PMD/model/�������J.pmd";
	//const char* modelPath = "Resource/PMD/��ߔe��v1.0/��ߔe��v1.pmd";
	const char* modelPath = "Resource/PMD/�Ö��n���Ƃ�/�Ö��n���Ƃ�152Normal.pmd";
	//const char* modelPath = "Resource/PMD/�얲/reimu_F02.pmd";
	pmdActor_.push_back(make_shared<PMDActor>(dev_, modelPath, XMFLOAT3(0,0,0), cameraCtr_->GetCamera()));
	//modelPath = "Resource/PMD/�Ö��n���Ƃ�/�Ö��n���Ƃ�152Normal.pmd";
	//pmdActor_.push_back(make_shared<PMDActor>(dev_, modelPath, XMFLOAT3(10,0,0)));
	

	// �����_�[�^�[�Q�b�g���쐬
	CreateRenderTargetDescriptorHeap();
	
	// �[�x�o�b�t�@�r���[�쐬
	CreateDepthBufferView();
	// �V���h�E�}�b�v�p�o�b�t�@��dsv�쐬
	CreateShadowMapBufferAndView();
	// �|���o�[�e�b�N�X�쐬
	CreateBoardPolyVerts();

	// ��{�e�N�X�`���쐬
	CreateDefaultTextures();

	// �䂪�ݗp�e�N�X�`���쐬
	//texManager_ = make_shared<TexManager>(*dev_.Get());
	TexManager::GetInstance().CreateTexture(L"Resource/image/NormalMap2.png", normalMapTex_);

	// 1�p�X�ڏ������݃o�b�t�@�ƑΉ�����RTV,
	// SRV�����
	CreateRenderTargetTexture();

	for (auto& actor : pmdActor_)
	{
		// �e�N�X�`���쐬
		actor->CreatePMDModelTexture();

		// �}�e���A���o�b�t�@�y�уr���[�̍쐬*
		actor->CreateMaterialBufferView();

		// �萔�o�b�t�@�쐬*�J�����s��𕪗�
		actor->CreateTransformBuffer();
		// �{�[���o�b�t�@�쐬*
		actor->CreateBoneBuffer();

		// ���W�ϊ�SRV�p�f�B�X�N���v�^�q�[�v�쐬*
		actor->CreateBasicDescriptors();

		// ���\�[�X�f�[�^���r���h
		actor->GetPMDResource().Build({ GroopType::TRANSFORM, GroopType::MATERIAL, GroopType::DEPTH });
	}

	// ���̃f�X�N���v�^�q�[�v
	CreatePrimitiveBufferView();
	// �r���[�|�[�g�ƃV�U�[��`������
	InitViewRect();
	
	// �|���p�C�v���C���쐬
	CreateBoardPipeline();
	// �e�̃p�C�v���C���쐬
	CreateShadowPipeline();
	// SSAO�p�C�v���C���쐬
	CreateSSAOPipeLine();

	primManager_ = make_shared<PrimitiveManager>(dev_);
	primManager_->CreatePlane(XMFLOAT3(0, 0, 0), 50, 50);
	//efcMng_ = make_shared<EffectManager>(dev_.Get(), cmdQue_.Get());

	return true;
}

void Dx12Wrapper::CreatePrimitiveBufferView()
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = static_cast<UINT>(1 + pmdActor_.size());
	descHeapDesc.NodeMask = 0;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	auto result = dev_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(primitiveDescHeap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	auto handle = primitiveDescHeap_->GetCPUDescriptorHandleForHeapStart();
	// ���W�ϊ��y�у��C�g����B�e�����[�x�̃e�N�X�`���̂��߂̃r���[
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	for (auto& actor : pmdActor_)
	{
		auto& transDesc = actor->GetPMDResource().GetGroops(GroopType::TRANSFORM).resources_[0];
		cbvDesc.BufferLocation = transDesc.resource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = static_cast<UINT>(transDesc.resource->GetDesc().Width);
		dev_->CreateConstantBufferView(&cbvDesc, handle);
		handle.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	dev_->CreateShaderResourceView(shadowDepthBuffer_.Get(), &srvDesc, handle);
}

bool Dx12Wrapper::InitViewRect()
{
	auto& app = Application::GetInstance();
	auto wSize = rtTextures_[0]->GetDesc();
	// �r���[�|�[�g	
	viewPort_.TopLeftX = 0;
	viewPort_.TopLeftY = 0;
	viewPort_.Width = wSize.Width;
	viewPort_.Height = wSize.Height;
	viewPort_.MaxDepth = 1.0f;
	viewPort_.MinDepth = 0.0f;

	// �V�U�[��`�̐ݒ�
	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = wSize.Width;
	scissorRect_.bottom = wSize.Height;

	return true;
}

bool Dx12Wrapper::Update()
{
	static auto lastTime = GetTickCount64();
	static float deltaTime = 0.0f;
	static int frame = 0;
	
	auto rtvIncSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeaps[6] = {};
	// 0:1�p�X�ڂ̐F
	rtvHeaps[0] = rtvHeaps[1] = rtvHeaps[2] = rtvHeaps[3] = rtvHeaps[4] = rtvHeaps[5] = firstRtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHeaps[1].ptr += rtvIncSize;		// 1:1�p�X�ڂ̖@��
	rtvHeaps[2].ptr += rtvIncSize * 2;	// 2:1�p�X�ڂ̍��P�x
	rtvHeaps[3].ptr += rtvIncSize * 3;	// 3:�k���o�b�t�@�̍��P�x
	rtvHeaps[4].ptr += rtvIncSize * 4;	// 4:�k���o�b�t�@�̐F
	rtvHeaps[5].ptr += rtvIncSize * 5;	// 5:SSAO�������ݐ�
	D3D12_CPU_DESCRIPTOR_HANDLE  dsvHeaps[] = { depthDescHeap_->GetCPUDescriptorHandleForHeapStart() };

	// �J��������
	//cameraCtr_->MoveCamera(XMFLOAT3(0, 1, 0));

	// pmd���f���A�b�v�f�[�g
	for (auto& actor : pmdActor_)
	{
		actor->Update(deltaTime);
		cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList_->IASetVertexBuffers(0, 1, &actor->GetVbView());
		cmdList_->IASetIndexBuffer(&actor->GetIbView());
		// �e
		DrawShadow(actor->GetBasicMarix());
		// ���f���`��
		CD3DX12_VIEWPORT vp(bbResouces[bbIdx_].Get());
		cmdList_->RSSetViewports(1, &viewPort_);
		cmdList_->RSSetScissorRects(1, &scissorRect_);
		cmdList_->SetGraphicsRootSignature(renderer_->GetRootSignature().Get());
		cmdList_->SetPipelineState(renderer_->GetPipelineState().Get());
		cmdList_->OMSetRenderTargets(3, rtvHeaps, false, dsvHeaps);
		actor->DrawModel(cmdList_);
	}
	cmdList_->OMSetRenderTargets(3, rtvHeaps, false, dsvHeaps);
	// ���\�[�X�o���A��ݒ�f�v�X����s�N�Z���V�F�[�_
	auto depthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		shadowDepthBuffer_.Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// �O�^�[�Q�b�g
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// ���^�[�Q�b�g
	);
	cmdList_->ResourceBarrier(1, &depthBarrier);
	primManager_->Draw(cmdList_.Get(), primitiveDescHeap_.Get());
	// ���\�[�X�o���A��ݒ�f�v�X����s�N�Z���V�F�[�_
	depthBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		shadowDepthBuffer_.Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �O�^�[�Q�b�g
		D3D12_RESOURCE_STATE_DEPTH_WRITE		// ���^�[�Q�b�g		
	);
	cmdList_->ResourceBarrier(1, &depthBarrier);
	
	// �G�t�F�N�g(effekseer)
	//efcMng_->Update(deltaTime * 60, cmdList_.Get());
	BYTE keyState[256] = {};
	static BYTE lastState[256] = {};
	auto result = GetKeyboardState(keyState);
	if (keyState[VK_SPACE] & 0x80 && !(lastState[VK_SPACE] & 0x80))
	{
		//efcMng_->PlayEffect("depthTest" , {0,10,0});
	}
	result = GetKeyboardState(lastState);
	frame++;

	// �|���X�V
	mappedBoardBuffer_->time += deltaTime;
	//mappedBoardBuffer_->time = fmodf(mappedBoardBuffer_->time, 2);
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		POINT point = {};
		GetCursorPos(&point);
		ScreenToClient(hwnd_, &point);
		mappedBoardBuffer_->pos.x = point.x;
		mappedBoardBuffer_->pos.y = point.y;
		auto wSize = Application::GetInstance().GetWindowSize();
		mappedBoardBuffer_->pos.x /= wSize.width;
		mappedBoardBuffer_->pos.y /= wSize.height;
	}

	// ���\�[�X�o���A��ݒ背���_�[�^�[�Q�b�g����V�F�[�_
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		rtTextures_[0].Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_RENDER_TARGET,	// �O�^�[�Q�b�g
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// ���^�[�Q�b�g
	);
	cmdList_->ResourceBarrier(1, &barrier);

	// �k���o�b�t�@�ւ̏�������
	rtvHeaps[0] = rtvHeaps[3];	
	rtvHeaps[1] = rtvHeaps[4];
	cmdList_->OMSetRenderTargets(2, rtvHeaps, false, nullptr);
	cmdList_->SetPipelineState(shrinkPipeLine_.Get());
	cmdList_->SetGraphicsRootSignature(boardSig_.Get());
	ID3D12DescriptorHeap* deskHeaps[] = { firstSrvHeap_.Get() };
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, firstSrvHeap_->GetGPUDescriptorHandleForHeapStart());
	cmdList_->IASetVertexBuffers(0, 1, &boardVBView_);
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �k���o�b�t�@��������
	auto svp = CD3DX12_VIEWPORT(bbResouces[bbIdx_].Get());
	auto shrc = scissorRect_;
	for (int i = 0; i < 8; ++i)
	{
		svp.Width /= 2.0f;
		svp.Height /= 2.0f;
		cmdList_->RSSetViewports(1, &svp);
		svp.TopLeftY += svp.Height;

		shrc.right = shrc.left + svp.Width;
		shrc.bottom = shrc.top + svp.Height;
		cmdList_->RSSetScissorRects(1, &shrc);
		shrc.top = svp.TopLeftY;

		cmdList_->DrawInstanced(4, 1, 0, 0);
	}

	// ���\�[�X�o���A��ݒ�V�F�[�_���烌���_�[�^�[�Q�b�g
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		rtShrinkForBloom_.Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_RENDER_TARGET,			// �O��^�[�Q�b�g
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// ���^�[�Q�b�g			
	);
	cmdList_->ResourceBarrier(1, &barrier);

	cmdList_->RSSetViewports(1, &viewPort_);
	cmdList_->RSSetScissorRects(1, &scissorRect_);
	// SSAO�ւ̏�������
	rtvHeaps[0] = rtvHeaps[5];
	cmdList_->OMSetRenderTargets(1, rtvHeaps, false, nullptr);
	cmdList_->SetPipelineState(ssaoPipeline_.Get());
	cmdList_->SetGraphicsRootSignature(boardSig_.Get());
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, firstSrvHeap_->GetGPUDescriptorHandleForHeapStart());
	cmdList_->IASetVertexBuffers(0, 1, &boardVBView_);
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList_->DrawInstanced(4, 1, 0, 0);
	// ���\�[�X�o���A��ݒ�V�F�[�_���烌���_�[�^�[�Q�b�g
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		ssaoBuffer_.Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_RENDER_TARGET,			// �O��^�[�Q�b�g
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// ���^�[�Q�b�g			
	);
	cmdList_->ResourceBarrier(1, &barrier);

	// -�|���Ƀo�b�N�o�b�t�@����������

	auto bbRtvHeap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	bbRtvHeap.ptr += static_cast<SIZE_T>(bbIdx_) * rtvIncSize;
	cmdList_->OMSetRenderTargets(1, &bbRtvHeap, false, nullptr);
	// �|���`��
	cmdList_->SetPipelineState(boardPipeLine_.Get());
	cmdList_->SetGraphicsRootSignature(boardSig_.Get());
	//ID3D12DescriptorHeap* deskHeaps[] = { firstSrvHeap_.Get() };
	cmdList_->SetDescriptorHeaps(1, deskHeaps);
	cmdList_->SetGraphicsRootDescriptorTable(0, firstSrvHeap_->GetGPUDescriptorHandleForHeapStart());
	cmdList_->IASetVertexBuffers(0, 1, &boardVBView_);
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList_->DrawInstanced(4, 1, 0, 0);
	

	// ���\�[�X�o���A��ݒ�V�F�[�_���烌���_�[�^�[�Q�b�g
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		rtTextures_[0].Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �O�^�[�Q�b�g		
		D3D12_RESOURCE_STATE_RENDER_TARGET// ���^�[�Q�b�g
	);

	// font
	ID3D12DescriptorHeap* fontHeaps[] = { fontDescHeap_.Get() };
	cmdList_->SetDescriptorHeaps(1, fontHeaps);
	spriteBatch_->SetViewport(viewPort_);
	spriteBatch_->Begin(cmdList_.Get());
	
	static wstring wString = L"�n���[���[���h";
	spriteFonts_->DrawString(spriteBatch_, wString.c_str(), XMFLOAT2(600, 100), DirectX::Colors::Red);
	spriteBatch_->End();

	// imgui
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("RenderingtestMenu");

	static ImVec2 imSize = ImVec2(100, 100);
	imSize = ImGui::GetWindowSize();
	ImGui::SetWindowSize(imSize);
	//static char string[] = "new Text";
	//ImGui::InputText("string", string, 256);
	//wString = StrOperater::GetWideStringfromString(string);
	static float lightFloatVal; 
	auto& mat = pmdActor_[0]->GetBasicMarix();
	if(ImGui::TreeNode("Light Pos"))
	{
		ImGui::SliderFloat("pos X", &mat.lightPos.m128_f32[0], -1.0f, 1.0f, "%f");
		ImGui::SliderFloat("pos Y", &mat.lightPos.m128_f32[1], -1.0f, 1.0f, "%f");
		ImGui::SliderFloat("pos Z", &mat.lightPos.m128_f32[2], -1.0f, 1.0f, "%f");
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Camera"))
	{
		ImGui::SliderFloat("FieldOfView", &fov_, 30.0f, 150.0f, "%f");
		ImGui::SliderFloat("Near", &cNear_, 0.1f, 30.0f, "%f");
		ImGui::SliderFloat("Far", &cFar_, 100.0f, 1000.0f, "%f");
		ImGui::TreePop();
	}
	auto& instID = pmdActor_[0]->GetInstID();
	if (ImGui::TreeNode("InstID"))
	{
		ImGui::SliderInt("X", &instID.x, 1, 5, "%d");
		ImGui::SliderInt("Z", &instID.y, 1, 5, "%d");
		ImGui::TreePop();
	}	
	ImGui::Checkbox("Shadow", &mat.isShadowing);
	if (ImGui::TreeNode("PostEffect"))
	{
		bool ssao = mappedBoardBuffer_->ssaoActive;
		bool bloom = mappedBoardBuffer_->bloomActive;
		bool dof = mappedBoardBuffer_->dofActive;
		bool nOutline = mappedBoardBuffer_->outLineN;
		bool dOutline = mappedBoardBuffer_->outLineD;
		ImGui::Checkbox("Ssao", &ssao);
		ImGui::Checkbox("Bloom", &bloom);
		ImGui::Checkbox("Dof", &dof);
		ImGui::Checkbox("OutLineN", &nOutline);
		ImGui::Checkbox("OutLineD", &dOutline);
		mappedBoardBuffer_->ssaoActive = ssao;
		mappedBoardBuffer_->bloomActive = bloom;
		mappedBoardBuffer_->dofActive = dof;
		mappedBoardBuffer_->outLineN = nOutline;
		mappedBoardBuffer_->outLineD = dOutline;


		ImGui::ColorPicker3("BloomColor", mappedBoardBuffer_->bloomCol, ImGuiColorEditFlags_PickerHueWheel);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Disolve"))
	{
		ImGui::SliderFloat("Top", &mat.disolveTop, 0.0f, 20.0f, "%f");
		ImGui::SliderFloat("Bottom", &mat.disolveBottom, -2.0f, 20.0f, "%f");
		ImGui::TreePop();
	}

	ImGui::End();
	ImGui::Render();

	cmdList_->SetDescriptorHeaps(1, imguiHeap_.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList_.Get());

	const auto wSize = Application::GetInstance().GetWindowSize();
	cameraCtr_->GetCamera().GetCameaProj() = XMMatrixPerspectiveFovRH(XMConvertToRadians(fov_), // ��p(FOV)
		static_cast<float>(wSize.width) / static_cast<float>(wSize.height),
		cNear_,	// �j�A(�߂�)
		cFar_);	//�@�t�@�[(����)

	cmdList_->ResourceBarrier(1, &barrier);
	// ���\�[�X�o���A��ݒ�V�F�[�_���烌���_�[�^�[�Q�b�g
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		rtShrinkForBloom_.Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �O�^�[�Q�b�g		
		D3D12_RESOURCE_STATE_RENDER_TARGET// ���^�[�Q�b�g
	);
	cmdList_->ResourceBarrier(1, &barrier);
	// ���\�[�X�o���A��ݒ�V�F�[�_���烌���_�[�^�[�Q�b�g
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		ssaoBuffer_.Get(),	// ���\�[�X
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �O�^�[�Q�b�g		
		D3D12_RESOURCE_STATE_RENDER_TARGET// ���^�[�Q�b�g
	);
	cmdList_->ResourceBarrier(1, &barrier);

	auto nowTime = GetTickCount64();
	deltaTime = static_cast<float>(nowTime - oldTime) / 1000;
	oldTime = static_cast<float>(nowTime);
	
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
	swapchain_->Present(0, 0);
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

	// �o�b�N�o�b�t�@�ɃZ�b�g	
	auto rtvIncSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeaps[6] = {};
	// 0:1�p�X�ڂ̐F
	rtvHeaps[0] = rtvHeaps[1] = rtvHeaps[2] = rtvHeaps[3] = rtvHeaps[4] = rtvHeaps[5] = firstRtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHeaps[1].ptr += rtvIncSize;		// 1:1�p�X�ڂ̖@��
	rtvHeaps[2].ptr += rtvIncSize * 2;	// 2:1�p�X�ڂ̍��P�x
	rtvHeaps[3].ptr += rtvIncSize * 3;	// 3:�k���o�b�t�@�̍��P�x
	rtvHeaps[4].ptr += rtvIncSize * 4;	// 4:�k���o�b�t�@�̐F
	rtvHeaps[5].ptr += rtvIncSize * 5;	// 5:SSAO�������ݐ�
	D3D12_CPU_DESCRIPTOR_HANDLE  dsvHeaps[] = { depthDescHeap_->GetCPUDescriptorHandleForHeapStart() };
	cmdList_->OMSetRenderTargets(2, rtvHeaps, false, dsvHeaps);
	// ��ʂ��N���A(�F�ς���)
	for (auto rtv : rtvHeaps)
	{
		cmdList_->ClearRenderTargetView(rtv, clearCol, 0, nullptr);
	}
	cmdList_->ClearDepthStencilView(
		depthDescHeap_->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);

	// �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	CD3DX12_VIEWPORT vp(bbResouces[bbIdx_].Get());	// ����łł��邪�����ł��Ȃ�
	cmdList_->RSSetViewports(1, &viewPort_);
	cmdList_->RSSetScissorRects(1, &scissorRect_);
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


bool Dx12Wrapper::CreateDevice(IDXGIAdapter* adapter)
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
		auto result = D3D12CreateDevice(adapter, lv, IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf()));
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
	scDesc.Width = static_cast<UINT>(wSize.width );
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
	result = dev_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(rtvHeap_.ReleaseAndGetAddressOf()));
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
	rtvDesc.Texture2D.PlaneSlice = 0;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	auto heap = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	const auto incSize = dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (int i = 0; i < num_rtvs; i++)
	{
		swapchain_->GetBuffer(i, IID_PPV_ARGS(bbResouces[i].ReleaseAndGetAddressOf()));//�u�L�����o�X�v���擾
		dev_->CreateRenderTargetView(bbResouces[i].Get(), &rtvDesc, heap);	// �L�����p�X�ƐE�l��R�Â���
		heap.ptr += incSize;// �E�l�ƃL�����o�X�̃y�A�̂Ԃ񎟂̂Ƃ���܂ŃI�t�Z�b�g
	}

	return SUCCEEDED(result);
}
