struct BoardOutput
{
	float4 svPos : SV_POSITION;
	float2 uv : TEXCOORD;
};
// �萔�o�b�t�@
cbuffer Const : register(b0)
{
    matrix proj;    // AO�p
    matrix invProj; // AO�p
    float2 clickPos; // ���W
    float time; // ����
    
    // imgui�p
    bool ssaoActive;
    bool bloomActive; // �[�x�A�E�g���C��
    bool dofActive; // �[�x�A�E�g���C��
    bool outLineN; // �@���A�E�g���C��
    bool outLineD; // �[�x�A�E�g���C��
    
    float3 bloomCol;
}
