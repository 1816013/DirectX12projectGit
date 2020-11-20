#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include <string>

struct VMDData
{
	size_t frameNo;
	DirectX::XMFLOAT4 quaternion;	// �N�D�I�[�^�j�I��(��]���)
	DirectX::XMFLOAT3 pos;			// ���s�ړ�(�ړ�����W)
	DirectX::XMFLOAT2 p1, p2;	// �x�W�F�̒��ԃR���g���[��
	VMDData(size_t f, const DirectX::XMFLOAT4& q,
		const DirectX::XMFLOAT3& p,
		float x1,float y1,
		float x2, float y2) :
		frameNo(f), quaternion(q),  pos(p), p1(x1, y1), p2(x2, y2) {};
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

