struct BoardOutput
{
	float4 svPos : SV_POSITION;
	float2 uv : TEXCOORD;
};
// �萔�o�b�t�@
cbuffer Const : register(b0)
{
    matrix proj;
    matrix invProj;
    float2 clickPos; // ���W
    float time; // ����
}
