#include "Common.hlsli"


float4 PS(VsOutput input) : SV_TARGET
{
	float3 light = normalize(float3(-1, 1, 1));
	float b = saturate(dot(input.norm.xyz, light));
	float4 toonCol = toon.Sample(toonSmp, float2(0.5,1.0f -b));

	float3 eye = float3(0.0f, 10.5f, 30.0f);
	float3 eray = normalize(eye - input.pos.xyz);
	float3 lray = reflect(-light, input.norm.xyz);
	// saturateは0~1にクランプするもの
	// シェーダではノーコストである
	float s = saturate(pow(saturate(dot(eray, lray)), speqular.a));
	float2 spUv = input.norm.xy * float2(0.5f, -0.5f) + 0.5f;
	//return toonCol;
	return float4(max(ambient.rgb ,toonCol * diffuse.rgb )+ speqular.rgb * s, diffuse.a)
		* tex.Sample(smp, input.uv)
		* sph.Sample(smp, spUv)
		+ spa.Sample(smp, spUv);
	//return tex.Sample(smp, input.uv);
	//return float4(input.uv.x, input.uv.y, 1.0f, 1.0f);
}