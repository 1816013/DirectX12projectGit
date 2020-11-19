#include "VMDMotion.h"
#include <Windows.h>
#include <stdio.h>
#include <cassert>
#include <cstdint>

#include <vector>

using namespace DirectX;
using namespace std;

bool VMDMotion::Load(const char* path)
{
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
	struct VMDHeader
	{
		char vmdHeader[30];
		char vmdModekName[20];
	};
	fseek(fp, sizeof(VMDHeader), SEEK_CUR);


#pragma pack(1)
	struct Motion		// 111Bytes 
	{			
		char BoneName[15]; // �{�[����
		uint32_t FrameNo; // �t���[���ԍ�(�Ǎ����͌��݂̃t���[���ʒu��0�Ƃ������Έʒu)
		XMFLOAT3 Location; // �ʒu
		XMFLOAT4 Rotatation; // Quaternion // ��]
		uint8_t Interpolation[64]; // [4][4][4] // �⊮
	};
#pragma pack()
	uint32_t motionCount;
	fread_s(&motionCount, sizeof(uint32_t), sizeof(uint32_t), 1, fp);

	vector<Motion>motionData(motionCount);

	fread_s(motionData.data(),
		motionData.size() * sizeof(motionData[0]),
		motionData.size() * sizeof(motionData[0]),
		1, fp);
	
	for (int i = 0; i < motionCount; i++)
	{
		XMVECTOR quaternion = XMLoadFloat4(&motionData[i].Rotatation);
		quaternions_.emplace(motionData[i].BoneName, quaternion);
	}

	fclose(fp);
	return true;
}

const QuaternionMap VMDMotion::GetQuaternionMap() const
{
	return quaternions_;
}
