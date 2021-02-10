#include "common.hlsli"
Texture2D<float> lightDepthTex : register(t0); // 通常テクスチャ
//SamplerState smp : register(s0);

struct PsOutput
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 hdr : SV_TARGET2;
};

PsOutput PrimitivePS(PrimOut input)
{
    PsOutput output;
    // ライトから見た座標は-1～1に正規化されるためuv値に利用できる
    float2 uv = (input.lvpos.xy + float2(1, -1)) * float2(0.5, -0.5);
    output.normal = input.normal;
    if (isShadow)
    {
    
        if (input.lvpos.z > lightDepthTex.Sample(smp, uv))
        {
            output.color = float4(0.5, 0.5, 0.5, 1);
            float gray = dot(float3(0.299, 0.587, 0.114), output.color.rgb);
            output.hdr = step(0.95f, gray);
            return output;
        }
    }
    output.color = float4(uv, 1, 1);
    
   
    output.hdr = float4(0.0f, 0.0f, 0.0f, 0.0f );
   
    //float4 b = lightDepthTex.Sample(smp, uv);
    return output;
}