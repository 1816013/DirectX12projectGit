#pragma once
#include <string>
#include <vector>
#include <Windows.h>

/// <summary>
/// 文字列操作用
/// </summary>
class StrOperater
{
public:
	/// <summary>
	/// モデルパスとテクスチャパスからテクスチャ取得
	/// </summary>
	/// <param name="modelPath"></param>
	/// <param name=""></param>
	/// <returns></returns>
	static std::string GetTextureFromModelAndTexPath(const std::string& modelPath, const std::string&);
	/// <summary>
	/// stringをwstringに変換
	/// </summary>
	/// <param name="str">変換する文字列</param>
	/// <returns>wstringになった文字列</returns>
	static std::wstring GetWideStringfromString(const std::string& str);

	static std::string GetStringfromWideString(const std::wstring& wstr);
	/// <summary>
	/// 拡張子抽出
	/// </summary>
	/// <param name="paths">抽出元のパス</param>
	/// <returns>抽出された拡張子</returns>
	static std::string GetExtension(const std::string& path);

	/// <summary>
	/// 拡張子抽出(wstring)
	/// </summary>
	/// <param name="paths">抽出元のパス</param>
	/// <returns>抽出された拡張子</returns>
	static std::wstring GetExtension(const std::wstring& path);

	/// <summary>
	/// テクスチャのパスをセパレータ文字で分離する
	/// </summary>
	/// <param name="paths">パス文字列</param>
	/// <param name="splitter">セパレータ文字</param>
	/// <returns>分離後の文字列ペア</returns>
	static std::vector<std::string> SplitFileName(const std::string& path, const char splitter = '*');
};

