#include "boardCommon.hlsli"

SamplerState smp:register(s0);
Texture2D<float4>rtvTex:register(t0);
Texture2D<float4>normalTex:register(t1);
Texture2D<float4>shadowTex:register(t2);

cbuffer Const:register(b0)
{
	float2 pos;	// ���W
	float time;	// ����
}

float4 PS(BoardOutput input) : SV_TARGET
{
	if(input.uv.x < 0.5f && input.uv.y <0.5f)
    {
        float b = shadowTex.Sample(smp, input.uv * 2.0);
      //  b = pow(b, 10);
        return float4(b, b, b, 1);

    }
	
	float2 nUV = input.uv - 0.5f;
	nUV /=time;
	nUV += 0.5;

	float2 nTex = normalTex.Sample(smp, nUV).xy;
	
	nUV = nUV * 2.0f - 1.0f;
	//return nTex;

	float4 col = rtvTex.Sample(smp, input.uv);
	float w, h, level;
	rtvTex.GetDimensions(0, w, h, level);
	float4 ret = float4(0, 0, 0, 0);
	float2 dt = float2(1.0f / w, 1.0f / h);
	float4 org = rtvTex.Sample(smp, input.uv /*+ nTex.xy * dt * 35*/);

	
	if (org.a > 0.0f)
	{
		return float4(org.rgb,org.a);
	}
	else
	{
		return float4(input.uv, 1, 1);
		float div = 1.0f / 128.0f;
		float2 aspect = float2(1.0f, w / h);
		//float2 iuv = (input.uv * 0.2) + float2(0.4,0.2);	// �g��
		float4 dest = float4(fmod(input.uv / aspect, div) / div, 1, 1);
		return dest;
	}

//	return ret;


	// �֊s���\��
	/*ret =(rtvTex.Sample(smp, input.uv)* 4 +
		rtvTex.Sample(smp, input.uv + float2(0, dt.y)) * -1 +
		rtvTex.Sample(smp, input.uv + float2(0, -dt.y)) * -1 + 
		rtvTex.Sample(smp, input.uv + float2(dt.x, 0)) * -1 +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, 0))* -1);
	float b = dot(float3(0.298912f, 0.586611f, 0.114478f),1- ret.rgb);
	b = step(0.8, b);

	return float4(b,b,b, col.a);*/
	// �G���{�X
	/*(rtvTex.Sample(smp, input.uv) +
		rtvTex.Sample(smp, input.uv + float2(0, dt.y)) * -1 +
		rtvTex.Sample(smp, input.uv + float2(0, -dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, 0)) * -1 +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, 0)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, dt.y)) * -2 +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, -dt.y)) * 2);*/
	// �ڂ���
	/*rtvTex.Sample(smp, input.uv) +
		rtvTex.Sample(smp, input.uv + float2(0, dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(0, -dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, 0)) +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, 0)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(dt.x, -dt.y)) +
		rtvTex.Sample(smp, input.uv + float2(-dt.x, -dt.y))) / 9.0f;*/

	// �|�X�^���[�[�V�����K��4
	//col = trunc(col * 4.0f) / 4.0f;

	// ������
	//float mono = dot(float3(0.299,0.587,0.114),col.rgb);
	
	

	//return float4(mono * 0.5, mono, mono * 0.5, 1);
	return col;
}