// ���W�ϊ�
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

// �e�N�X�`��
Texture2D<float4>tex:register(t0);	// �ʏ�e�N�X�`��
Texture2D<float4>sph:register(t1);	// ��Z�X�t�B�A�}�b�v
Texture2D<float4>spa:register(t2);	// ���Z�X�t�B�A�}�b�v
Texture2D<float4>toon:register(t3);	// ���Z�X�t�B�A�}�b�v
SamplerState smp:register(s0);
SamplerState toonSmp:register(s1);

// ���_�f�[�^�o��
struct VsOutput
{
	float4 svpos : SV_POSITION;	// �V�X�e�����W
	float4 pos : POSITION;		// ���_���W
	float4 norm : NORMAL;		// �@��
	float2 uv : TEXCOORD;		// uv���W
};