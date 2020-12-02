#pragma once
#include <string>
#include <Windows.h>
#include <d3d12.h>
struct Size
{
	size_t width;
	size_t height;
};

class Common
{
public:
	/// <summary>
	/// alignの倍数にした値を返す
	/// </summary>
	/// <param name="value">値</param>
	/// <param name="align">align値</param>
	/// <returns>alignの倍数にした値</returns>
	static unsigned int AligndValue(unsigned int value, unsigned int align)
	{
		//return (value + (align - 1)) &~ (align-1);
		return value + (align - (value % align)) % align;
	}

	/// <summary>
	/// エラー情報を出力に表示
	/// </summary>
	/// <param name="errBlob">エラー情報</param>
	static void OutputFromErrorBlob(ID3DBlob* errBlob)
	{
		if (errBlob != nullptr)
		{
			std::string errStr = "";
			auto errSize = errBlob->GetBufferSize();
			errStr.resize(errSize);
			std::copy_n((char*)errBlob->GetBufferPointer(), errSize, errStr.begin());
			OutputDebugStringA(errStr.c_str());
		}
	}
};