#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include <string>

struct VMDData
{
	size_t frameNo;
	DirectX::XMFLOAT4 quaternion;	// クゥオータニオン(回転情報)
	DirectX::XMFLOAT3 pos;			// 平行移動(移動先座標)
	DirectX::XMFLOAT2 p1, p2;	// ベジェの中間コントロール
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
	/// pmdファイル読み込み
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>true:成功　false:失敗</returns>
	bool Load(const char* path);

	const VMDMotion GetVMDData()const;
private:
	VMDMotion vmdDatas_;
};

