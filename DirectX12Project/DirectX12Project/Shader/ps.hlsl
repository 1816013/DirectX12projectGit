#include "Common.hlsli"
struct PsOutput
{
    float4 color : SV_TARGET0; 
    float4 normal : SV_TARGET1;
    float4 hdr : SV_TARGET2;
};

float Random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

[RootSignature(RS1)]
PsOutput PS(VsOutput input, uint instID : SV_InstanceID) 
{
    float rand = Random(floor(input.pos.xy * 10) / 10) - 0.5f;
    rand *= 0.5f;
    if (input.pos.y > disolveTop + rand)
    {
        discard;
    }
    if (input.pos.y < disolveBottom + rand)
    {
        discard;
    }
    PsOutput output;
	float3 light = normalize(lightPos);
	float b = saturate(dot(input.norm.xyz, light));
	float4 toonCol = toon.Sample(toonSmp, float2(0.0f,1.0f - b));
	float3 eye = float3(0.0f, 10.5f, 30.0f);
	float3 eray = normalize(eye - input.pos.xyz);	// 視線
	float3 lray = reflect(-light, input.norm.xyz);
	// リムライト
    float rim = saturate(1-dot(normalize(input.norm.xyz), normalize(eray)));
    rim = pow(rim, 5);
	// saturateは0~1にクランプするもの
	// シェーダではノーコストである
	float s = saturate(pow(saturate(dot(eray, lray)), speqular.a));
	float2 spUv = input.norm.xy * float2(0.5f, -0.5f) + 0.5f;
    float4 texCol = tex.Sample(smp, input.uv);
	//toonCol = float4(max(toonCol.rgb, float3(0.9, 0.9, 0.9)),toonCol.a);
	//return float4(toonCol.rgb,toonCol.a);
	if (instID == 1)
	{
		//return float4(0, 0, 0, 1);
	}

    float bbias = 1.0f;
    if (isShadow)
    {
    
        float sCol = 0.5f; // 影色
        float epsilon = 0.0005f;
        float2 suv = (input.lvpos.xy + float2(1, -1)) * float2(0.5, -0.5);
        //float threshold = shadowTex.SampleCmpLevelZero(shadowCmpSmp, suv, input.lvpos.z - epsilon);
        //bbias = lerp(sCol, 1.0f, threshold);
        if (input.lvpos.z > shadowTex.Sample(smp, suv) + epsilon)
        {
            bbias = 0.5f;
        }
    }
    output.color = float4(max(ambient.rgb, toonCol.rgb * diffuse.rgb * bbias) /** step(rim, 0.1f)*/ + speqular.rgb * s, diffuse.a)
		* texCol
		* sph.Sample(smp, spUv)
		+ spa.Sample(smp, spUv)
		/*+ float4(texCol.rgb * ambient.rgb * 0.5, 1)*/;
    output.color = texCol;
    output.normal = float4(input.norm.xyz + 1.0f * 0.5f,  1);
	
    float gray = dot(float3(0.299, 0.587, 0.114), output.color.rgb);
    
    // disolve
    output.hdr = pow(1.0f - saturate(abs(input.pos.y - disolveTop + rand)), 20);
    output.hdr += pow(1.0f - saturate(abs(input.pos.y - disolveBottom + rand)), 20);
    output.hdr += step(0.95f, gray);
    //output.hdr += step(0.8f,output.color);
    //output.hdr =  step(1.5f, s + b);
    return output;
}