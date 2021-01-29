struct BoardOutput
{
	float4 svPos : SV_POSITION;
	float2 uv : TEXCOORD;
};
// 定数バッファ
cbuffer Const : register(b0)
{
    matrix proj;
    matrix invProj;
    float2 clickPos; // 座標
    float time; // 時間
}
