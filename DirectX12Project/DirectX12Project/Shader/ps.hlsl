#include "Common.hlsli"


float4 PS(VsOutput input) : SV_TARGET
{
	//float2 bno = float2(input.boneNum % 2);
	//float col = lerp(bno.x, bno.y, input.weight);
	// lerp = A(1-t) + Btを行うもの]
	//return float4(input.weight, 1, 1, 1);
	//return float4(float2(input.boneNum / 128.0f), 0, 1);

	float3 light = normalize(float3(-1, 1, 1));
	float b = saturate(dot(input.norm.xyz, light));
	float4 toonCol = toon.Sample(toonSmp, float2(0.0f,1.0f - b));

	float3 eye = float3(0.0f, 10.5f, 30.0f);
	float3 eray = normalize(eye - input.pos.xyz);
	float3 lray = reflect(-light, input.norm.xyz);
	// saturateは0~1にクランプするもの
	// シェーダではノーコストである
	float s = saturate(pow(saturate(dot(eray, lray)), speqular.a));
	float2 spUv = input.norm.xy * float2(0.5f, -0.5f) + 0.5f;
	float4 texCol = tex.Sample(smp, input.uv);
	toonCol = float4(max(toonCol.rgb, float3(0.9, 0.9, 0.9)),toonCol.a);
	return float4(toonCol.rgb,toonCol.a);
	return float4(max(ambient.rgb,toonCol.rgb * diffuse.rgb) + speqular.rgb * s, diffuse.a)
		* texCol
		* sph.Sample(smp, spUv)
		+ spa.Sample(smp, spUv)
		+ float4(texCol * ambient * 0.5);
}