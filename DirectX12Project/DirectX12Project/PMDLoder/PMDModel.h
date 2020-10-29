#pragma once
#include <DirectXMath.h>
#include <vector>
#include <cstdint>
struct  PMDVertex
{
	DirectX::XMFLOAT3 pos;		// 座標
	DirectX::XMFLOAT3 normal;	// 法線
	DirectX::XMFLOAT2 uv;		// uv

};

struct PMDMaterial
{
	DirectX::XMFLOAT3 diffuse;	// ディフューズ色
	uint32_t indexNum;		// マテリアルのインデックス数
};
class PMDModel
{
public:
	bool Load(const char* path);
	const std::vector<PMDVertex>& GetVertexData()const;
	const std::vector<uint16_t>& GetIndexData()const;
	const std::vector<PMDMaterial>& GetMaterialData()const;

private:
	std::vector<PMDVertex>vertices_;
	std::vector<uint16_t>indices_;
	std::vector<PMDMaterial>materials_;
	
};


