#include "Common.hlsli"

VsOutput VS( float4 pos : POSITION , float2 uv : TEXCOORD) //: SV_POSITION
{
	VsOutput o;
	o.svpos = pos;
	o.pos = pos;
	o.uv = uv;
	return o;
}