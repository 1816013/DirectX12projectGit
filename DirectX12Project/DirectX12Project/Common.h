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
};