Texture2D<float> lightDepthTex : register(t0); // 通常テクスチャ
SamplerState smp : register(s0);

struct PrimOut
{
    float4 svpos : SV_POSITION; // システム座標
    float4 pos : POSITION0; // 頂点座標
    float4 lvpos : POSITION1; // ライトから見た頂点
};

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
    output.normal = float4(0, 1, 0, 1);
    if (input.lvpos.z > lightDepthTex.Sample(smp, uv))
    {
        output.color = float4(0.5, 0.5, 0.5, 1);
        output.hdr = output.color;
        return output;
    }
    output.color = float4(uv, 1, 1);
    
    output.hdr = output.color;
   
    //float4 b = lightDepthTex.Sample(smp, uv);
    return output;
}