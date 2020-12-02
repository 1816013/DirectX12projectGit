// 座標変換
cbuffer Transform : register(b0)
{
	matrix world;	// ワールド行列
	matrix viewproj;	// カメラ行列
    matrix shadowMat;
    matrix lightVP;
    float4 lightPos;
}
// ボーン
cbuffer Bone:register(b1)
{
	matrix boneMats[512];	// ボーンマトリックス
}

float4 ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneNum : BONE_NO, float weight : WEIGHT, uint instID : SV_InstanceID):SV_Position
{
	matrix mat = mul(world,boneMats[boneNum[0]] * weight + boneMats[boneNum[1]] * (1.0f - weight));
	mat = mul(lightVP, mat);
	return mul(mat,pos);
}