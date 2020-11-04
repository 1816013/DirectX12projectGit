#pragma once
#include <DirectXMath.h>
#include <vector>
#include <cstdint>
#include <string>
struct  PMDVertex
{
	DirectX::XMFLOAT3 pos;		// 座標
	DirectX::XMFLOAT3 normal;	// 法線
	DirectX::XMFLOAT2 uv;		// uv

};

struct PMDMaterial
{
	DirectX::XMFLOAT3 diffuse;	// ディフューズ色
	float alpha;
	DirectX::XMFLOAT3 speqular;	// スペキュラ色
	float speqularity;
	DirectX::XMFLOAT3 ambient;	// アンビエント色
	uint32_t indexNum;		// マテリアルのインデックス数
};
class PMDModel
{
public:
	/// <summary>
	/// pmdファイル読み込み
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	bool Load(const char* path);
	const std::vector<PMDVertex>& GetVertexData()const;
	const std::vector<uint16_t>& GetIndexData()const;
	const std::vector<PMDMaterial>& GetMaterialData()const;
	const std::vector<std::string>& GetTexturePaths()const;

private:
	std::string GetTextureFromModelAndTexPath(const std::string& modelPath,
		const std::string&);

	std::vector<PMDVertex>vertices_;
	std::vector<uint16_t>indices_;
	std::vector<PMDMaterial>materials_;
	std::vector<std::string>texturePaths_;
	
};


