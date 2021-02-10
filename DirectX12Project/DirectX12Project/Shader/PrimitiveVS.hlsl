#include "common.hlsli"
// ���W�ϊ�
//cbuffer Transform : register(b0)
//{
//    matrix world; // ���[���h�s��
//    matrix viewproj; // �J�����s��
//    matrix shadowMat; // �e
//    matrix lightVP; 
//    float4 lightPos; // ���C�g���W
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