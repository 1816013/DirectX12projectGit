// 座標変換
cbuffer Transform : register(b0)
{
    matrix world; // ワールド行列
    matrix viewproj; // カメラ行列
    matrix shadowMat; // 影
    matrix lightVP; 
    float4 lightPos; // ライト座標
}

struct PrimOut
{
    float4 svpos : SV_POSITION; // システム座標
    float4 pos : POSITION0; // 頂点座標
    float4 lvpos : POSITION1;   // ライトから見た頂点
};

PrimOut PrimitiveVS(float4 pos : POSITION)
{
    PrimOut output;
    output.pos = mul(world, pos);
    output.svpos = mul(viewproj, output.pos);
    output.lvpos = mul(lightVP, output.pos);   
    return output;
}