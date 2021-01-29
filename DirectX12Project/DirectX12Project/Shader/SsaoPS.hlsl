#include "boardCommon.hlsli"

Texture2D<float> depTex : register(t3); // 1パス目深度
Texture2D<float4> normalTex : register(t4); // 1パス目法線
SamplerState smp : register(s0);

float Random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

float SsaoPS(BoardOutput input) : SV_TARGET
{
    float w, h, mip;
    depTex.GetDimensions(0, w, h, mip);
    float2 dUV = float2(1.0f / w, 1.0f / h);
    
    float depth = depTex.Sample(smp, input.uv);
    float4 orgPos = mul(invProj, float4(input.uv * float2(2, -2) + float2(-1, 1), depth, 1));
    orgPos.xyz = orgPos.xyz / orgPos.w; // 座標Xp

    float4 N = normalTex.Sample(smp, input.uv);
    N = (N * 2.0f) - 1.0f;
    N.xyz= normalize(N.xyz);
    const float rad = 1.0f;
    float div = 0.0f;
    float ao = 0.0f;
    if (depth < 1.0f)
    {
        for (int i = 0; i < 64; i++)
        {
            float3 omega;
            //omega.x = Random(input.uv + float2(0, dUV.y * i)) * 2 - 1;
            //omega.y = Random(input.uv + dUV * i) * 2 - 1;
            //omega.z = Random(input.uv + float2(dUV.x * i, 0)) * 2 - 1;
            omega.x = Random( float2(dUV.x * i, dUV.y * i)) * 2 - 1;
            omega.y = Random(float2(omega.x, dUV.y * i)) * 2 - 1;
            omega.z = Random(float2(omega.y, omega.x)) * 2 - 1;
            omega = normalize(omega);
            float dt = dot(N.xyz, omega);
            omega *= sign(dt);
            dt *= sign(dt);
            float4 rd = float4(orgPos.xyz + omega * rad, 1);
            rd = mul(proj, rd);
            rd.xyz /= rd.w;
            float2 rdUV = ((rd.xy + float2(1, -1)) * float2(0.5, -0.5));
            float rdD = depTex.Sample(smp, rdUV);
            if (rd.z > rdD)
            {
                ao += dt;
            }
            div += dt;
        }
        ao /= div;

    }
    return 1.0f - ao;

}