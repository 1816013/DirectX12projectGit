
#define MyRS1 "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT ),"\
              "DescriptorTable( CBV(b0,numDescriptors= 2))" \
           //   "RootConstants(num32BitConstants=1, b0) " \
            //  "RootConstants(num32BitConstants=1, b1) " \
              //"CBV(b0, space=2), " \
// 座標変換
// 座標変換
cbuffer Transform : register(b0)
{
    matrix world; // ワールド行列
    matrix viewproj; // カメラ行列
    matrix shadowMat; // 影
    matrix lightVP;
    matrix trans[25];
    float4 lightPos; // ライト座標
}
// ボーン
cbuffer Bone:register(b1)
{
	matrix boneMats[512];	// ボーンマトリックス
}

[RootSignature(MyRS1)]
float4 ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneNum : BONE_NO, float weight : WEIGHT, uint instID : SV_InstanceID):SV_Position
{
	matrix mat = mul(trans[instID],boneMats[boneNum[0]] * weight + boneMats[boneNum[1]] * (1.0f - weight));
	mat = mul(lightVP, mat);
	return mul(mat,pos);
}