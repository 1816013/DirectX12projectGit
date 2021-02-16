struct BoardOutput
{
	float4 svPos : SV_POSITION;
	float2 uv : TEXCOORD;
};
// 定数バッファ
cbuffer Const : register(b0)
{
    matrix proj;    // AO用プロジェクション
    matrix invProj; // AO用逆プロジェクション
    float2 clickPos; // 座標
    float time; // 時間
    
    // imgui用
    bool ssaoActive;    // SSAO
    bool bloomActive;   // ブルーム
    bool dofActive;     // 被写界深度
    bool outLineN;      // 法線アウトライン
    bool outLineD;      // 深度アウトライン
    bool debug;         // デバッグ
    
    float3 bloomCol;
}
