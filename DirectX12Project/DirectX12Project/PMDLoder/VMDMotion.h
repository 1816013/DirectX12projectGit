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
	/// pmd�t�@�C���ǂݍ���
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	/// <returns>true:�����@false:���s</returns>
	bool Load(const char* path);

	const QuaternionMap GetQuaternionMap()const;
private:
	QuaternionMap quaternions_;
};

