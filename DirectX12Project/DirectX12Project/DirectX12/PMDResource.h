#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <memory>
#include <string>
#include <d3dx12.h>
#include <array>

using Microsoft::WRL::ComPtr;

// 読み込むバッファのグループ
enum class GroopType
{
	TRANSFORM,	// 座標変換(CBV, CBV)
	MATERIAL,	// マテリアル(CBV, SRV, SRV, SRV, SRV)
	MAX
};

/// <summary>
/// バッファタイプ
/// </summary>
enum class BuffType
{
	CBV,	// 定数バッファ
	SRV,	// シェーダリソース(マテリアル)
	MAX
};
struct PMDResourceBinding
{
	std::vector<BuffType>types_;	// リソースの順番
	struct Resource
	{
		ID3D12Resource* resource;
		int size;
	};
	std::vector<Resource> resources_;
	ComPtr<ID3D12DescriptorHeap>descHeap_;
	int strideBytes_;

	/// <summary>
	/// 格納するバッファのタイプをセットする
	/// </summary>
	/// <param name="types">バッファタイプ</param>
	void Init(std::vector<BuffType>types);

	/// <summary>
	/// リソースバッファを追加
	/// </summary>
	/// <param name="res">リソースバッファ</param>
	/// <param name="size">バッファの一つのサイズ</param>
	void AddBuffers(ID3D12Resource* res, int size = 0);
};
using PMDResources = std::array<PMDResourceBinding, static_cast<int>(GroopType::MAX)>;
class PMDResource
{
public:
	PMDResource(ID3D12Device* dev);

	/// <summary>
	/// バインドしたリソースから色々作成
	/// </summary>
	/// <param name="groopTypes">バッファのグループ</param>
	void Build(const std::vector<GroopType> groopTypes);

	PMDResourceBinding& GetGroops(GroopType groopType);
	void SetPMDState(ID3D12GraphicsCommandList& cmdList);
	ComPtr<ID3D12RootSignature> GetRootSignature();
	ComPtr<ID3D12PipelineState> GetPipelineState();
private:
	/// <summary>
	/// ルートシグネチャ生成
	/// </summary>
	/// <param name="plsDesc">パイプラインステートデスク</param>
	void CreateRootSignature();

	/// <summary>
	/// パイプラインステートオブジェクト作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreatePipelineState();

	/// <summary>
	/// バインドしたバッファからリソースビューを作成
	/// </summary>
	/// <param name="groopType">グループタイプ</param>
	void CreateResouses(const std::vector<GroopType>& groopType);

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSig_ = nullptr;
	PMDResources res_;
	ComPtr<ID3D12Device> dev_;
};

