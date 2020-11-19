#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include <string>

using QuaternionMap = std::unordered_map<std::string, DirectX::XMVECTOR>;
class VMDMotion
{
public:
	VMDMotion() = default;
	~VMDMotion() = default;

	/// <summary>
	/// pmdファイル読み込み
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>true:成功　false:失敗</returns>
	bool Load(const char* path);

	const QuaternionMap GetQuaternionMap()const;
private:
	QuaternionMap quaternions_;
};

