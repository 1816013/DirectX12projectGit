#pragma once
#include <d3d12.h>
#include <d3dx12.h>

using Microsoft::WRL::ComPtr;
class Renderer
{
public:
	Renderer(ID3D12Device& dev);
	ComPtr<ID3D12RootSignature> GetRootSignature();
	ComPtr<ID3D12PipelineState> GetPipelineState();
private:
	/// <summary>
	/// ���[�g�V�O�l�`������
	/// </summary>
	/// <param name="plsDesc">�p�C�v���C���X�e�[�g�f�X�N</param>
	void CreateRootSignature(ID3D12Device& dev);

	/// <summary>
	/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�쐬
	/// </summary>
	/// <returns>true:���� false:���s</returns>
	bool CreatePipelineState(ID3D12Device& dev);

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSig_ = nullptr;
};

