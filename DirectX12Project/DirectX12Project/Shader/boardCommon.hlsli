struct BoardOutput
{
	float4 svPos : SV_POSITION;
	float2 uv : TEXCOORD;
};
// 定数バッファ
cbuffer Const : register(b0)
{
    matrix proj;    // AO用
    matrix invProj; // AO用
    float2 clickPos; // 座標
    float time; // 時間
    
    // imgui用
    bool ssaoActive;
    bool bloomActive; // 深度アウトライン
    bool dofActive; // 深度アウトライン
    bool outLineN; // 法線アウトライン
    bool outLineD; // 深度アウトライン
    
    float3 bloomCol;
}
