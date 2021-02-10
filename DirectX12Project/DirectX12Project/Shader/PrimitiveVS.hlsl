#include "common.hlsli"
// 座標変換
//cbuffer Transform : register(b0)
//{
//    matrix world; // ワールド行列
//    matrix viewproj; // カメラ行列
//    matrix shadowMat; // 影
//    matrix lightVP; 
//    float4 lightPos; // ライト座標
//}



PrimOut PrimitiveVS(float4 pos : POSITION, float4 normal : NORMAL)
{
    PrimOut output;
    output.pos = mul(world, pos);
    output.normal = normal;
    output.normal.w = 0;
    output.normal = mul(world, normal);
    output.svpos = mul(viewproj, output.pos);
    output.lvpos = mul(lightVP, output.pos);   
    return output;
}