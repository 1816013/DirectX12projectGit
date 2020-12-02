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
	// ルートシグネチャ
	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};

	D3D12_ROOT_PARAMETER rp[2] = {};
	D3D12_DESCRIPTOR_RANGE range[4] = {};

	// レンジ
	// 行列定数バッファ
	range[0] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // レンジタイプ b
		1,// デスクリプタ数	b0～b0まで
		0);// ベースレジスタ番号 b0	

	// ボーン
	range[1] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // レンジタイプ b
		1,// デスクリプタ数	b1～b1まで
		1);// ベースレジスタ番号 b1

	// マテリアル
	range[2] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV, // レンジタイプ b
		1,// デスクリプタ数	b2～b2まで
		2);// ベースレジスタ番号 b2

	// テクスチャ
	range[3] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV, // レンジタイプ t
		4,// デスクリプタ数	t0～t3まで
		0);// ベースレジスタ番号 t0

	// ルートパラメータ
	// 座標変換
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[0],	// ルートパラメータ
		2,		// レンジ数
		&range[0]);	// レンジ先頭アドレス
	// マテリアル&テクスチャ
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rp[1],	// ルートパラメータ
		2,	// レンジ数
		&range[2],	// レンジ先頭アドレス
		D3D12_SHADER_VISIBILITY_PIXEL);	// どのシェーダで使うか

	// sを定義
	// サンプラの定義、サンプラはuvが0未満や1越えとかの時や
	// UVをもとに色をとってくるときのルールを指定するもの
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	samplerDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	samplerDesc[1] = CD3DX12_STATIC_SAMPLER_DESC(1);
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	rsDesc.Init(_countof(rp), rp, _countof(samplerDesc), samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// シグネチャ設定
	ComPtr<ID3DBlob> errBlob = nullptr;
	ComPtr<ID3DBlob> sigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sigBlob,
		&errBlob);
	Common::OutputFromErrorBlob(errBlob.Get());
	assert(SUCCEEDED(result));

	// ルートシグネチャの生成
	result = dev.CreateRootSignature(0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSig_));
	assert(SUCCEEDED(result));
}

bool Renderer::CreatePipelineState(ID3D12Device& dev)
{
	HRESULT result = S_OK;
	// パイプラインステートデスク
	D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc = {};
	// IA(InputAssembler)
	// 入力レイアウト
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		// 頂点情報
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// 頂点情報
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// UV情報
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		// ボーン番号 2バイト整数型
		{"BONE_NO", 0, DXGI_FORMAT_R16G16_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		//  2バイト整数型
		{"WEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	plsDesc.InputLayout.pInputElementDescs = layout;
	plsDesc.InputLayout.NumElements = _countof(layout);

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// 頂点シェーダ
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

	// ピクセルシェーダ
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

	// ラスタライザ設定
	plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// その他設定
	// デプスとステンシル設定
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	plsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	plsDesc.NodeMask = 0;
	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;
	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 出力設定
	plsDesc.NumRenderTargets = 1;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ブレンド
	plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// ルートシグネチャ生成
	plsDesc.pRootSignature = rootSig_.Get();
	result = dev.CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	return true;
}
