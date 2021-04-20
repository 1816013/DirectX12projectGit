#include "boardCommon.hlsli"

SamplerState smp:register(s0);
Texture2D<float4>rtvTex:register(t0);	    // 1�p�X�ڌ���
Texture2D<float4>distTex:register(t1);	    // ��ʘc�ݗp�m�[�}���e�N�X�`��
Texture2D<float>lightDepTex:register(t2);   // ���C�g���猩���[�x
Texture2D<float>depTex:register(t3);		// 1�p�X�ڐ[�x
Texture2D<float4>normalTex:register(t4);	// 1�p�X�ږ@��
Texture2D<float4>hdrTex:register(t5);	    // ���P�x
Texture2D<float4>hdrShrinkTex:register(t6);	// ���P�x�k��
Texture2D<float4>dofShrinkTex:register(t7);	// ��ʊE�[�x�p�k��
Texture2D<float>ssaoTex:register(t8);	// SSAO


// �K�E�X�ڂ���
float4 Blur5x5(Texture2D<float4> tex, float2 uv, float2 uvWeight, float4 limit)
{
    float w, h, level;
    tex.GetDimensions(0, w, h, level); // �X�N���[�����擾
    float2 dtUv = float2(1.0f / w, 1.0f / h);
    dtUv *= uvWeight;
    float div = 0.0f;
    // �ڂ���
    float4 ret = 0;
    // ��i��
    ret += tex.Sample(smp, uv + dtUv * float2(-2, -2)) * 1;
    ret += tex.Sample(smp, uv + dtUv * float2(-1, -2)) * 4;
    ret += tex.Sample(smp, uv + dtUv * float2(0, -2)) * 6;
    ret += tex.Sample(smp, uv + dtUv * float2(1, -2)) * 4;
    ret += tex.Sample(smp, uv + dtUv * float2(2, -2)) * 1;
    div += 16;
    // ��i��
    ret += tex.Sample(smp, uv + dtUv * float2(-2, -1)) * 4;
    ret += tex.Sample(smp, uv + dtUv * float2(-1, -1)) * 16;
    ret += tex.Sample(smp, uv + dtUv * float2(0, -1)) * 24;
    ret += tex.Sample(smp, uv + dtUv * float2(1, -1)) * 16;
    ret += tex.Sample(smp, uv + dtUv * float2(2, -1)) * 4;
    div += 64;
     // �O�i��
    ret += tex.Sample(smp, uv + dtUv * float2(-2, 0)) * 6;
    ret += tex.Sample(smp, uv + dtUv * float2(-1, 0)) * 24;
    ret += tex.Sample(smp, uv + dtUv * float2(0, 0)) * 36;
    ret += tex.Sample(smp, uv + dtUv * float2(1, 0)) * 24;
    ret += tex.Sample(smp, uv + dtUv * float2(2, 0)) * 6;
    
    div += 96;
    if (uv.y + dtUv.y < limit.y)
    {
        // �l�i��
        ret += tex.Sample(smp, uv + dtUv * float2(-2, 1)) * 4;
        ret += tex.Sample(smp, uv + dtUv * float2(-1, 1)) * 16;
        ret += tex.Sample(smp, uv + dtUv * float2(0, 1)) * 24;
        ret += tex.Sample(smp, uv + dtUv * float2(1, 1)) * 16;
        ret += tex.Sample(smp, uv + dtUv * float2(2, 1)) * 4;
        div += 64;
        // �ܒi��
        if (uv.y + dtUv.y < limit.w)
        {
        
            ret += tex.Sample(smp, uv + dtUv * float2(-2, 2)) * 1;
            ret += tex.Sample(smp, uv + dtUv * float2(-1, 2)) * 4;
            ret += tex.Sample(smp, uv + dtUv * float2(0, 2)) * 6;
            ret += tex.Sample(smp, uv + dtUv * float2(1, 2)) * 4;
            ret += tex.Sample(smp, uv + dtUv * float2(2, 2)) * 1;
            div += 16;
        }
      
    }
    return ret / div;
}
struct ShrinkOutput
{
    float4 hdr : SV_Target0;
    float4 plain : SV_Target1;
};
// �k���o�b�t�@
ShrinkOutput ShrinkPSForBloom(BoardOutput input)
{
    ShrinkOutput ret;
    ret.hdr = Blur5x5(hdrTex, input.uv, float2(1.0f, 1.0f), float4(0, 0, 1, 1));
    ret.plain = Blur5x5(rtvTex, input.uv, float2(1.0f, 1.0f), float4(0, 0, 1, 1));
    return ret;
}
float4 DistortionByNormalTex(float2 uv)
{
    float2 nUV = uv - 0.5f;
   // nUV /= time;
    //nUV += 0.5;
	
	// �m�[�}���e�N�X�`��
    nUV = nUV * 2.0f - 1.0f;
    float4 nTex = distTex.Sample(smp, nUV);
	
    float4 org = rtvTex.Sample(smp, uv + nTex.xy /** dt*/ /** 50*/);
	
	return nTex;
}

float3 mod(float3 pos, float divider)
{
    return pos - divider * floor(pos / divider);
}

float3 rotate(float3 p, float angle, float3 axis)
{
    float3 a = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float r = 1.0 - c;
    float3x3 m = float3x3(
                    a.x * a.x * r + c,
                    a.y * a.x * r + a.z * s,
                    a.z * a.x * r - a.y * s,
                    a.x * a.y * r - a.z * s,
                    a.y * a.y * r + c,
                    a.z * a.y * r + a.x * s,
                    a.x * a.z * r + a.y * s,
                    a.y * a.z * r - a.x * s,
                    a.z * a.z * r + c
                    );
    return mul(m, p);
}


float SDFCircle2D(float2 xy, float2 center, float r, float2 aspect)
{
    return length((center - xy)* aspect) - r;
}

float SDFLatticeCircle2D(float2 xy, float2 aspect, float divNum)
{
    float div = 1.0f / divNum;
    return length(fmod(xy * aspect, div) - div * 0.5) - div * 0.5f;
}
float SDFSphere(float3 pos,float3 center, float r)
{
    return length(pos - center) - r;
}
float SDFLatticeSphere(float3 pos, float r, float divider, out float normal)
{
    float3 tmp = mod(pos, divider) - divider * 0.5f;
    normal = normalize(tmp) /** float3(1, 1, -1)*/;
    return length(tmp) - r;
    //float3 q = fmod(abs(pos) + 0.5f * divider, divider) - 0.5f * divider;
    //normal = normalize(q);
    //return SDFSphere(q, float3(0, 0, 0), r);
}

float SDFRoundBox(float3 pos, float b, float r)
{
   float3 q = abs(pos) - b;
   return length(max(q, 0.0f) + min(max(q.x ,max(q.y, q.z)), 0.0f)) - r;

}
float SDFLatticeRoundBox(float3 pos,float b, float r, float divider)
{
    float3 tmp = mod(pos, divider) - divider * 0.5f;
    tmp = rotate(tmp, time, float3(1, 1, 0));
    return SDFRoundBox(tmp, b, r);
}
float SDFRoundBox2(float3 pos, float3 b, float r)
{
    float3 pp = mod(pos, 2) - 0.5f * 2;
    float3 q = abs(pp) - b;
    return length(max(q, 0.0f) + min(max(q.x, max(q.y, q.z)), 0.0f)) - r;
}

float3 NormalRoundBox(float3 pos,float b, float r, float epsilon)
{
    float3 epsX = float3(epsilon, 0, 0);
    float3 epsY = float3(0, epsilon, 0);
    float3 epsZ = float3(0, 0, epsilon);
                
    float3 n = float3(
                SDFLatticeRoundBox(pos + epsX, b, r, 2) -
                SDFLatticeRoundBox(pos - epsX, b, r, 2),
                SDFLatticeRoundBox(pos + epsY, b, r, 2) -
                SDFLatticeRoundBox(pos - epsY, b, r, 2),
                SDFLatticeRoundBox(pos + epsZ, b, r, 2) -
                SDFLatticeRoundBox(pos - epsZ, b, r, 2));
   return normalize(n);
}

float4 PS(BoardOutput input) : SV_TARGET
{
	
    float w, h, level;
    rtvTex.GetDimensions(0, w, h, level); // �X�N���[�����擾
    float2 dt = float2(1.0f / w, 1.0f / h);
    if (debug)
    {
    
        if (input.uv.x < 0.25f && input.uv.y < 0.25f)
        {
            float b = lightDepTex.Sample(smp, input.uv * 4.0f);
            b = pow(b, 100);
            return float4(b, b, b, 1);
        }
        //if (input.uv.x < 0.25f && input.uv.y < 0.25f)
        //{
        //    float bright = depTex.Sample(smp, input.uv * 4.0f);
        //    bright = pow(abs(bright), 100.0f);
        //    return float4(bright, bright, bright, 1.0f);
        //}
        if (input.uv.x < 0.25f && input.uv.y < 0.5f)
        {
            float4 normal = normalTex.Sample(smp, input.uv * 4.0f - float2(0, 2.0f));
            return normal;
        }
        if (input.uv.x < 0.25f && input.uv.y < 0.75f)
        {
            float4 hdr = hdrTex.Sample(smp, input.uv * 4.0f - float2(0, 3.0f));
            return hdr;
        }
        if (input.uv.x < 0.125f && input.uv.y < 1.0f)
        {
            float4 shrink = hdrShrinkTex.Sample(smp, input.uv * (8.0f * float2(1, 0.5f)));
            return shrink;
        }
    //if (input.uv.x < 0.25f && input.uv.y < 1.0f)
    //{
    //    float ssao = ssaoTex.Sample(smp, input.uv * 4.0f - float2(0, 3.0f));
    //    return float4(ssao, ssao, ssao, 1);
    //}
    }
    //return DistortionByNormalTex(clickPos);
    
	// ���v�V�f�B�A���t�B���^�֊s��
    float3 normal = normalTex.Sample(smp, input.uv).rgb;
	//return float4(normal, 1);
    normal *= 4.0;
    normal -= normalTex.Sample(smp, input.uv + float2(0, dt.y)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(0, -dt.y)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(dt.x, 0)).rgb;
    normal -= normalTex.Sample(smp, input.uv + float2(-dt.x, 0)).rgb;
    float outline = outLineN ? step((normal.r * normal.g * normal.b), 0.001f) : 1.0f;
    
    float dOutline = depTex.Sample(smp, input.uv);
    dOutline *= 4.0;
    dOutline -= depTex.Sample(smp, input.uv + float2(0, dt.y));
    dOutline -= depTex.Sample(smp, input.uv + float2(0, -dt.y));
    dOutline -= depTex.Sample(smp, input.uv + float2(dt.x, 0));
    dOutline -= depTex.Sample(smp, input.uv + float2(-dt.x, 0));
    dOutline = outLineD ?saturate(step(dOutline, 0.001f)):1.0f;
    float bright = saturate(dOutline * outline);
	// return float4(bright, bright, bright, 1);
	
    float4 org = rtvTex.Sample(smp, input.uv);
    // �u���[��
    if (bloomActive)
    {
    
        org += Blur5x5(hdrTex, input.uv, float2(1.0f, 1.0f), float4(0.0f, 0.0f, 1.0f, 1.0f))* float4(bloomCol, 1);
        float2 uvRate = float2(1.0f, 0.5f);
        float2 uvOffset = float2(0.0f, 0.0f);
        for (int i = 0; i < 7; ++i)
        {
            org += Blur5x5(hdrShrinkTex, uvOffset + input.uv * uvRate, float2(1.0f, 0.5f), float4(uvOffset, uvRate))* float4(bloomCol, 1);
            uvOffset.y += uvRate.y;
            uvRate *= 0.5f;
        }
    }
    
    float depForDof = depTex.Sample(smp, clickPos);
    
	// �f�B�t�@�[�h�����_�����O�p
    //float3 nml = (normalTex.Sample(smp, input.uv).rgb * 2.0f - 1.0f);
    //float3 light = normalize(float3(-1, 1, 1));
    //float b = dot(nml, light);
    
    float ssao = ssaoActive? ssaoTex.Sample(smp, input.uv):1.0f;
    if (org.a > 0.1f)
    {
        float depDiff = distance(depForDof, depTex.Sample(smp, input.uv));
        if (depDiff < 0.001f || !dofActive)
        {
            return float4(org.rgb * ssao * bright, org.a);
        }
        else
        {
            const int blendNum = 8;
            float blendRate[blendNum] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
            
            float t = saturate(depDiff * 200.0f) * blendNum;
            uint no = 0;
            t = modf(t, no);
            
            if (no > 0)
            {
                blendRate[no] = t;
                blendRate[no - 1] = (1.0f - t);
            }
            else
            {
                blendRate[0] = 1.0f;
            }
        
            float2 uvRate = float2(1.0f, 0.5f);
            float2 uvOffset = float2(0.0f, 0.0f);
            float4 dof = org * blendRate[0];
            dof += Blur5x5(rtvTex, input.uv, float2(1.0f, 1.0f), float4(0.0f, 0.0f, 1.0f, 1.0f)) * blendRate[1];
            for (int i = 0; i < 6; ++i)
            {
                dof += Blur5x5(dofShrinkTex, uvOffset + input.uv * uvRate, float2(1.0f, 0.5f), float4(uvOffset, uvRate)) * blendRate[i + 2];
                uvOffset.y += uvRate.y;
                uvRate *= 0.5f;
            }
            return dof;
        }
    }
	else
	{
        //// 2D���C�}�[�`���O
        //float2 aspect = float2(w / h, 1);

        //float div = 10.0f;
        //float sdf = SDFLatticeCircle2D(input.uv, aspect, div);
        ////sdf = length(fmod(input.uv * aspect, 0.1)) - 0.1f;
        //if (sdf < 0)
        //{
        //    //sdf = abs(sdf) * (1.0 / div);
        //    return float4(1, 1, 1, 1);
        //}
        //else
        //{
        //    return float4(0.5, 0.5, 1, 1);
        //}
        
        // 3D���C�}�[�`���O
        const float epsilon = 0.001f;
        float2 aspect = float2(w / h, 1);
        float3 eye = float3(0, 0, -5.0f); //���_
        float3 tpos = float3((input.uv * float2(2, -2) + float2(-1, 1)) * aspect, 0);
        float3 center = float3(0, 0, 5);
        float3 ray = normalize(tpos - eye); //���C�x�N�g��(���̃V�F�[�_���ł͈�x�v�Z������Œ�)
       
        const uint count = 64;
        for (int i = 0; i < count; ++i)
        {
            float3 normal = 1.0f;
            //float len = length(fmod(eye - float3(1, 1, 10), rsph * 2) - rsph) - rsph / 2;
            //float3 q = fmod(eye + 0.5f * ray, ray) - 0.5f * ray;
            //float t = SDFLatticeSphere(abs(eye), 0.25f, 1.0f, normal);
            //float t = SDFRoundBox2(eye, float3(0.2,0.2,0.2), 0.1);
            float t = SDFLatticeRoundBox(eye,0.2f, 0.1f, 2.0f);
            if (t <= epsilon)
            {
                  //float(count - i) / float(count);
              // return float4(1, 1, 1, 1);
                //float3 nRay = float3(-1.0f, 0.0f, 0.0f);
                float3 nRay = float3(-1.0f, 1.0f, -1.0f);
                normal = NormalRoundBox(eye, 0.2f, 0.1f, epsilon);
                float b = saturate(dot(normal, normalize(nRay))/* + 0.1f*/);
            
                float3 harfV = -ray + normalize(nRay);
                harfV = normalize(harfV);
                float3 mcol = float3(1, 1, 0);
                mcol = saturate(b * mcol);
                mcol += saturate(pow(saturate(dot(harfV, normal)), 30));
                
                return float4(mcol, 1);
                //return float4(float3(b,b,b), 1);
                
            }
            eye += ray * t;
        }
        return float4(0.0, 0.0, 0, 0);
        
        
        //return float4(0, 0, 0, 0);
		//return float4(input.uv, 1, 1);
       // float div = 1.0f / 128.0f;
        //float2 aspect = float2(1.0f, w / h);
		//float2 iuv = (input.uv * 0.2) + float2(0.4,0.2);	// �g��
        //float4 dest = float4(fmod(input.uv / aspect, div) / div, 1, 1);
    }

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
	//return col;
}