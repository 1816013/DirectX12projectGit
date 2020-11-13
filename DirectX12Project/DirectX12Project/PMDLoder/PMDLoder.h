#pragma once
#include <DirectXMath.h>
#include <vector>
#include <cstdint>
#include <string>

/// <summary>
/// PMDの頂点情報
/// </summary>
struct  PMDVertex
{
	DirectX::XMFLOAT3 pos;		// 座標
	DirectX::XMFLOAT3 normal;	// 法線
	DirectX::XMFLOAT2 uv;		// uv

};

/// <summary>
/// PMDのマテリアル情報
/// </summary>
struct PMDMaterial
{
	DirectX::XMFLOAT3 diffuse;	// ディフューズ色
	float alpha;
	DirectX::XMFLOAT3 speqular;	// スペキュラ色
	float speqularity;
	DirectX::XMFLOAT3 ambient;	// アンビエント色
	uint32_t indexNum;		// マテリアルのインデックス数
};
struct PMDBone
{
	std::string name;	// ボーンの名前
	std::vector<int> children;	// 子ボーンの番号
	DirectX::XMFLOAT3 pos;
#ifdef _DEBUG
	std::vector<std::string> childrenName;
#endif // _DEBUG
};

/// <summary>
/// PMDモデルデータを扱うクラス
/// </summary>
class PMDLoder
{
public:
	/// <summary>
	/// pmdファイル読み込み
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>true:成功　false:失敗</returns>
	bool Load(const char* path);
	
	/// <summary>
	/// 頂点情報取得
	/// </summary>
	/// <returns>頂点情報</returns>
	const std::vector<PMDVertex>& GetVertexData()const;

	/// <summary>
	/// インデックス情報取得
	/// </summary>
	/// <returns>インデックス情報</returns>
	const std::vector<uint16_t>& GetIndexData()const;

	/// <summary>
	/// マテリアル情報取得
	/// </summary>
	/// <returns>マテリアル情報</returns>
	const std::vector<PMDMaterial>& GetMaterialData()const;

	/// <summary>
	/// テクスチャ情報取得
	/// </summary>
	/// <returns>テクスチャ情報</returns>
	const std::vector<std::string>& GetTexturePaths()const;

	/// <summary>
	/// トゥーンファイルパス取得
	/// </summary>
	/// <returns>トゥーンファイルパス</returns>
	const std::vector<std::string>& GetToonPaths()const;

	/// <summary>
	/// モデルのパスを取得
	/// </summary>
	/// <returns></returns>
	const std::string& GetModelPath()const;

	const std::vector<PMDBone>& GetBones_()const;

private:
	std::vector<PMDVertex>vertices_;
	std::vector<uint16_t>indices_;
	std::vector<PMDMaterial>materials_;
	std::vector<std::string>texturePaths_;
	std::vector<std::string>toonPaths_;
	
	std::string modelPath_;
	std::vector<PMDBone>bones_;	// ボーン情報	
	std::vector<DirectX::XMMATRIX>boneMats;	// ボーン行列
	
};


