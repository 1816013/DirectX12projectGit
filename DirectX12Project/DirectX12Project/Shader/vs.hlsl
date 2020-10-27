#include "Common.hlsli"

cbuffer Matrix:register(b0)
{
	matrix world;	// ワールド行列
	matrix viewproj;	// カメラ行列
}

VsOutput VS( float4 pos : POSITION , float2 uv : TEXCOORD) //: SV_POSITION
{
	VsOutput output;
	pos = mul(mul(viewproj, world), pos);
	output.svpos = pos;
	output.pos = pos;
	output.uv = uv;
	return output;
}