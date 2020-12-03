// ���W�ϊ�
cbuffer Transform : register(b0)
{
    matrix world; // ���[���h�s��
    matrix viewproj; // �J�����s��
    matrix shadowMat; // �e
    matrix lightVP; 
    float4 lightPos; // ���C�g���W
}

struct PrimOut
{
    float4 svpos : SV_POSITION; // �V�X�e�����W
    float4 pos : POSITION0; // ���_���W
    float4 lvpos : POSITION1;   // ���C�g���猩�����_
};

PrimOut PrimitiveVS(float4 pos : POSITION)
{
    PrimOut output;
    output.pos = mul(world, pos);
    output.svpos = mul(viewproj, output.pos);
    output.lvpos = mul(lightVP, output.pos);   
    return output;
}