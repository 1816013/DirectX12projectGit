struct BoardOutput
{
	float4 svPos : SV_POSITION;
	float2 uv : TEXCOORD;
};
// �萔�o�b�t�@
cbuffer Const : register(b0)
{
    matrix proj;    // AO�p�v���W�F�N�V����
    matrix invProj; // AO�p�t�v���W�F�N�V����
    float2 clickPos; // ���W
    float time; // ����
    
    // imgui�p
    bool ssaoActive;    // SSAO
    bool bloomActive;   // �u���[��
    bool dofActive;     // ��ʊE�[�x
    bool outLineN;      // �@���A�E�g���C��
    bool outLineD;      // �[�x�A�E�g���C��
    bool debug;         // �f�o�b�O
    
    float3 bloomCol;
}
