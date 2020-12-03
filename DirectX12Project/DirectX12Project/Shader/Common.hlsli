// 座標変換
cbuffer Transform:register(b0)
{
	matrix world;	// ワールド行列
	matrix viewproj;	// カメラ行列
	matrix shadowMat;	// 影
    matrix lightVP;
	float4 lightPos;	// ライト座標
}

// ボーン
cbuffer Bone:register(b1)
{
	matrix boneMats[512];	// ボーンマトリックス
}

// ピクセルシェーダからのみ見える
// マトリックス
cbuffer Material:register(b2)
{
	float4 diffuse;	// ディフューズ色
	float4 speqular;	// スペキュラ色
	float4 ambient;	// アンビエント色
}

// テクスチャ
Texture2D<float4>tex:register(t0);	// 通常テクスチャ
Texture2D<float4>sph:register(t1);	// 乗算スフィアマップ
Texture2D<float4>spa:register(t2);	// 加算スフィアマップ
Texture2D<float4>toon:register(t3);	// トゥーン

 // シャドウマップ
Texture2D<float> shadowTex : register(t4);

SamplerState smp:register(s0);
SamplerState toonSmp:register(s1);
SamplerState shadowSmp:register(s2);

// 頂点データ出力
struct VsOutput
{
	float4 svpos : SV_POSITION;	// システム座標
	float4 pos : POSITION0;		// 頂点座標
    float4 lvpos : POSITION1;	// 
	float4 norm : NORMAL;		// 法線
	float2 uv : TEXCOORD;		// uv座標
	min16uint2 boneNum : BONE_NO;	// デバッグ用ボーン番号
	float weight : WEIGHT;		// デバッグ用ウェイト
	uint instID : SV_InstanceID;
};