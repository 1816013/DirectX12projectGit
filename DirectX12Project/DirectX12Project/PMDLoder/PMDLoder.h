#pragma once
#include <DirectXMath.h>
#include <vector>
#include <cstdint>
#include <string>

/// <summary>
/// PMD�̒��_���
/// </summary>
struct  PMDVertex
{
	DirectX::XMFLOAT3 pos;		// ���W
	DirectX::XMFLOAT3 normal;	// �@��
	DirectX::XMFLOAT2 uv;		// uv

};

/// <summary>
/// PMD�̃}�e���A�����
/// </summary>
struct PMDMaterial
{
	DirectX::XMFLOAT3 diffuse;	// �f�B�t���[�Y�F
	float alpha;
	DirectX::XMFLOAT3 speqular;	// �X�y�L�����F
	float speqularity;
	DirectX::XMFLOAT3 ambient;	// �A���r�G���g�F
	uint32_t indexNum;		// �}�e���A���̃C���f�b�N�X��
};
struct PMDBone
{
	std::string name;	// �{�[���̖��O
	std::vector<int> children;	// �q�{�[���̔ԍ�
	DirectX::XMFLOAT3 pos;
#ifdef _DEBUG
	std::vector<std::string> childrenName;
#endif // _DEBUG
};

/// <summary>
/// PMD���f���f�[�^�������N���X
/// </summary>
class PMDLoder
{
public:
	/// <summary>
	/// pmd�t�@�C���ǂݍ���
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	/// <returns>true:�����@false:���s</returns>
	bool Load(const char* path);
	
	/// <summary>
	/// ���_���擾
	/// </summary>
	/// <returns>���_���</returns>
	const std::vector<PMDVertex>& GetVertexData()const;

	/// <summary>
	/// �C���f�b�N�X���擾
	/// </summary>
	/// <returns>�C���f�b�N�X���</returns>
	const std::vector<uint16_t>& GetIndexData()const;

	/// <summary>
	/// �}�e���A�����擾
	/// </summary>
	/// <returns>�}�e���A�����</returns>
	const std::vector<PMDMaterial>& GetMaterialData()const;

	/// <summary>
	/// �e�N�X�`�����擾
	/// </summary>
	/// <returns>�e�N�X�`�����</returns>
	const std::vector<std::string>& GetTexturePaths()const;

	/// <summary>
	/// �g�D�[���t�@�C���p�X�擾
	/// </summary>
	/// <returns>�g�D�[���t�@�C���p�X</returns>
	const std::vector<std::string>& GetToonPaths()const;

	/// <summary>
	/// ���f���̃p�X���擾
	/// </summary>
	/// <returns></returns>
	const std::string& GetModelPath()const;

	const std::vector<PMDBone>& GetBones_()const;

private:
	std::vector<PMDVertex>vertices_;
	std::vector<uint16_t>indices_;
	std::vector<PMDMaterial>materials_;
	std::vector<std::string>texturePaths_;
	std::vector<std::string>toonPaths_;
	
	std::string modelPath_;
	std::vector<PMDBone>bones_;	// �{�[�����	
	std::vector<DirectX::XMMATRIX>boneMats;	// �{�[���s��
	
};


