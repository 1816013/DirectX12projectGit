#include "common.hlsli"
#define MyRS1 "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT ),"\
              "DescriptorTable( CBV(b0,numDescriptors= 2))" \
           //   "RootConstants(num32BitConstants=1, b0) " \
            //  "RootConstants(num32BitConstants=1, b1) " \
              //"CBV(b0, space=2), " \
// 座標変換
//cbuffer Transform : register(b0)
//{
//    matrix world; // ワールド行列
//    matrix viewproj; // カメラ行列
//    matrix shadowMat; // 影
//    matrix lightVP;
//    matrix trans[25];
//    float4 lightPos; // ライト座標
//}
// ボーン
//cbuffer Bone:register(b1)
//{
//	matrix boneMats[512];	// ボーンマトリックス
//}

float Random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

struct ShadowVSOutput
{
    float4 svpos : SV_POSITION; // システム座標
    float4 pos : POSITION0; // 頂点座標 
};

[RootSignature(MyRS1)]
ShadowVSOutput ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneNum : BONE_NO, float weight : WEIGHT, uint instID : SV_InstanceID)
{
    ShadowVSOutput ret;
	matrix mat = mul(trans[instID],boneMats[boneNum[0]] * weight + boneMats[boneNum[1]] * (1.0f - weight));
    ret.pos = mul(mat, pos);
	mat = mul(lightVP, mat);
    ret.svpos = mul(mat, pos);
	return ret;
}

void ShadowPS(ShadowVSOutput input)
{
    float rand = Random(floor(input.pos.xy * 10) / 10) - 0.5f;
    rand *= 0.5f;
    if (input.pos.y > disolveTop + rand)
    {
        discard;
    }
    if (input.pos.y < disolveBottom + rand)
    {
        discard;
    }
}