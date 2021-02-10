#define RS1 "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT ),"\
              "DescriptorTable( CBV(b0,numDescriptors= 1),"	\
							   "CBV(b1,numDescriptors= 1)),"	\
			  "DescriptorTable( CBV(b2,numDescriptors= 1),"	\
							   "SRV(t0,numDescriptors= 4),visibility = SHADER_VISIBILITY_PIXEL),"	\
			  "DescriptorTable( SRV(t4,numDescriptors= 1),visibility = SHADER_VISIBILITY_PIXEL)," \
			  "StaticSampler(s0),"\
			  "StaticSampler(s1,addressU = TEXTURE_ADDRESS_CLAMP,addressV = TEXTURE_ADDRESS_CLAMP,addressW = TEXTURE_ADDRESS_CLAMP),"\
			  "StaticSampler(s2,filter = FILTER_COMPARISON_MIN_MAG_MIP_LINEAR," \
							"comparisonFunc = COMPARISON_LESS_EQUAL)" \
// ���W�ϊ�
cbuffer Transform:register(b0)
{
	matrix world;	// ���[���h�s��
	matrix viewproj;	// �J�����s��
	matrix shadowMat;	// �e
    matrix lightVP;
    matrix trans[25];
	float4 lightPos;	// ���C�g���W
    float disolveTop;
    float disolveBottom ;
	
    bool isShadow;	// �e�𗎂Ƃ����ǂ���
}

// �{�[��
cbuffer Bone:register(b1)
{
	matrix boneMats[512];	// �{�[���}�g���b�N�X
}

// �s�N�Z���V�F�[�_����̂݌�����
// �}�g���b�N�X
cbuffer Material:register(b2)
{
	float4 diffuse;	// �f�B�t���[�Y�F
	float4 speqular;	// �X�y�L�����F
	float4 ambient;	// �A���r�G���g�F
}

// �e�N�X�`��
Texture2D<float4>tex:register(t0);	// �ʏ�e�N�X�`��
Texture2D<float4>sph:register(t1);	// ��Z�X�t�B�A�}�b�v
Texture2D<float4>spa:register(t2);	// ���Z�X�t�B�A�}�b�v
Texture2D<float4>toon:register(t3);	// �g�D�[��

 // �V���h�E�}�b�v
Texture2D<float> shadowTex : register(t4);

SamplerState smp:register(s0);
SamplerState toonSmp:register(s1);
SamplerComparisonState shadowCmpSmp:register(s2);

// ���_�f�[�^�o��
struct VsOutput
{
	float4 svpos : SV_POSITION;	// �V�X�e�����W
	float4 pos : POSITION0;		// ���_���W
    float4 lvpos : POSITION1;	// 
	float4 norm : NORMAL;		// �@��
	float2 uv : TEXCOORD;		// uv���W
	min16uint2 boneNum : BONE_NO;	// �f�o�b�O�p�{�[���ԍ�
	float weight : WEIGHT;		// �f�o�b�O�p�E�F�C�g
	uint instID : SV_InstanceID;
};

struct PrimOut
{
    float4 svpos : SV_POSITION; // �V�X�e�����W
    float4 normal : NORMAL;
    float4 pos : POSITION0; // ���_���W
    float4 lvpos : POSITION1; // ���C�g���猩�����_
};