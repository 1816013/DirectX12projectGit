#pragma once

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
};