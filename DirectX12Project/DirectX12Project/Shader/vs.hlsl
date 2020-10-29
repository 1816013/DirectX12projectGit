#include "Common.hlsli"

VsOutput VS( float4 pos : POSITION ,float4 normal : NORMAL, float2 uv : TEXCOORD) //: SV_POSITION
{
	VsOutput output;
	pos = mul(mul(viewproj, world), pos);
	output.svpos = pos;
	matrix wld = world;
	wld._14_24_34  = 0;	// ïΩçsê¨ï™ñ≥å¯
	output.norm = mul(wld,normal);
	output.pos = pos;
	output.uv = uv;
	return output;
}