#include "boardCommon.hlsli"

SamplerState smp:register(s0);
Texture2D<float4>rtvTex:register(t0);
Texture2D<float4>normalTex:register(t1);

cbuffer Const:register(b0)
{
	float2 pos;	// 座標
	float time;	// 時間
}

float4 PS(BoardOutput input) : SV_TARGET
{
	float2 nUV = input.uv - 0.5f;
	nUV /=time;
	nUV += 0.5;
	

	float4 nTex = normalTex.Sample(smp, nUV);
	
	nUV = nUV * 2.0f - 1.0f;
	//return nTex;

	float4 col = rtvTex.Sample(smp, input.uv);
	float w, h, level;
	rtvTex.GetDimensions(0, w, h, level);
	float4 ret = float4(0, 0, 0, 0);
	float2 dt = float2(1.0f / w, 1.0f / h);
	
	return rtvTex.Sample(smp, input.uv + nTex.xy * dt * 35);

//	return ret;


	// 輪郭線表示
	/*ret =(rtvTex.Sample(smp, input.uv)* 4 +
		rtvTex.Sample(smp, input.uv + float2(0, dt.y)) * -1 +
		rtvTex.Sample(smp, input.uv + float2(0, -dt.y)) * -1 + 
		rtvTex.Sample(smp, input.uv + float2(dt.x, 0)) * -1 +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, 0))* -1);
	float b = dot(float3(0.298912f, 0.586611f, 0.114478f),1- ret.rgb);
	b = step(0.8, b);

	return float4(b,b,b, col.a);*/
	// エンボス
	/*(rtvTex.Sample(smp, input.uv) +
		rtvTex.Sample(smp, input.uv + float2(0, dt.y)) * -1 +
		rtvTex.Sample(smp, input.uv + float2(0, -dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, 0)) * -1 +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, 0)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, dt.y)) * -2 +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, -dt.y)) * 2);*/
	// ぼかし
	/*rtvTex.Sample(smp, input.uv) +
		rtvTex.Sample(smp, input.uv + float2(0, dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(0, -dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, 0)) +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, 0)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, -dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, -dt.y))) / 9.0f;*/

	// ポスタリゼーション階調4
	//col = trunc(col * 4.0f) / 4.0f;

	// 白黒化
	//float mono = dot(float3(0.299,0.587,0.114),col.rgb);
	
	

	//return float4(mono * 0.5, mono, mono * 0.5, 1);
	return col;
}