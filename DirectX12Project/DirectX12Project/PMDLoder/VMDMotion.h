#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include <string>

struct VMDData
{
	size_t frameNo;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT3 pos;
	VMDData(int f, DirectX::XMFLOAT4 q, DirectX::XMFLOAT3 p) :
		frameNo(f), quaternion(q),  pos(p) {};
};


using VMDDataMap = std::unordered_map<std::string,std::vector<VMDData>>;

struct VMDMotion
{
	VMDDataMap data;
	int duration;
};
class VMDLoder
{
public:
	VMDLoder() = default;
	~VMDLoder() = default;

	/// <summary>
	/// pmdファイル読み込み
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>true:成功　false:失敗</returns>
	bool Load(const char* path);

	const VMDMotion GetVMDData()const;
private:
	VMDMotion vmdDatas_;
};

