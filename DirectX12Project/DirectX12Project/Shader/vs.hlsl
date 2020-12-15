#include "Common.hlsli"

[RootSignature(RS1)]
VsOutput VS( float4 pos : POSITION ,float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneNum : BONE_NO, float weight : WEIGHT, uint instID:SV_InstanceID) //: SV_POSITION
{
	VsOutput output;

	matrix mat = mul(instID == 0 ? world:shadowMat,boneMats[boneNum[0]] * weight + boneMats[boneNum[1]] * (1.0f - weight));
	output.pos = mul(mat, pos);// 3d�̍��W
	pos = mul(viewproj, output.pos);	// 2D�ɉ����Ԃ�
	output.svpos = pos;	// �V�X�e���̃|�W�V����
    output.lvpos = mul(lightVP,output.pos); // �����̍��W
	//matrix wld = world;
	//wld._14_24_34  = 0;	
	normal.w = 0;// ���s��������
	output.norm = mul(mat, normal);
	
	output.uv = uv;

	output.boneNum = boneNum;
	output.weight = weight;
	output.instID = instID;
	return output;
}