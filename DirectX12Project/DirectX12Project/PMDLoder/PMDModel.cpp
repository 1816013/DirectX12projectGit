#include "PMDModel.h"
#include <Windows.h>
#include <stdio.h>
#include <cstdint>

using namespace DirectX;

bool PMDModel::Load(const char* path)
{
	// 識別子"pmd"
	FILE* fp = nullptr;	
	errno_t err = fopen_s(&fp, path, "rb");
	if (err != 0)
	{
		char cerr[256];
		strerror_s(cerr, 256, err);
		OutputDebugStringA(cerr);
		return false;
	}
	if (fp == nullptr)return false;

	// ここだけプラグマパックを使う
#pragma pack(1)
	struct PMDHeader
	{
		char id[3];
		// パディングが入ってしまう
		float version;
		char name[20];
		char comment[256];
	};
	struct Vertex
	{
		XMFLOAT3 pos; // x, y, z // 座標
		XMFLOAT3 normal_vec; // nx, ny, nz // 法線ベクトル
		XMFLOAT2 uv; // u, v // UV 座標 // MMD は頂点 UV
		uint16_t bone_num[2]; // ボーン番号 1、番号 2 // モデル変形(頂点移動)時に影響
		uint8_t bone_weight; // ボーン 1 に与える影響度 // min:0 max:100 // ボーン 2 への影響度は、(100 - bone_weight)
		uint8_t edge_flag; // 0:通常、1:エッジ無効 // エッジ(輪郭)が有効の場合
	};
	struct Material
	{
		XMFLOAT3 diffuse; // dr, dg, db // 減衰色
		float alpha; // 減衰色の不透明度
		float specularity;
		XMFLOAT3 specular; // sr, sg, sb // 光沢色
		XMFLOAT3 ambient; // mr, mg, mb // 環境色(ambient)
		uint8_t toonIndex; // toon??.bmp // 0.bmp:0xFF, 1(01).bmp:0x00 ・・・ 10.bmp:0x09
		uint8_t edgeFlag; // 輪郭、影
		uint32_t indexNum; // 面頂点数
		char textureFilePath[20]; // テクスチャファイル名またはスフィアファイル名
	};
#pragma pack()
	PMDHeader header;
	auto readSize = fread_s(&header, sizeof(header), sizeof(header), 1, fp);
	// プラグマパック使わない場合
	//auto readSize = fread_s(&header.id, sizeof(header.id), sizeof(header.id), 1, fp);
	//readSize = fread_s(&header.version, sizeof(header) - sizeof(header.id), sizeof(header)- sizeof(header.id), 1, fp);
	uint32_t vertexCount = 0;
	readSize = fread_s(&vertexCount, sizeof(vertexCount), sizeof(vertexCount), 1, fp);

	std::vector<Vertex>vert(vertexCount);
	readSize = fread_s(vert.data(),
		vert.size() * sizeof(vert[0]),
		vert.size() * sizeof(vert[0]),
		1, fp);
	vertices_.resize(vertexCount);
	for (int i = 0; i < vertices_.size(); i++)
	{
		vertices_[i].pos = vert[i].pos;
		vertices_[i].normal = vert[i].normal_vec;
		vertices_[i].uv = vert[i].uv;
	}
	uint32_t indexNum;
	readSize = fread_s(&indexNum, sizeof(indexNum), sizeof(indexNum), 1, fp);
	indices_.resize(indexNum);
	readSize = fread_s(indices_.data(),
		indices_.size() * sizeof(indices_[0]),
		indices_.size() * sizeof(indices_[0]),
		1, fp);

	uint32_t materialNum;
	readSize = fread_s(&materialNum, sizeof(materialNum), sizeof(materialNum), 1, fp);
	std::vector<Material>materials(materialNum);
	readSize = fread_s(materials.data(),
		materials.size() * sizeof(materials[0]),
		materials.size() * sizeof(materials[0]),
		1, fp);

	for (auto m : materials)
	{
		PMDMaterial mat;
		mat.diffuse = m.diffuse;
		mat.speqular = m.specular;
		mat.ambient = m.ambient;
		mat.alpha = m.alpha;
		mat.speqularity = m.specularity;
		mat.indexNum = m.indexNum;
		materials_.push_back(mat);
	}


	fclose(fp);
	return true;

}

const std::vector<PMDVertex>& PMDModel::GetVertexData() const
{
	return vertices_;
}

const std::vector<uint16_t>& PMDModel::GetIndexData() const
{
	return indices_;
}

const std::vector<PMDMaterial>& PMDModel::GetMaterialData() const
{
	return materials_;
}
