#define RS1 "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT ),"\
              "DescriptorTable( CBV(b0,numDescriptors= 1),"	\
							   "CBV(b1,numDescriptors= 1)),"	\
			  "DescriptorTable( CBV(b2,numDescriptors= 1),"	\
							   "SRV(t0,numDescriptors= 4),visibility = SHADER_VISIBILITY_PIXEL),"	\
			  "DescriptorTable( SRV(t4,numDescriptors= 1),visibility = SHADER_VISIBILITY_PIXEL)," \
			  "StaticSampler(s0),"\
			  "StaticSampler(s1,addressU = TEXTURE_ADDRESS_CLAMP,addressV = TEXTURE_ADDRESS_CLAMP,addressW = TEXTURE_ADDRESS_CLAMP),"\
			  "StaticSampler(s2,filter = FILTER_COMPARISON_MIN_MAG_MIP_LINEAR," \
							"comparisonFunc = COMPARISON_LESS_EQUAL)" \
// 座標変換
cbuffer Transform:register(b0)
{
	matrix world;	// ワールド行列
	matrix viewproj;	// カメラ行列
	matrix shadowMat;	// 影
    matrix lightVP;
    matrix trans[25];
	float4 lightPos;	// ライト座標
    float disolveTop;
    float disolveBottom ;
	
    bool isShadow;	// 影を落とすかどうか
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
SamplerComparisonState shadowCmpSmp:register(s2);

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

struct PrimOut
{
    float4 svpos : SV_POSITION; // システム座標
    float4 normal : NORMAL;
    float4 pos : POSITION0; // 頂点座標
    float4 lvpos : POSITION1; // ライトから見た頂点
};