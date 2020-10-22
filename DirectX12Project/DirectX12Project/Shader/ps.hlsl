#include "Common.hlsli"
Texture2D<float4>tex:register(t0);
SamplerState smp:register(s0);

float4 PS(VsOutput input) : SV_TARGET
{
	return tex.Sample(smp, input.uv);
	//return float4(input.uv.x, input.uv.y, 1.0f, 1.0f);
}