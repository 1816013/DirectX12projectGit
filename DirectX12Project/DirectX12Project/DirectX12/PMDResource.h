#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <memory>
#include <string>
#include <d3dx12.h>
#include <array>

using Microsoft::WRL::ComPtr;

// �ǂݍ��ރo�b�t�@�̃O���[�v
enum class GroopType
{
	TRANSFORM,	// ���W�ϊ�(CBV ���W�ϊ�, CBV �{�[��)
	MATERIAL,	// �}�e���A��(CBV ��{�}�e���A��, SRV bmp, SRV sph, SRV spa, SRV toon)
	DEPTH,	// �V���h�E�}�b�v(SRV �e)
	MAX
};

/// <summary>
/// �o�b�t�@�^�C�v
/// </summary>
enum class BuffType
{
	CBV,	// �萔�o�b�t�@
	SRV,	// �V�F�[�_���\�[�X(�}�e���A��)
	MAX
};
struct PMDResourceBinding
{
	std::vector<BuffType>types_;	// ���\�[�X�̏���
	struct Resource
	{
		ID3D12Resource* resource;
		int size;
	};
	std::vector<Resource> resources_;
	ComPtr<ID3D12DescriptorHeap>descHeap_;
	int strideBytes_ = 0;

	/// <summary>
	/// �i�[����o�b�t�@�̃^�C�v���Z�b�g����
	/// </summary>
	/// <param name="types">�o�b�t�@�^�C�v</param>
	void Init(std::vector<BuffType>types);

	/// <summary>
	/// ���\�[�X�o�b�t�@��ǉ�
	/// </summary>
	/// <param name="res">���\�[�X�o�b�t�@</param>
	/// <param name="size">�o�b�t�@�̈�̃T�C�Y</param>
	void AddBuffers(ID3D12Resource* res, int size = 0);
};
using PMDResources = std::array<PMDResourceBinding, static_cast<int>(GroopType::MAX)>;
class PMDResource
{
public:
	PMDResource(ID3D12Device& dev);

	/// <summary>
	/// �o�C���h�������\�[�X����F�X�쐬
	/// </summary>
	/// <param name="groopTypes">�o�b�t�@�̃O���[�v</param>
	void Build(const std::vector<GroopType>& groopTypes);

	PMDResourceBinding& GetGroops(GroopType groopType);
	void SetPMDState(ID3D12GraphicsCommandList& cmdList);
private:
	/// <summary>
	/// �o�C���h�����o�b�t�@���烊�\�[�X�r���[���쐬
	/// </summary>
	/// <param name="groopType">�O���[�v�^�C�v</param>
	void CreateResouses(const std::vector<GroopType>& groopType);

	PMDResources res_;
	ComPtr<ID3D12Device> dev_;
};

