Texture2D<float> lightDepthTex : register(t0); // �ʏ�e�N�X�`��
SamplerState smp : register(s0);

struct PrimOut
{
    float4 svpos : SV_POSITION; // �V�X�e�����W
    float4 pos : POSITION0; // ���_���W
    float4 lvpos : POSITION1; // ���C�g���猩�����_
};

float4 PrimitivePS(PrimOut input) : SV_TARGET
{
    // ���C�g���猩�����W��-1�`1�ɐ��K������邽��uv�l�ɗ��p�ł���
    float2 uv = (input.lvpos.xy + float2(1, -1)) * float2(0.5, -0.5);
    if (input.lvpos.z > lightDepthTex.Sample(smp, uv))
    {
        return float4(0.5, 0.5, 0.5, 1);

    }
    //float4 b = lightDepthTex.Sample(smp, uv);
    return float4(1, 1, 1, 1);
}