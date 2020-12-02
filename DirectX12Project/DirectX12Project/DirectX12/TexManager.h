#pragma once
#include <d3d12.h>
#include <string>
#include <d3dx12.h>
#include <unordered_map>
#include "../Common.h"
#pragma comment(lib, "DirectXTex.lib")

using Microsoft::WRL::ComPtr;

struct Color
{
	uint8_t r, g, b, a;
	Color() :r(0), g(0), b(0), a(255) {};
	Color(uint8_t inr, uint8_t ing, uint8_t inb, uint8_t ina) :
		r(inr), g(ing), b(inb), a(inr) {}
	Color(uint8_t inc) :
		r(inc), g(inc), b(inc), a(255) {}
};

enum class ColTexType
{
	White,
	Black,
	Grad,
	Max
};

class TexManager
{
public:
	TexManager(ID3D12Device& dev);

	/// <summary>
	/// テクスチャ作成してバッファに書き込む
	/// </summary>
	/// <param name="path"></param>
	/// <param name="res"></param>
	/// <returns></returns>
	bool CreateTexture(const std::wstring& path, ComPtr<ID3D12Resource>& res);

	/*/// <summary>
	/// 単色テクスチャ作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateMonoColorTexture(ColTexType colType, const Color col);

	/// <summary>
	/// グラデーションテクスチャ作成
	/// </summary>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateGradationTexture(const Size size);

	/// <summary>
	/// GPUにアップロードするための準備
	/// </summary>
	/// <param name="size">大きさ</param>
	/// <param name="tex">テクスチャバッファ</param>
	/// <param name="subResData"></param>
	void SetUploadTexure(D3D12_SUBRESOURCE_DATA& subResData, ColTexType colType);

	/// <summary>
	/// デフォルトテクスチャ作成
	/// </summary>
	/// <returns></returns>
	bool CreateDefaultTextures();*/
private:
	ComPtr<ID3D12Device> dev_;
	// 重複したデータが入らないようにするためのデータ
	std::unordered_map<std::wstring, ID3D12Resource*>textureResource_;


	// デフォルトテクスチャ
	std::vector<ComPtr<ID3D12Resource>>defTextures_;
};

