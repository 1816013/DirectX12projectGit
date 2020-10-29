#pragma once
#include "../Common.h"
#include <vector>
class BmpLoder
{
public:
	BmpLoder(const char* filepath);

	/// <summary>
	/// ビットマップの縦横のサイズを返す
	/// </summary>
	/// <returns>ビットマップの縦横のサイズ</returns>
	Size GetBmpSize();

	/// <summary>
	/// BMP生データを取得する
	/// </summary>
	/// <returns>BMP生データ</returns>
	const std::vector<uint8_t>& GetRawData()const;
private:
	bool LoadFile(const char* filepath);
	std::vector<uint8_t> rawData_;
	Size bmpSize_;


};

