#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include <string>

struct VMDData
{
	size_t frameNo;
	DirectX::XMFLOAT4 quaternion;	// クゥオータニオン(回転情報)
	DirectX::XMFLOAT3 pos;			// 平行移動(移動先座標)
	DirectX::XMFLOAT2 bz[2];	// ベジェの中間コントロール
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
	/// pmdファイル読み込み
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>true:成功　false:失敗</returns>
	bool Load(const char* path);

	const VMDMotion GetVMDData()const;
private:
	VMDMotion vmdDatas_;
};

