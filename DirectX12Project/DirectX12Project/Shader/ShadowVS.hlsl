
#define MyRS1 "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT ),"\
              "DescriptorTable( CBV(b0,numDescriptors= 2))" \
           //   "RootConstants(num32BitConstants=1, b0) " \
            //  "RootConstants(num32BitConstants=1, b1) " \
              //"CBV(b0, space=2), " \
// ���W�ϊ�
// ���W�ϊ�
cbuffer Transform : register(b0)
{
    matrix world; // ���[���h�s��
    matrix viewproj; // �J�����s��
    matrix shadowMat; // �e
    matrix lightVP;
    matrix trans[25];
    float4 lightPos; // ���C�g���W
}
// �{�[��
cbuffer Bone:register(b1)
{
	matrix boneMats[512];	// �{�[���}�g���b�N�X
}

[RootSignature(MyRS1)]
float4 ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneNum : BONE_NO, float weight : WEIGHT, uint instID : SV_InstanceID):SV_Position
{
	matrix mat = mul(trans[instID],boneMats[boneNum[0]] * weight + boneMats[boneNum[1]] * (1.0f - weight));
	mat = mul(lightVP, mat);
	return mul(mat,pos);
}