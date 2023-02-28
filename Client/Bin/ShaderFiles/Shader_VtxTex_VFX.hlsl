#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

texture2D GrapTexture;

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

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;

};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	// Out.vColor = float4(1.f, 1.f, 1.f, 1.f);

	return Out;
}

struct PS_OUT_Flag
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

PS_OUT_Flag PS_MAIN_Flag(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	// Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	Out.vFlag = float4(1.0f, 0.f, 0.f, 0.f);

	return Out;
}


PS_OUT_Flag PS_MAIN_TEST(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.r);
	Out.vColor = 0.f;
	// Out.vColor.a = 1 - Out.vColor.r;
	

	
	return Out;
}


technique11 DefaultTechnique
{
	//0
	pass Empty
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//1
	pass Test
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEST();
	}
}
