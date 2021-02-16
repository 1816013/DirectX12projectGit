#include "StrOperater.h"

using namespace std;

wstring StrOperater::GetWideStringfromString(const string& str)
{
	wstring ret;
	// 一回目はサイズ
	auto wstringSize = MultiByteToWideChar(CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		static_cast<int>(str.length()),
		nullptr,
		0);
	ret.resize(wstringSize);
	// 二回目は文字列
	MultiByteToWideChar(CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		static_cast<int>(str.length()),
		&ret[0],
		static_cast<int>(ret.length()));
	return ret;
}

string StrOperater::GetStringfromWideString(const wstring& wstr)
{
	string ret;
	// 一回目はサイズ
	auto stringSize = WideCharToMultiByte(CP_ACP,
		WC_COMPOSITECHECK | WC_DEFAULTCHAR,
		wstr.c_str(),
		static_cast<int>(wstr.length()),
		nullptr,
		0,
		nullptr,
		nullptr);
	ret.resize(stringSize);
	// 二回目は文字列
	WideCharToMultiByte(CP_ACP,
		WC_COMPOSITECHECK | WC_DEFAULTCHAR,
		wstr.c_str(),
		static_cast<int>(wstr.length()),
		&ret[0],
		static_cast<int>(ret.length()),
		nullptr,
		nullptr);
	return ret;
}

string StrOperater::GetExtension(const string& path)
{
	int idx = static_cast<int>(path.rfind("."));
	if (idx == string::npos)
	{
		return "";
	}
	idx++;
	return path.substr(idx, path.length() - idx);
}

wstring StrOperater::GetExtension(const wstring& path)
{
	int idx = static_cast<int>(path.rfind(L"."));
	if (idx == string::npos)
	{
		return L"";
	}
	idx++;
	return path.substr(idx, path.length() - idx);
}

vector<string> StrOperater::SplitFileName(const string& path, const char splitter)
{
	int idx = static_cast<int>(path.find(splitter));
	vector<string>ret;
	if (idx == string::npos)
	{
		ret.push_back(path);

		return ret;
	}

	ret.push_back(path.substr(0, idx));
	idx++;
	ret.push_back(path.substr(idx, path.length() - idx));
	return ret;
}

std::string StrOperater::GetTextureFromModelAndTexPath(const std::string& modelPath, const std::string& texPath)
{
	auto idx1 = modelPath.rfind('/');
	if (idx1 == std::string::npos)
	{
		idx1 = 0;
	}
	auto idx2 = modelPath.rfind('\\');
	if (idx2 == std::string::npos)
	{
		idx2 = 0;
	}
	auto pathIndex = max(idx1, idx2);
	auto folderPath = modelPath.substr(0, pathIndex + 1);
	return folderPath + texPath;
}
