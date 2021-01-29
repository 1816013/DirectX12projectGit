#include "Common.hlsli"

[RootSignature(RS1)]
VsOutput VS( float4 pos : POSITION ,float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneNum : BONE_NO, float weight : WEIGHT, uint instID:SV_InstanceID) //: SV_POSITION
{
	VsOutput output;

	
	//matrix wld = world;
	//wld._14_24_34  = 0;	
    matrix mat = mul(trans[instID], boneMats[boneNum[0]] * weight + boneMats[boneNum[1]] * (1.0f - weight));
    normal.w = 0; // 平行成分無効
    output.norm = mul(mat, normal);
    output.pos = pos;
    //if (instID == 1)
    //{
    //    output.pos += output.norm; // 3dの座標
    //}
    output.pos = mul(mat, output.pos); // 3dの座標
    pos = mul(viewproj, output.pos); // 2Dに押しつぶす
    output.svpos = pos; // システムのポジション
    output.lvpos = mul(lightVP, output.pos); // 光源の座標

	output.uv = uv;

	output.boneNum = boneNum;
	output.weight = weight;
	output.instID = instID;
	return output;
}