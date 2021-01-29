#include "boardCommon.hlsli"

SamplerState smp:register(s0);
Texture2D<float4>rtvTex:register(t0);	    // 1パス目結果
Texture2D<float4>distTex:register(t1);	    // 画面歪み用ノーマルテクスチャ
Texture2D<float>lightDepTex:register(t2);   // ライトから見た深度
Texture2D<float>depTex:register(t3);		// 1パス目深度
Texture2D<float4>normalTex:register(t4);	// 1パス目法線
Texture2D<float4>hdrTex:register(t5);	    // 高輝度
Texture2D<float4>hdrShrinkTex:register(t6);	// 高輝度縮小
Texture2D<float4>dofShrinkTex:register(t7);	// 被写界深度用縮小
Texture2D<float>ssaoTex:register(t8);	// SSAO


// ガウスぼかし
float4 Blur5x5(Texture2D<float4> tex, float2 uv, float2 uvWeight, float4 limit)
{
    float w, h, level;
    tex.GetDimensions(0, w, h, level); // スクリーン情報取得
    float2 dtUv = float2(1.0f / w, 1.0f / h);
    dtUv *= uvWeight;
    float div = 0.0f;
    // ぼかし
    float4 ret = 0;
    // 一段目
    ret += tex.Sample(smp, uv + dtUv * float2(-2, -2)) * 1;
    ret += tex.Sample(smp, uv + dtUv * float2(-1, -2)) * 4;
    ret += tex.Sample(smp, uv + dtUv * float2(0, -2)) * 6;
    ret += tex.Sample(smp, uv + dtUv * float2(1, -2)) * 4;
    ret += tex.Sample(smp, uv + dtUv * float2(2, -2)) * 1;
    div += 16;
    // 二段目
    ret += tex.Sample(smp, uv + dtUv * float2(-2, -1)) * 4;
    ret += tex.Sample(smp, uv + dtUv * float2(-1, -1)) * 16;
    ret += tex.Sample(smp, uv + dtUv * float2(0, -1)) * 24;
    ret += tex.Sample(smp, uv + dtUv * float2(1, -1)) * 16;
    ret += tex.Sample(smp, uv + dtUv * float2(2, -1)) * 4;
    div += 64;
     // 三段目
    ret += tex.Sample(smp, uv + dtUv * float2(-2, 0)) * 6;
    ret += tex.Sample(smp, uv + dtUv * float2(-1, 0)) * 24;
    ret += tex.Sample(smp, uv + dtUv * float2(0, 0)) * 36;
    ret += tex.Sample(smp, uv + dtUv * float2(1, 0)) * 24;
    ret += tex.Sample(smp, uv + dtUv * float2(2, 0)) * 6;
    
    div += 96;
    if (uv.y + dtUv.y < limit.y)
    {
        // 四段目
        ret += tex.Sample(smp, uv + dtUv * float2(-2, 1)) * 4;
        ret += tex.Sample(smp, uv + dtUv * float2(-1, 1)) * 16;
        ret += tex.Sample(smp, uv + dtUv * float2(0, 1)) * 24;
        ret += tex.Sample(smp, uv + dtUv * float2(1, 1)) * 16;
        ret += tex.Sample(smp, uv + dtUv * float2(2, 1)) * 4;
        div += 64;
        // 五段目
        if (uv.y + dtUv.y < limit.w)
        {
        
            ret += tex.Sample(smp, uv + dtUv * float2(-2, 2)) * 1;
            ret += tex.Sample(smp, uv + dtUv * float2(-1, 2)) * 4;
            ret += tex.Sample(smp, uv + dtUv * float2(0, 2)) * 6;
            ret += tex.Sample(smp, uv + dtUv * float2(1, 2)) * 4;
            ret += tex.Sample(smp, uv + dtUv * float2(2, 2)) * 1;
            div += 16;
        }
      
    }
    return ret / div;
}
struct ShrinkOutput
{
    float4 hdr : SV_Target0;
    float4 plain : SV_Target1;
};
// 縮小バッファ
ShrinkOutput ShrinkPSForBloom(BoardOutput input)
{
    ShrinkOutput ret;
    ret.hdr = Blur5x5(hdrTex, input.uv, float2(1.0f, 1.0f), float4(0, 0, 1, 1));
    ret.plain = Blur5x5(rtvTex, input.uv, float2(1.0f, 1.0f), float4(0, 0, 1, 1));
    return ret;
}
float4 DistortionByNormalTex(float2 uv)
{
    float2 nUV = uv - 0.5f;
   // nUV /= time;
    //nUV += 0.5;
	
	// ノーマルテクスチャ
    nUV = nUV * 2.0f - 1.0f;
    float4 nTex = distTex.Sample(smp, nUV);
	
    float4 org = rtvTex.Sample(smp, uv + nTex.xy /** dt*/ /** 50*/);
	
	return nTex;
}


float4 PS(BoardOutput input) : SV_TARGET
{
	
    float w, h, level;
    rtvTex.GetDimensions(0, w, h, level); // スクリーン情報取得
    float2 dt = float2(1.0f / w, 1.0f / h);
 	
    //if (input.uv.x < 0.25f && input.uv.y < 0.25f)
    //{
    //    float b = lightDepTex.Sample(smp, input.uv * 4.0f);
    //   // b = pow(b, 100);
    //    return float4(b, b, b, 1);
    //}
    if (input.uv.x < 0.25f && input.uv.y < 0.25f)
    {
        float bright = depTex.Sample(smp, input.uv * 4.0f);
        //bright = pow(abs(bright), 100.0f);
        return float4(bright, bright, bright, 1.0f);
    }
    if (input.uv.x < 0.25f && input.uv.y < 0.5f)
    {
        float4 normal = normalTex.Sample(smp, input.uv * 4.0f - float2(0,2.0f));
        return normal;
    }
    if (input.uv.x < 0.25f && input.uv.y < 0.75f)
    {
        float4 hdr = hdrTex.Sample(smp, input.uv * 4.0f - float2(0, 3.0f));
        return hdr;
    } 
    //if (input.uv.x < 0.125f && input.uv.y < 1.0f)
    //{
    //    float4 shrink = hdrShrinkTex.Sample(smp, input.uv * (8.0f * float2(1, 0.5f)));
    //    return shrink;
    //}
    if (input.uv.x < 0.25f && input.uv.y < 1.0f)
    {
        float ssao = ssaoTex.Sample(smp, input.uv * 4.0f - float2(0, 3.0f));
        return float4(ssao, ssao, ssao, 1);
    }
    //return DistortionByNormalTex(clickPos);
    
	// ラプシディアンフィルタ輪郭線
    float3 normal = normalTex.Sample(smp, input.uv).rgb;
	//return float4(normal, 1);
    normal *= 4.0;
    normal -= normalTex.Sample(smp, input.uv + float2(0, dt.y)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(0, -dt.y)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(dt.x, 0)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(-dt.x, 0)).rgb;
    float dtstp /*= dot(float3(1,1,1), normal)*/;
    dtstp = step((normal.r * normal.g * normal.b), 0.001f);
    float bright = depTex.Sample(smp, input.uv);
    ////bright = pow(abs(bright), 100.0f);
    bright *= 4.0;
    bright -= depTex.Sample(smp, input.uv + float2(0, dt.y));
    bright -= depTex.Sample(smp, input.uv + float2(0, -dt.y));
    bright -= depTex.Sample(smp, input.uv + float2(dt.x, 0));
    bright -= depTex.Sample(smp, input.uv + float2(-dt.x, 0));
    bright = saturate(step(bright, 0.0001f));
	bright = saturate(bright * dtstp);
	// return float4(bright, bright, bright, 1);
	
    float4 org = rtvTex.Sample(smp, input.uv);
    //org += Blur5x5(hdrTex, input.uv, float2(1.0f, 1.0f), float4(0.0f, 0.0f, 1.0f, 1.0f));
    float2 uvRate = float2(1.0f, 0.5f);
    float2 uvOffset = float2(0.0f, 0.0f);
    // ブルーム用
    //for (int i = 0; i < 8; ++i)
    //{
    //    org += Blur5x5(shrinkTex, uvOffset + input.uv * uvRate, float2(1.0f, 0.5f), float4(uvOffset, uvRate));
    //    uvOffset.y += uvRate.y;
    //    uvRate *= 0.5f;
    //}
    
    float depForDof = depTex.Sample(smp, clickPos);
    
	// ディファードレンダリング用
    //float3 nml = (normalTex.Sample(smp, input.uv).rgb * 2.0f - 1.0f);
    //float3 light = normalize(float3(-1, 1, 1));
    //float b = dot(nml, light);
    
    float ssao = ssaoTex.Sample(smp, input.uv);
    if (org.a > 0.0f)
	{
        //float depDiff = distance(depForDof, depTex.Sample(smp, input.uv));
        //if (depDiff < 0.0001f)
        //{
        //    return float4(org.rgb /** bright*/, org.a);
        //}
        //else
        //{
        //    float blendRate[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            
        //    float t = saturate(depDiff * 200.0f) * 8.0f;
        //    uint no = 0;
        //    t = modf(t, no);
            
        //    if (no > 0)
        //    {
        //        blendRate[no] = t;
        //        blendRate[no - 1] = (1.0f - t);
        //    }
        //    else
        //    {
        //        blendRate[0] = 1.0f;
        //    }
            
           
        //    float2 uvRate = float2(1.0f, 0.5f);
        //    float2 uvOffset = float2(0.0f, 0.0f);
        //    float4 dof = org * blendRate[0];
        //    dof += Blur5x5(rtvTex,input.uv, float2(1.0f, 1.0f), float4(0.0f,0.0f, 1.0f, 1.0f)) * blendRate[1];
        //    for (int i = 0; i < 6; ++i)
        //    {
                
        //        dof += Blur5x5(dofShrinkTex, uvOffset + input.uv * uvRate, float2(1.0f, 0.5f), float4(uvOffset, uvRate)) * blendRate[i + 2];
        //        uvOffset.y += uvRate.y;
        //        uvRate *= 0.5f;
        //    }
        //    return dof;
        //}
        return float4(org.rgb * ssao /** bright*/, org.a);
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
    }

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