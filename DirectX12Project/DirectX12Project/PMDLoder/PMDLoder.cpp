#include "PMDLoder.h"
#include <Windows.h>
#include <stdio.h>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <array>

using namespace DirectX;
using namespace std;

bool PMDLoder::Load(const char* path)
{
	modelPath_ = path;
	// 識別子"pmd"
	FILE* fp = nullptr;	
	errno_t err = fopen_s(&fp, path, "rb");
	if (err != 0)
	{
		char cerr[256];
		strerror_s(cerr, 256, err);
		OutputDebugStringA(cerr);
		assert(0);
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
	struct Bone
	{
		char boneName[20];		// ボーンの名前20
		uint16_t parentNo;		// 親ボーン番号2
		uint16_t tailNo;		// ボーンの終端番号2
		uint8_t type;			// ボーン種別1
		uint16_t ikParentNo;	// ik親番号2
		XMFLOAT3 pos;			// 座標12

	};
#pragma pack()
	PMDHeader header;
	auto readSize = fread_s(&header, sizeof(header), sizeof(header), 1, fp);
	// プラグマパック使わない場合
	//auto readSize = fread_s(&header.id, sizeof(header.id), sizeof(header.id), 1, fp);
	//readSize = fread_s(&header.version, sizeof(header) - sizeof(header.id), sizeof(header)- sizeof(header.id), 1, fp);
	// 頂点読み込み
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
		copy(begin(vert[i].bone_num),
			end(vert[i].bone_num), vertices_[i].boneNum);
		vertices_[i].weight = static_cast<float>(vert[i].bone_weight) / 100.0f;
	}
	uint32_t indexNum;
	readSize = fread_s(&indexNum, sizeof(indexNum), sizeof(indexNum), 1, fp);
	indices_.resize(indexNum);
	readSize = fread_s(indices_.data(),
		indices_.size() * sizeof(indices_[0]),
		indices_.size() * sizeof(indices_[0]),
		1, fp);

	// マテリアル読み込み
	uint32_t materialNum;
	readSize = fread_s(&materialNum, sizeof(materialNum), sizeof(materialNum), 1, fp);
	std::vector<Material>materials(materialNum);
	readSize = fread_s(materials.data(),
		materials.size() * sizeof(materials[0]),
		materials.size() * sizeof(materials[0]),
		1, fp);

	// ボーン読み込み
	uint16_t boneNum = 0;
	readSize = fread_s(&boneNum,
		sizeof(boneNum),
		sizeof(boneNum),
		1, fp);



	// 読み飛ばし
	std::vector<Bone>boneData(boneNum);
	readSize = fread_s(boneData.data(),
		boneData.size() * sizeof(boneData[0]),
		boneData.size() * sizeof(boneData[0]),
		1, fp);

	bones_.resize(boneNum);

	for (int i = 0; i < boneNum; ++i)
	{
		bones_[i].name = boneData[i].boneName;
		bones_[i].pos = boneData[i].pos;
	}
	for (int i = 0; i < boneNum; ++i)
	{
		if (boneData[i].parentNo == 0xffff)continue;
		auto pno = boneData[i].parentNo;
		bones_[pno].children.push_back(i);
#ifdef _DEBUG
		bones_[pno].childrenName.push_back(boneData[i].boneName);
#endif // _DEBUG
	}
	

	// IK読み込み
	uint16_t IKNum = 0;
	readSize = fread_s(&IKNum,
		sizeof(IKNum),
		sizeof(IKNum),
		1, fp);

	for (int i = 0; i < IKNum; ++i)
	{
		fseek(fp, 4, SEEK_CUR);
		uint8_t chainNum = 0; 
		readSize = fread_s(&chainNum,
			sizeof(chainNum),
			sizeof(chainNum),
			1, fp);
		fseek(fp, 6, SEEK_CUR);
		fseek(fp, sizeof(uint16_t) * chainNum, SEEK_CUR);
	}

	// 表情数
	uint16_t skinNum = 0;
	readSize = fread_s(&skinNum,
		sizeof(skinNum),
		sizeof(skinNum),
		1, fp);

	for (int i = 0; i < skinNum; ++i)
	{
		fseek(fp, 20, SEEK_CUR);	// 表情名[20]
		uint32_t skinVertNum = 0;	// その表情に関わる頂点数
		readSize = fread_s(&skinVertNum,
			sizeof(skinVertNum),
			sizeof(skinVertNum),
			1, fp);
		fseek(fp, 1, SEEK_CUR);	// 表情の種類
		fseek(fp, skinVertNum * 16, SEEK_CUR);	// 表情用の頂点データ
	}

	// 表情表示名
	uint8_t skinDispNum = 0;
	readSize = fread_s(&skinDispNum,
		sizeof(skinDispNum),
		sizeof(skinDispNum),
		1, fp);
	fseek(fp, skinDispNum * 2, SEEK_CUR);

	// ik表示名
	uint8_t ikNameNum = 0;
	readSize = fread_s(&ikNameNum,
		sizeof(ikNameNum),
		sizeof(ikNameNum),
		1, fp);
	fseek(fp, ikNameNum * 50, SEEK_CUR);

	// 表示ボーン情報
	uint32_t boneDispNum = 0;
	readSize = fread_s(&boneDispNum,
		sizeof(boneDispNum),
		sizeof(boneDispNum),
		1, fp);
	fseek(fp, (sizeof(uint16_t)+sizeof(uint8_t))* boneDispNum, SEEK_CUR);

	// 英名対応
	uint8_t isEng = 0;
	readSize = fread_s(&isEng,
		sizeof(isEng),
		sizeof(isEng),
		1, fp);
	if (isEng)
	{
		fseek(fp, 276, SEEK_CUR);
		fseek(fp, boneNum * 20, SEEK_CUR);	// ボーン英名リスト
		fseek(fp, (skinNum - 1) * 20, SEEK_CUR);	// 表情英名リスト
		fseek(fp, ikNameNum * 50, SEEK_CUR);	// ik英名リスト
	}
	
	array<char[100], 10>toonNames;
	readSize = fread_s(&toonNames,
		toonNames.size() * sizeof(toonNames[0]),
		toonNames.size() * sizeof(toonNames[0]),
		1, fp);

	fclose(fp);

	for (auto m : materials)
	{
		PMDMaterial mat;
		mat.diffuse = m.diffuse;
		mat.speqular = m.specular;
		mat.ambient = m.ambient;
		mat.alpha = m.alpha;
		mat.speqularity = m.specularity;
		mat.indexNum = m.indexNum;
		texturePaths_.push_back(m.textureFilePath);
		if (m.toonIndex != 0xff)
		{
			toonPaths_.push_back(toonNames[m.toonIndex]);
		}
		else
		{
			toonPaths_.push_back("");
		}

		materials_.push_back(mat);
	}
	return true;

}

const std::vector<PMDVertex>& PMDLoder::GetVertexData() const
{
	return vertices_;
}

const std::vector<uint16_t>& PMDLoder::GetIndexData() const
{
	return indices_;
}

const std::vector<PMDMaterial>& PMDLoder::GetMaterialData() const
{
	return materials_;
}

const std::vector<std::string>& PMDLoder::GetTexturePaths() const
{
	return texturePaths_;
}

const std::vector<std::string>& PMDLoder::GetToonPaths() const
{
	return toonPaths_;
}

const std::string& PMDLoder::GetModelPath() const
{
	return modelPath_;
}

const std::vector<PMDBone>& PMDLoder::GetBoneData() const
{
	return bones_;
}
const std::vector<DirectX::XMMATRIX>& PMDLoder::GetBoneMat() const
{
	return boneMats_;
}

