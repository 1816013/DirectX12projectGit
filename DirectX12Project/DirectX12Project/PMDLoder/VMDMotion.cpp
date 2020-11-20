#include "VMDMotion.h"
#include <Windows.h>
#include <stdio.h>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <vector>

using namespace DirectX;
using namespace std;

bool VMDLoder::Load(const char* path)
{
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
	struct VMDHeader
	{
		char vmdHeader[30];
		char vmdModekName[20];
	};
	fseek(fp, sizeof(VMDHeader), SEEK_CUR);


#pragma pack(1)
	struct Motion		// 111Bytes 
	{			
		char BoneName[15]; // ボーン名
		uint32_t FrameNo; // フレーム番号(読込時は現在のフレーム位置を0とした相対位置)
		XMFLOAT3 Location; // 位置
		XMFLOAT4 Rotatation; // Quaternion // 回転
		uint8_t Interpolation[64]; // [4][4][4] // 補完
	};
#pragma pack()
	uint32_t motionCount;
	fread_s(&motionCount, sizeof(uint32_t), sizeof(uint32_t), 1, fp);

	vector<Motion>motionData(motionCount);

	fread_s(motionData.data(),
		motionData.size() * sizeof(motionData[0]),
		motionData.size() * sizeof(motionData[0]),
		1, fp);

	fclose(fp);

	sort(motionData.begin(), motionData.end(),
		[](const Motion& lval, const Motion& rval)
		{
			return lval.FrameNo < rval.FrameNo;
		});
	
	for (int i = 0; i < motionCount; i++)
	{
		vmdDatas_.data[motionData[i].BoneName].emplace_back(motionData[i].FrameNo, motionData[i].Rotatation, motionData[i].Location);
		vmdDatas_.duration = max(vmdDatas_.duration, motionData[i].FrameNo);
	}

	
	return true;
}

const VMDMotion VMDLoder::GetVMDData() const
{
	return vmdDatas_;
}
