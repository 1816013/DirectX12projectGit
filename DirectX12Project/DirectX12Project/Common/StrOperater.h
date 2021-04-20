#pragma once
#include <string>
#include <vector>
#include <Windows.h>

/// <summary>
/// �����񑀍�p
/// </summary>
class StrOperater
{
public:
	/// <summary>
	/// ���f���p�X�ƃe�N�X�`���p�X����e�N�X�`���擾
	/// </summary>
	/// <param name="modelPath"></param>
	/// <param name=""></param>
	/// <returns></returns>
	static std::string GetTextureFromModelAndTexPath(const std::string& modelPath, const std::string&);
	/// <summary>
	/// string��wstring�ɕϊ�
	/// </summary>
	/// <param name="str">�ϊ����镶����</param>
	/// <returns>wstring�ɂȂ���������</returns>
	static std::wstring GetWideStringfromString(const std::string& str);

	static std::string GetStringfromWideString(const std::wstring& wstr);
	/// <summary>
	/// �g���q���o
	/// </summary>
	/// <param name="paths">���o���̃p�X</param>
	/// <returns>���o���ꂽ�g���q</returns>
	static std::string GetExtension(const std::string& path);

	/// <summary>
	/// �g���q���o(wstring)
	/// </summary>
	/// <param name="paths">���o���̃p�X</param>
	/// <returns>���o���ꂽ�g���q</returns>
	static std::wstring GetExtension(const std::wstring& path);

	/// <summary>
	/// �e�N�X�`���̃p�X���Z�p���[�^�����ŕ�������
	/// </summary>
	/// <param name="paths">�p�X������</param>
	/// <param name="splitter">�Z�p���[�^����</param>
	/// <returns>������̕�����y�A</returns>
	static std::vector<std::string> SplitFileName(const std::string& path, const char splitter = '*');
};

