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
	/// ルートシグネチャ生成
	/// </summary>
	/// <param name="plsDesc">パイプラインステートデスク</param>
	void CreateRootSignature(ID3D12Device& dev);

	/// <summary>
	/// パイプラインステートオブジェクト作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreatePipelineState(ID3D12Device& dev);

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSig_ = nullptr;
};

