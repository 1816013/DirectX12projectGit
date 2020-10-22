// 頂点データ出力
struct VsOutput
{
	float4 svpos : SV_POSITION;	// システム座標
	float4 pos : POSITION;		// 頂点座標
	float2 uv : TEXCOORD;		// uv座標
};