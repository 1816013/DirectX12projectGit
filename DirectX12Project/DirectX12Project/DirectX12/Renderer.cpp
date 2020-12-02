#include "Renderer.h"
#include <d3dcompiler.h>
#include <cassert>
#include <Windows.h>
#include <string>
#include "../Common.h"

Renderer::Renderer(ID3D12Device& dev)
{
	CreateRootSignature(dev);
	CreatePipelineState(dev);
}

ComPtr<ID3D12RootSignature> Renderer::GetRootSignature()
{
	return rootSig_;
}

ComPtr<ID3D12PipelineState> Renderer::GetPipelineState()
{
	return pipelineState_;
}

void Renderer::CreateRootSignature(ID3D12Device& dev)
{
	HRESULT result = S_OK;
	// ���[�g�V�O�l�`��
	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};

	D3D12_ROOT_PARAMETER rp[2] = {};
	D3D12_DESCRIPTOR_RANGE range[4] = {};

	// �����W
	// �s��萔�o�b�t�@
	range[0] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // �����W�^�C�v b
		1,// �f�X�N���v�^��	b0�`b0�܂�
		0);// �x�[�X���W�X�^�ԍ� b0	

	// �{�[��
	range[1] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // �����W�^�C�v b
		1,// �f�X�N���v�^��	b1�`b1�܂�
		1);// �x�[�X���W�X�^�ԍ� b1

	// �}�e���A��
	range[2] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // �����W�^�C�v b
		1,// �f�X�N���v�^��	b2�`b2�܂�
		2);// �x�[�X���W�X�^�ԍ� b2

	// �e�N�X�`��
	range[3] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // �����W�^�C�v t
		4,// �f�X�N���v�^��	t0�`t3�܂�
		0);// �x�[�X���W�X�^�ԍ� t0

	// ���[�g�p�����[�^
	// ���W�ϊ�
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[0],	// ���[�g�p�����[�^
		2,		// �����W��
		&range[0]);	// �����W�擪�A�h���X
	// �}�e���A��&�e�N�X�`��
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[1],	// ���[�g�p�����[�^
		2,	// �����W��
		&range[2],	// �����W�擪�A�h���X
		D3D12_SHADER_VISIBILITY_PIXEL);	// �ǂ̃V�F�[�_�Ŏg����

	// s���`
	// �T���v���̒�`�A�T���v����uv��0������1�z���Ƃ��̎���
	// UV�����ƂɐF���Ƃ��Ă���Ƃ��̃��[�����w�肷�����
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	samplerDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	samplerDesc[1] = CD3DX12_STATIC_SAMPLER_DESC(1);
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	rsDesc.Init(_countof(rp), rp, _countof(samplerDesc), samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// �V�O�l�`���ݒ�
	ComPtr<ID3DBlob> errBlob = nullptr;
	ComPtr<ID3DBlob> sigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sigBlob,
		&errBlob);
	Common::OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	// ���[�g�V�O�l�`���̐���
	result = dev.CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSig_));
	assert(SUCCEEDED(result));
}

bool Renderer::CreatePipelineState(ID3D12Device& dev)
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
	result = D3DCompileFromFile(L"Shader/vs.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_1",
		0,
		0, vsBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	Common::OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));
	plsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());

	// �s�N�Z���V�F�[�_
	ComPtr<ID3DBlob> psBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/ps.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_1",
		0,
		0, psBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	Common::OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	plsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// ���X�^���C�U�ݒ�
	plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// ���̑��ݒ�
	// �f�v�X�ƃX�e���V���ݒ�
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// �o�͐ݒ�
	plsDesc.NumRenderTargets = 1;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �u�����h
	plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// ���[�g�V�O�l�`������
	plsDesc.pRootSignature = rootSig_.Get();
	result = dev.CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	return true;
}
