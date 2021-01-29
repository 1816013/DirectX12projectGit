#include "PrimitiveManager.h"
#include <d3dcompiler.h>
#include "../../Common.h"
#include <cassert>
#include "PlaneMesh.h"

using namespace std;

PrimitiveManager::PrimitiveManager(ComPtr<ID3D12Device> dev)
{
	dev_ = dev;
	CreatePipeline();
}

void PrimitiveManager::CreatePipeline()
{
	HRESULT result = S_OK;
	// パイプラインステートデスク
	D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	// IA(InputAssembler)
	// 入力レイアウト
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		// 頂点情報
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// 頂点シェーダ
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/PrimitiveVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PrimitiveVS", "vs_5_1",
		0,
		0, vsBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	Common::OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));
	plsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	// ピクセルシェーダ
	ComPtr<ID3DBlob> psBlob = nullptr;
	result = D3DCompileFromFile(L"Shader/PrimitivePS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PrimitivePS", "ps_5_1",
		0,
		0, psBlob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
	Common::OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	plsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// ラスタライザ設定
	plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	// その他設定
	// デプスとステンシル設定
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//plsDesc.DepthStencilState.DepthEnable = false;
	plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 出力設定
	plsDesc.NumRenderTargets = 3;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	plsDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	plsDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ブレンド
	plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	CreateRootSignature();
	plsDesc.pRootSignature = rootSig_.Get();
	result = dev_->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(pipeline_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void PrimitiveManager::CreateRootSignature()
{
	// ルートシグネチャ生成
	// ルートシグネチャ
	D3D12_ROOT_PARAMETER rp[1] = {};
	D3D12_DESCRIPTOR_RANGE range[2] = {};

	// レンジ
	// 行列定数バッファ
	range[0] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // レンジタイプ d
		1,// デスクリプタ数	d0～d0まで
		0);// ベースレジスタ番号 d0	
	range[1] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // レンジタイプ t
		1,// デスクリプタ数	t0～t0まで
		0);// ベースレジスタ番号 t0	

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[0],	// ルートパラメータ
		2,		// レンジ数
		&range[0],// レンジ先頭アドレス
		D3D12_SHADER_VISIBILITY_ALL);

	D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
	samplerDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	//samplerDesc[0].Filter = D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc(1, rp, 1, samplerDesc);
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// シグネチャ設定
	ComPtr<ID3DBlob> errBlob = nullptr;
	ComPtr<ID3DBlob> sigBlob = nullptr;
	auto result = D3D12SerializeRootSignature(&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sigBlob,
		&errBlob);
	Common::OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	// ルートシグネチャの生成
	result = dev_->CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSig_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

PrimitiveManager::~PrimitiveManager()
{
}

void PrimitiveManager::CreatePlane(const DirectX::XMFLOAT3& pos, float width, float depth)
{
	primitives_.push_back(make_shared<PlaneMesh>(dev_.Get(), pos, width, depth));
}

void PrimitiveManager::Draw(ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* descHeap)
{
	cmdList->SetPipelineState(pipeline_.Get());
	cmdList->SetGraphicsRootSignature(rootSig_.Get());
	cmdList->SetDescriptorHeaps(1, &descHeap);
	cmdList->SetGraphicsRootDescriptorTable(0, descHeap->GetGPUDescriptorHandleForHeapStart());
	
	for (auto prim : primitives_)
	{
		prim->Draw(cmdList);
	}
}
