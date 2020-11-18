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
	// ���ʎq"pmd"
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
	struct Bone
	{
		char boneName[20];		// �{�[���̖��O20
		uint16_t parentNo;		// �e�{�[���ԍ�2
		uint16_t tailNo;		// �{�[���̏I�[�ԍ�2
		uint8_t type;			// �{�[�����1
		uint16_t ikParentNo;	// ik�e�ԍ�2
		XMFLOAT3 pos;			// ���W12

	};
#pragma pack()
	PMDHeader header;
	auto readSize = fread_s(&header, sizeof(header), sizeof(header), 1, fp);
	// �v���O�}�p�b�N�g��Ȃ��ꍇ
	//auto readSize = fread_s(&header.id, sizeof(header.id), sizeof(header.id), 1, fp);
	//readSize = fread_s(&header.version, sizeof(header) - sizeof(header.id), sizeof(header)- sizeof(header.id), 1, fp);
	// ���_�ǂݍ���
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

	// �}�e���A���ǂݍ���
	uint32_t materialNum;
	readSize = fread_s(&materialNum, sizeof(materialNum), sizeof(materialNum), 1, fp);
	std::vector<Material>materials(materialNum);
	readSize = fread_s(materials.data(),
		materials.size() * sizeof(materials[0]),
		materials.size() * sizeof(materials[0]),
		1, fp);

	// �{�[���ǂݍ���
	uint16_t boneNum = 0;
	readSize = fread_s(&boneNum,
		sizeof(boneNum),
		sizeof(boneNum),
		1, fp);



	// �ǂݔ�΂�
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
	

	// IK�ǂݍ���
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

	// �\�
	uint16_t skinNum = 0;
	readSize = fread_s(&skinNum,
		sizeof(skinNum),
		sizeof(skinNum),
		1, fp);

	for (int i = 0; i < skinNum; ++i)
	{
		fseek(fp, 20, SEEK_CUR);	// �\�[20]
		uint32_t skinVertNum = 0;	// ���̕\��Ɋւ�钸�_��
		readSize = fread_s(&skinVertNum,
			sizeof(skinVertNum),
			sizeof(skinVertNum),
			1, fp);
		fseek(fp, 1, SEEK_CUR);	// �\��̎��
		fseek(fp, skinVertNum * 16, SEEK_CUR);	// �\��p�̒��_�f�[�^
	}

	// �\��\����
	uint8_t skinDispNum = 0;
	readSize = fread_s(&skinDispNum,
		sizeof(skinDispNum),
		sizeof(skinDispNum),
		1, fp);
	fseek(fp, skinDispNum * 2, SEEK_CUR);

	// ik�\����
	uint8_t ikNameNum = 0;
	readSize = fread_s(&ikNameNum,
		sizeof(ikNameNum),
		sizeof(ikNameNum),
		1, fp);
	fseek(fp, ikNameNum * 50, SEEK_CUR);

	// �\���{�[�����
	uint32_t boneDispNum = 0;
	readSize = fread_s(&boneDispNum,
		sizeof(boneDispNum),
		sizeof(boneDispNum),
		1, fp);
	fseek(fp, (sizeof(uint16_t)+sizeof(uint8_t))* boneDispNum, SEEK_CUR);

	// �p���Ή�
	uint8_t isEng = 0;
	readSize = fread_s(&isEng,
		sizeof(isEng),
		sizeof(isEng),
		1, fp);
	if (isEng)
	{
		fseek(fp, 276, SEEK_CUR);
		fseek(fp, boneNum * 20, SEEK_CUR);	// �{�[���p�����X�g
		fseek(fp, (skinNum - 1) * 20, SEEK_CUR);	// �\��p�����X�g
		fseek(fp, ikNameNum * 50, SEEK_CUR);	// ik�p�����X�g
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

