cbuffer Matrix:register(b0)
{
	matrix world;	// ワールド行列
	matrix viewproj;	// カメラ行列
}

// ピクセルシェーダからのみ見える
cbuffer Material:register(b1)
{
	float4 diffuse;	// ディフューズ色
	float4 speqular;	// スペキュラ色
	float4 ambient;	// アンビエント色

}

// 頂点データ出力
struct VsOutput
{
	float4 svpos : SV_POSITION;	// システム座標
	float4 pos : POSITION;		// 頂点座標
	float4 norm : NORMAL;		// 法線
	float2 uv : TEXCOORD;		// uv座標
};