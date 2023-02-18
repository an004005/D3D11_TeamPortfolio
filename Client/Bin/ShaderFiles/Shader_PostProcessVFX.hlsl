#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"

texture2D		g_LDRTexture;
texture2D		g_NormalTexture; 
texture2D		g_DepthTexture;
texture2D		g_DepthMaintainTexture;

texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;

struct VS_IN
{
	float3		vPosition : POSITION;	
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);	

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
	
	return Out;
}


PS_OUT PS_MAIN_WHITE_OUT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float WhiteRatio = saturate(g_float_0);
	float4 vColor =  (float4)1.f;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV) * (1.f - WhiteRatio) + vColor * WhiteRatio;
	return Out;
}


// https://www.shadertoy.com/view/XsfSDs
// g_vec2_0 : 블러 센터 UV값
PS_OUT PS_MAIN_BLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	const int nsamples = 10;
    float2 vBlurCenterUV = g_vec2_0;

	float blurStart = 1.0;
    float blurWidth = 0.15;
    
	float2 uv = In.vTexUV;
    
    uv -= vBlurCenterUV;
    float precompute = blurWidth * (1.0 / float(nsamples - 1));
    
    float4 color = (float4)0;
    for(int i = 0; i < nsamples; i++)
    {
        float scale = blurStart + (float(i)* precompute);
        color += g_LDRTexture.Sample(LinearSampler, uv * scale + vBlurCenterUV);
    }
    
    color /= float(nsamples);
    
	Out.vColor = color;

	return Out;
}

PS_OUT PS_MAIN_XRAY_VISION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	float4 depth = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float4 depthMaintain = g_DepthMaintainTexture.Sample(LinearSampler, In.vTexUV);

	if (depthMaintain.y == 1.f)
		return Out;

	if (depth.y >= depthMaintain.y) // Maintain보다 깊이가 크다 : Maintain이 가려지지 않았다.
		return Out;

	// g_TeamXRayIdx 체크
	if (depth.w == 4.f) // 비쳐 보이는 대상 오브젝트의 ID가 4(맵오브젝트)일 때만 비쳐보인다.
	{
		if (depthMaintain.w == 3.f) // 적군(빨강)
			Out.vColor = float4(241.f / 255.f, 18.f / 255.f, 12.f / 255.f, 1.f);
		else if (depthMaintain.w == 2.f) // 아군(초록)
			Out.vColor = float4(71.f / 255.f, 1.f, 189.f / 255.f, 1.f);
	}


	return Out;
}


PS_OUT PS_MAIN_RADIAL_MASK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float fRadius = g_float_0;
	float3 vPos = g_vec4_0.xyz;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * 300.f;

	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;
	vWorldPos *= fViewZ;

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	if (length(vPos - vWorldPos.xyz) <= fRadius)
	{
		Out.vColor *= 0.3f;
	}

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	// 1
	pass FlashOut
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WHITE_OUT();
	}

	// 2
	pass SimpleBlur
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	// 3
	pass XRayVision
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_XRAY_VISION();
	}

	// 4
	pass RadialMask
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RADIAL_MASK();
	}

}
