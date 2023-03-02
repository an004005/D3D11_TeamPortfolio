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

// g_tex_0 : 원 마스크 텍스쳐
// g_vec4_0 : 테스트용 컬러
PS_OUT_Flag PS_DISTORTION(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.r);

	Out.vColor = 0;
	// Out.vColor.a = 1 - Out.vColor.r;
	

	
	return Out;
}

// g_tex_0 : Default White
// g_vec4_0 : Origin Color

PS_OUT_Flag PS_DISTORTION_FLIPBOOK(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV); // Not Use Plz Fix
	//////
	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.1, 4, 4);
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;
	/////
	// float4 OriginColor = g_vec4_0;
	// float4 BlendColor = Default_White * OriginColor * 2.0f;
	// float4 FinalColor = saturate(BlendColor);

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);

	Out.vColor = 0.f; 
	// Out.vColor.a = 0.f;

	// Out.vColor.a = 1 - Out.vColor.r;



	return Out;
}

// g_tex_0 : Default White
// g_vec4_0 : OriginColor
// g_float_0 : Emissive
PS_OUT_Flag PS_SCIFI(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y * 10.f));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = defaultColor * OriginColor;
	BlendColor.a = 1.f;
	Out.vColor = CalcHDRColor(g_vec4_0, g_float_0);
	Out.vFlag = float4(0.f, SHADER_SCIFI, 0.f, Out.vColor.r);
	
	return Out;
}

// g_tex_0 : Default White
// g_vec4_0 : OriginColor
// g_float_0 : Emissive

PS_OUT_Flag PS_MASK_TEX(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	BlendColor.a = Mask * g_float_1;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

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
	pass Distortion
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	}
	//2
	pass Scifi
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SCIFI();
	}

	//3
	pass MaskTex
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASK_TEX();
	}

	//4
	pass DistortionFlipBook
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_FLIPBOOK();
	}
}
