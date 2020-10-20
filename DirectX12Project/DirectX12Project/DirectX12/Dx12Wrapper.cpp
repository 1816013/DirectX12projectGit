#include "Dx12Wrapper.h"
#include <Windows.h>
#include <cassert>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <string>
#include "../Application.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;

XMFLOAT3 vertices_[3];

void Dx12Wrapper::CreateVertices()
{
	vertices_[0] = { -0.7, -0.7f, 0.0f };
	vertices_[1] = { 0.0f, 0.7f, 0.0f };
	vertices_[2] = { 0.7f, -0.7f, 0.0f };
}

void Dx12Wrapper::CreateVertexBaffer()
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���Ȃ��Ă��悢
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.VisibleNodeMask = 0;
	heapProp.CreationNodeMask = 0;
	// �����܂�

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeof(vertices_);	// ���_���̃T�C�Y
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
	XMFLOAT3* mappedData = nullptr;
	result = vertexBuffer_->Map(0, nullptr, (void**)&mappedData);
	assert(SUCCEEDED(result));
	std::copy(std::begin(vertices_), std::end(vertices_), mappedData);
	vertexBuffer_->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.StrideInBytes = sizeof(vertices_[0]);
	vbView_.SizeInBytes = sizeof(vertices_);
}

bool Dx12Wrapper::CreatePipelineState()
{
	HRESULT result = S_OK;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	// IA(InputAssembler)
	// ���̓��C�A�E�g
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���_�V�F�[�_
	ID3DBlob* errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/vs.hlsl", nullptr, 
		nullptr, "VS", "vs_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &vertexShader_, &errBlob);
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));	
	plsDesc.VS.pShaderBytecode = vertexShader_->GetBufferPointer();
	plsDesc.VS.BytecodeLength = vertexShader_->GetBufferSize();

	// ���X�^���C�U�ݒ�
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	plsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	// �s�N�Z���V�F�[�_
	result = D3DCompileFromFile(L"Shader/ps.hlsl", nullptr, 
		nullptr, "PS", "ps_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &pixelShader_, &errBlob);
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));

	plsDesc.PS.pShaderBytecode = pixelShader_->GetBufferPointer();
	plsDesc.PS.BytecodeLength = pixelShader_->GetBufferSize();

	// ���̑��ݒ�
	// �f�v�X�ƃX�e���V���ݒ�
	plsDesc.DepthStencilState.DepthEnable = false;
	plsDesc.DepthStencilState.StencilEnable = false;

	plsDesc.NumRenderTargets = 1;
	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = 0xffffffff;

	// �o�͐ݒ�
	// �u�����h
	plsDesc.BlendState.AlphaToCoverageEnable = false;
	plsDesc.BlendState.IndependentBlendEnable = false;
	plsDesc.BlendState.RenderTarget[0].BlendEnable = false;
	plsDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// ���[�g�V�O�l�`��
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.DescriptorTable.NumDescriptorRanges = 1;

	D3D12_ROOT_SIGNATURE_DESC rsd = {};
	rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsd.NumParameters = 0;
	rsd.NumStaticSamplers = 0;

	// �V�O�l�`���ݒ�
	ID3DBlob* sig = nullptr;
	D3D12SerializeRootSignature(&rsd,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sig,
		&errBlob);

	// ���[�g�V�O�l�`���̐���
	result = dev_->CreateRootSignature(0,
		sig->GetBufferPointer(),
		sig->GetBufferSize(),
		IID_PPV_ARGS(&rootSig_));
	OutputFromErrorBlob(errBlob);
	assert(SUCCEEDED(result));
	plsDesc.pRootSignature = rootSig_;

	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
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
		result = D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&dev_));
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

	// ���_�o�b�t�@���쐬
	CreateVertices();
	CreateVertexBaffer();

	if (!CreatePipelineState())
	{
		return false;
	}

	//// �V�F�[�_�[�ǂݍ���
	//result = D3DCompileFromFile(L"Shader/vs.hlsl", nullptr, nullptr, "VS", "vs_5_1",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
	//	0, &vertexShader_, nullptr);
	//assert(SUCCEEDED(result));

	//result = D3DCompileFromFile(L"Shader/ps.hlsl", nullptr, nullptr, "PS", "ps_5_1",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
	//	0, &pixelShader_, nullptr);
	//assert(SUCCEEDED(result));
	//
	//// ���[�g�V�O�l�N�`��

	//D3D12_ROOT_SIGNATURE_DESC rsd = {};
	//rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//// �V�O�l�`���ݒ�
	//D3D12SerializeRootSignature(&rsd,
	//	D3D_ROOT_SIGNATURE_VERSION_1,
	//	&signature_,
	//	&error_);

	//// ���[�g�V�O�l�`���̐���
	//result = dev_->CreateRootSignature(0,
	//	signature_->GetBufferPointer(),
	//	signature_->GetBufferSize(),
	//	IID_PPV_ARGS(&rootSig_));
	//assert(SUCCEEDED(result));

	//// ���_���C�A�E�g
	//D3D12_INPUT_ELEMENT_DESC layout[] = {
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	//};

	//// �p�C�v���C���X�e�[�g�I�u�W�F�N�g����
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	//// -���[�g�V�O�l�N�`���ƒ��_���C�A�E�g
	//plsDesc.pRootSignature = rootSig_;
	//plsDesc.InputLayout.pInputElementDescs = layout;
	//plsDesc.InputLayout.NumElements = _countof(layout);
	//// -�V�F�[�_�n
	//plsDesc.VS.pShaderBytecode = vertexShader_->GetBufferPointer();
	//plsDesc.VS.BytecodeLength = vertexShader_->GetBufferSize();
	//plsDesc.PS.pShaderBytecode = pixelShader_->GetBufferPointer();
	//plsDesc.PS.BytecodeLength = pixelShader_->GetBufferSize();
	//// --�g��Ȃ�
	////plsDesc.HS;
	////plsDesc.DS;
	////plsDesc.GS;
	//plsDesc.NumRenderTargets = 1;
	//plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	//// �[�x�X�e���V��
	//plsDesc.DepthStencilState.DepthEnable = false;
	//plsDesc.DepthStencilState.StencilEnable = false;
	//plsDesc.DSVFormat;

	//// ���X�^���C�U
	//plsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	//plsDesc.RasterizerState.FrontCounterClockwise = false;
	//plsDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	//plsDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	//plsDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	//plsDesc.RasterizerState.DepthClipEnable = true;
	//plsDesc.RasterizerState.MultisampleEnable = false;
	//plsDesc.RasterizerState.AntialiasedLineEnable = false;
	//plsDesc.RasterizerState.ForcedSampleCount = 0;
	//plsDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//// ���̑�
	//plsDesc.BlendState.AlphaToCoverageEnable = false;
	//plsDesc.BlendState.IndependentBlendEnable = false;
	//plsDesc.BlendState.RenderTarget[0].BlendEnable = false;
	//plsDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	//plsDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	//plsDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//plsDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	//plsDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//plsDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//plsDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	//plsDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//plsDesc.NodeMask = 0;
	//plsDesc.SampleDesc.Count = 1;
	//plsDesc.SampleDesc.Quality = 0;
	//plsDesc.SampleMask = 0xffffffff;
	////plsDesc.Flags;// �f�t�H���gOK
	//plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; //�O�p�`	
	//result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(&pipelineState_));
	//assert(SUCCEEDED(result));

	// �r���[�|�[�g
	viewPort_.TopLeftX = 0;
	viewPort_.TopLeftY = 0;
	viewPort_.Width = wSize.width;
	viewPort_.Height = wSize.height;
	viewPort_.MaxDepth = 1.0f;
	viewPort_.MinDepth = 0.0f;
	
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
	float clsCol[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	cmdList_->ClearRenderTargetView(rtvHeap, clsCol, 0, nullptr);

	cmdList_->SetGraphicsRootSignature(rootSig_);
	// �r���[�|�[�g�ƃV�U�[��`�̐ݒ�
	cmdList_->RSSetViewports(1, &viewPort_);
	cmdList_->RSSetScissorRects(1, &scissorRect_);
	// �O�p�`�`��
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	cmdList_->DrawInstanced(3, 1, 0, 0);

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
	swapchain_->GetDesc1(&swDesc);
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
