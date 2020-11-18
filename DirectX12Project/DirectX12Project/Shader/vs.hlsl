#include "Common.hlsli"

VsOutput VS( float4 pos : POSITION ,float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneNum : BONE_NO, float weight : WEIGHT) //: SV_POSITION
{
	VsOutput output;
	matrix mat = boneMats[boneNum[0]] * weight + boneMats[boneNum[1]] * (1.0f - weight);
	output.pos = mul(world, mul(mat, pos));// 3dの座標
	pos = mul(viewproj, output.pos)/*mul(mul(viewproj, world), pos)*/;	// 2Dに押しつぶす
	output.svpos = pos;	// システムのポジション
	//matrix wld = world;
	//wld._14_24_34  = 0;	
	normal.w = 0;// 平行成分無効
	output.norm = mul(world,normal);
	
	output.uv = uv;

	output.boneNum = boneNum;
	output.weight = weight;
	return output;
}