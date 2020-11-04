#pragma once
#include <DirectXMath.h>
#include <vector>
#include <cstdint>
#include <string>
struct  PMDVertex
{
	DirectX::XMFLOAT3 pos;		// ���W
	DirectX::XMFLOAT3 normal;	// �@��
	DirectX::XMFLOAT2 uv;		// uv

};

struct PMDMaterial
{
	DirectX::XMFLOAT3 diffuse;	// �f�B�t���[�Y�F
	float alpha;
	DirectX::XMFLOAT3 speqular;	// �X�y�L�����F
	float speqularity;
	DirectX::XMFLOAT3 ambient;	// �A���r�G���g�F
	uint32_t indexNum;		// �}�e���A���̃C���f�b�N�X��
};
class PMDModel
{
public:
	/// <summary>
	/// pmd�t�@�C���ǂݍ���
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


