cbuffer Matrix:register(b0)
{
	matrix world;	// ���[���h�s��
	matrix viewproj;	// �J�����s��
}

// �s�N�Z���V�F�[�_����̂݌�����
cbuffer Material:register(b1)
{
	float4 diffuse;	// �f�B�t���[�Y�F
	float4 speqular;	// �X�y�L�����F
	float4 ambient;	// �A���r�G���g�F

}

// ���_�f�[�^�o��
struct VsOutput
{
	float4 svpos : SV_POSITION;	// �V�X�e�����W
	float4 pos : POSITION;		// ���_���W
	float4 norm : NORMAL;		// �@��
	float2 uv : TEXCOORD;		// uv���W
};