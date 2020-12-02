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
	/// align�̔{���ɂ����l��Ԃ�
	/// </summary>
	/// <param name="value">�l</param>
	/// <param name="align">align�l</param>
	/// <returns>align�̔{���ɂ����l</returns>
	static unsigned int AligndValue(unsigned int value, unsigned int align)
	{
		//return (value + (align - 1)) &~ (align-1);
		return value + (align - (value % align)) % align;
	}

	/// <summary>
	/// �G���[�����o�͂ɕ\��
	/// </summary>
	/// <param name="errBlob">�G���[���</param>
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