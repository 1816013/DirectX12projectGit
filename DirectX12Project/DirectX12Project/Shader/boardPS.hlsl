#include "boardCommon.hlsli"

SamplerState smp:register(s0);
Texture2D<float4>rtvTex:register(t0);	// 1パス目結果
Texture2D<float4>distTex:register(t1);	// 画面歪み用ノーマルテクスチャ
Texture2D<float>lightDepTex:register(t2);		// ライトから見た深度
Texture2D<float>depTex:register(t3);		// 1パス目深度
Texture2D<float4>normalTex:register(t4);		// 1パス目法線

cbuffer Const:register(b0)
{
	float2 pos;	// 座標
	float time;	// 時間
}

float4 PS(BoardOutput input) : SV_TARGET
{
	
    float w, h, level;
    rtvTex.GetDimensions(0, w, h, level); // スクリーン情報取得
    float2 dt = float2(1.0f / w, 1.0f / h);
 	
    if (input.uv.x < 0.25f && input.uv.y < 0.25f)
    {
        float b = lightDepTex.Sample(smp, input.uv * 4.0);
       // b = pow(b, 100);
        return float4(b, b, b, 1);
    }
    if (input.uv.x < 0.25f && input.uv.y < 0.5f)
    {
        float bright = depTex.Sample(smp, input.uv * 4.0);
        bright = pow(abs(bright), 100.0f);
        return float4(bright, bright, bright, 1);
    }
    if (input.uv.x < 0.25f && input.uv.y < 0.75f)
    {
        float4 normal = normalTex.Sample(smp, input.uv * 4.0 - float2(0,2));
        return normal;
    }
    float3 normal = normalTex.Sample(smp, input.uv).rgb;
    //return float4(normal, 1);
    //bright = pow(abs(bright), 100.0f);
    normal *= 4.0;
    normal -= normalTex.Sample(smp, input.uv + float2(0, dt.y)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(0, -dt.y)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(dt.x, 0)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(-dt.x, 0)).rgb;
    float dtstp = dot(normal, float3(1,1,1));
    dtstp = step(normal, 0.0001);
    float bright = depTex.Sample(smp, input.uv);
    //bright = pow(abs(bright), 100.0f);
    bright *= 4.0;
    bright -= depTex.Sample(smp, input.uv + float2(0, dt.y));
    bright -= depTex.Sample(smp, input.uv + float2(0, -dt.y));
    bright -= depTex.Sample(smp, input.uv + float2(dt.x,0));
    bright -= depTex.Sample(smp, input.uv + float2(-dt.x, 0));
    bright = step(bright, 0.0001);
	
  //  bright = saturate(bright * dtstp);
   // return float4(bright, bright, bright, 1);
	
	float2 nUV = input.uv - 0.5f;
	nUV /=time;
	nUV += 0.5;
	
	// ノーマルテクスチャ
    float4 nTex = distTex.Sample(smp, input.uv);
	
    float4 org = rtvTex.Sample(smp, input.uv /*+ nTex.xy * dt * 50*/);
	//nUV = nUV * 2.0f - 1.0f;
	//return nTex;
	
	// 輪郭線表示
	//  float4 ret = float4(0, 0, 0, 0);
	//  ret = (rtvTex.Sample(smp, input.uv) * 4 +
	//rtvTex.Sample(smp, input.uv + float2(0, dt.y)) * -1 +
	//rtvTex.Sample(smp, input.uv + float2(0, -dt.y)) * -1 +
	//rtvTex.Sample(smp, input.uv + float2(dt.x, 0)) * -1 +
	//rtvTex.Sample(smp, input.uv + float2(-dt.x, 0)) * -1);
	//  float b = dot(float3(0.298912f, 0.586611f, 0.114478f), 1 - ret.rgb);
	// return float4(b, b, b, org.a);
	
    if (org.a > 0.0f)
	{
        return float4(org.rgb * bright, org.a);
    }
	else
	{
        float2 aspect = float2(1.0f, w / h);
        float3 eye = float3(0, 0, -2.5); //視点
        float3 tpos = float3(input.uv / aspect, 0);
        float3 ray = normalize(tpos - eye); //レイベクトル(このシェーダ内では一度計算したら固定)
        float rsph = 1.0f; //球体の半径
        //int count = 256;
        for (int i = 0; i < 64; ++i)
        {
            float len = length(fmod(eye - float3(1, 1, 10), rsph * 2) - rsph) - rsph / 2;
            eye += ray * len;
            if (len < 0.001f)
            {
                return float4((float) (64 - i) / 64.0f, (float) (64 - i) / 64.0f, (float) (64 - i) / 64.0f, 1);
            }
        }
        return float4(0, 0, 0, 0);
		////return float4(input.uv, 1, 1);
		//float div = 1.0f / 128.0f;
		//float2 aspect = float2(1.0f, w / h);
		////float2 iuv = (input.uv * 0.2) + float2(0.4,0.2);	// 拡大
		//float4 dest = float4(fmod(input.uv / aspect, div) / div, 1, 1);
  //      
    }
	//float4 col = rtvTex.Sample(smp, input.uv);
  //  float4 ret = float4(0, 0, 0, 0);
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
	//return col;
}