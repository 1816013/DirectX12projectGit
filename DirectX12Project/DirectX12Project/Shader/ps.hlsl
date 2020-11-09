#include "Common.hlsli"


float4 PS(VsOutput input) : SV_TARGET
{
	float3 light = normalize(float3(-1, 1, 1));
	float b = saturate(dot(input.norm.xyz, light));
	float4 toonCol = toon.Sample(toonSmp, float2(0.5,1.0f -b));

	float3 eye = float3(0.0f, 10.5f, 30.0f);
	float3 eray = normalize(eye - input.pos.xyz);
	float3 lray = reflect(-light, input.norm.xyz);
	// saturate��0~1�ɃN�����v�������
	// �V�F�[�_�ł̓m�[�R�X�g�ł���
	float s = saturate(pow(saturate(dot(eray, lray)), speqular.a));
	float2 spUv = input.norm.xy * float2(0.5f, -0.5f) + 0.5f;
	float4 texCol = tex.Sample(smp, input.uv);
	//return toonCol;
	return float4(max(ambient.rgb,toonCol.rgb * diffuse.rgb) + speqular.rgb * s, diffuse.a)
		* texCol
		* sph.Sample(smp, spUv)
		+ spa.Sample(smp, spUv)
		+ float4(texCol * ambient * 0.5);
}