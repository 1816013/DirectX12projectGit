#include "PMDModel.h"
#include <Windows.h>
#include <stdio.h>
#include <cstdint>

using namespace DirectX;

bool PMDModel::Load(const char* path)
{
	// ���ʎq"pmd"
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

	// ���������v���O�}�p�b�N���g��
#pragma pack(1)
	struct PMDHeader
	{
		char id[3];
		// �p�f�B���O�������Ă��܂�
		float version;
		char name[20];
		char comment[256];
	};
	struct Vertex
	{
		XMFLOAT3 pos; // x, y, z // ���W
		XMFLOAT3 normal_vec; // nx, ny, nz // �@���x�N�g��
		XMFLOAT2 uv; // u, v // UV ���W // MMD �͒��_ UV
		uint16_t bone_num[2]; // �{�[���ԍ� 1�A�ԍ� 2 // ���f���ό`(���_�ړ�)���ɉe��
		uint8_t bone_weight; // �{�[�� 1 �ɗ^����e���x // min:0 max:100 // �{�[�� 2 �ւ̉e���x�́A(100 - bone_weight)
		uint8_t edge_flag; // 0:�ʏ�A1:�G�b�W���� // �G�b�W(�֊s)���L���̏ꍇ
	};
	struct Material
	{
		XMFLOAT3 diffuse; // dr, dg, db // �����F
		float alpha; // �����F�̕s�����x
		float specularity;
		XMFLOAT3 specular; // sr, sg, sb // ����F
		XMFLOAT3 ambient; // mr, mg, mb // ���F(ambient)
		uint8_t toonIndex; // toon??.bmp // 0.bmp:0xFF, 1(01).bmp:0x00 �E�E�E 10.bmp:0x09
		uint8_t edgeFlag; // �֊s�A�e
		uint32_t indexNum; // �ʒ��_��
		char textureFilePath[20]; // �e�N�X�`���t�@�C�����܂��̓X�t�B�A�t�@�C����
	};
#pragma pack()
	PMDHeader header;
	auto readSize = fread_s(&header, sizeof(header), sizeof(header), 1, fp);
	// �v���O�}�p�b�N�g��Ȃ��ꍇ
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
