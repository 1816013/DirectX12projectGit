cbuffer Matrix:register(b0)
{
	matrix world;	// ワールド行列
	matrix viewproj;	// カメラ行列
	float3 diffuse;	// ディフューズ色

}

// 頂点データ出力
struct VsOutput
{
	float4 svpos : SV_POSITION;	// システム座標
	float4 pos : POSITION;		// 頂点座標
	float4 norm : NORMAL;		// 法線
	float2 uv : TEXCOORD;		// uv座標
};