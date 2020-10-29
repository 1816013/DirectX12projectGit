cbuffer Matrix:register(b0)
{
	matrix world;	// ���[���h�s��
	matrix viewproj;	// �J�����s��
	float3 diffuse;	// �f�B�t���[�Y�F

}

// ���_�f�[�^�o��
struct VsOutput
{
	float4 svpos : SV_POSITION;	// �V�X�e�����W
	float4 pos : POSITION;		// ���_���W
	float4 norm : NORMAL;		// �@��
	float2 uv : TEXCOORD;		// uv���W
};