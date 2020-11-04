#include "Common.hlsli"


float4 PS(VsOutput input) : SV_TARGET
{
	float3 light = normalize(float3(-1, 1, 1));
	float b = dot(input.norm.xyz, light);

	float3 eye = float3(0.0f, 10.5f, 30.0f);
	float3 eray = normalize(eye - input.pos.xyz);
	float3 lray = reflect(-light, input.norm.xyz);
	// saturateは0~1にクランプするもの
	// シェーダではノーコストである
	float s = saturate(pow(saturate(dot(eray, lray)), speqular.a));
	return float4(b * diffuse.rgb + speqular.rgb * s + ambient.rgb, diffuse.a) * tex.Sample(smp, input.uv);
	//return tex.Sample(smp, input.uv);
	//return float4(input.uv.x, input.uv.y, 1.0f, 1.0f);
}