#include "Common.hlsli"
Texture2D<float4>tex:register(t0);
SamplerState smp:register(s0);

float4 PS(VsOutput input) : SV_TARGET
{
	float3 light = normalize(float3(-1, 1, 1));
	float b = dot(input.norm.xyz, light);
	return float4(b * diffuse, 1);
	return tex.Sample(smp, input.uv);
	//return float4(input.uv.x, input.uv.y, 1.0f, 1.0f);
}