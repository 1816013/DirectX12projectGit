Texture2D<float> lightDepthTex : register(t0); // 通常テクスチャ
SamplerState smp : register(s0);

struct PrimOut
{
    float4 svpos : SV_POSITION; // システム座標
    float4 pos : POSITION0; // 頂点座標
    float4 lvpos : POSITION1; // ライトから見た頂点
};

float4 PrimitivePS(PrimOut input) : SV_TARGET
{
    // ライトから見た座標は-1～1に正規化されるためuv値に利用できる
    float2 uv = (input.lvpos.xy + float2(1, -1)) * float2(0.5, -0.5);
    if (input.lvpos.z > lightDepthTex.Sample(smp, uv))
    {
        return float4(0.5, 0.5, 0.5, 1);

    }
    //float4 b = lightDepthTex.Sample(smp, uv);
    return float4(1, 1, 1, 1);
}