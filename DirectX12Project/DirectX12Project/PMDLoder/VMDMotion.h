#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include <string>

struct VMDData
{
	size_t frameNo;
	DirectX::XMFLOAT4 quaternion;	// �N�D�I�[�^�j�I��(��]���)
	DirectX::XMFLOAT3 pos;			// ���s�ړ�(�ړ�����W)
	DirectX::XMFLOAT2 bz[2];	// �x�W�F�̒��ԃR���g���[��
	VMDData(size_t f, const DirectX::XMFLOAT4& q,
		const DirectX::XMFLOAT3& p,
		float x1, float y1,
		float x2, float y2) :
		frameNo(f), quaternion(q), pos(p)
	{
		bz[0] = { x1 / 127.0f, y1 / 127.0f };
		bz[1] = { x2 / 127.0f, y2 / 127.0f };
	}
};


using VMDDataMap = std::unordered_map<std::string,std::vector<VMDData>>;

struct VMDMotion
{
	VMDDataMap data;
	uint32_t duration;
};
class VMDLoder
{
public:
	VMDLoder() = default;
	~VMDLoder() = default;

	/// <summary>
	/// pmd�t�@�C���ǂݍ���
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	/// <returns>true:�����@false:���s</returns>
	bool Load(const char* path);

	const VMDMotion GetVMDData()const;
private:
	VMDMotion vmdDatas_;
};

