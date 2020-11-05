#include "Common.hlsli"

VsOutput VS( float4 pos : POSITION ,float4 normal : NORMAL, float2 uv : TEXCOORD) //: SV_POSITION
{
	VsOutput output;
	output.pos = mul(world, pos);
	pos = mul(viewproj, output.pos)/*mul(mul(viewproj, world), pos)*/;
	output.svpos = pos;
	//matrix wld = world;
	//wld._14_24_34  = 0;	
	normal.w = 0;// 平行成分無効
	output.norm = mul(world,normal);
	
	output.uv = uv;
	return output;
}