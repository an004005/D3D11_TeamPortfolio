#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"
/*
* pos.xyz = 로컬 또는 월드 위치
* pos.w = 현재 life(시간)
* up.w = 총 life
* up.xy = p사이즈 xy
*
* look.xyz : 이동 방향
* look.w : 이동 속력
*/
int g_bLocal = 0;
int g_bSizeDecreaseByLife = 0;
int g_bSizeIncreaseByLife = 0;
float g_fIncreaseMaxSize = 1.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
	float4		vControlData : TEXCOORD5;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vChangeColor : TEXCOORD1;

	float		RamainLifeRatio : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matVP;
	matrix			TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	//matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matVP = mul(g_ViewMatrix, g_ProjMatrix);


	Out.RamainLifeRatio = (1.f - In.vControlData.x / In.vControlData.y);

	if (Out.RamainLifeRatio >= 1.f)
		Out.RamainLifeRatio = 1.f;

	if(g_bLocal == 1)
	{
		Out.vPosition = mul(mul(float4(In.vPosition,1.f), TransformMatrix), g_WorldMatrix);
	}
	else
	{
		Out.vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
	}

	//vector vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(Out.vPosition, matVP);

	Out.vTexUV = In.vTexUV;


	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vChangeColor : TEXCOORD1;
	float		RamainLifeRatio : TEXCOORD2;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
	// vector		vFlag : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = CalcHDRColor(g_vec4_0, g_float_0);

	Out.vColor.a = g_vec4_0.a;

	return Out;
}

PS_OUT PS_MAIN_TEX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 OriginColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vColor = OriginColor;

	// if (Out.vColor.a < 0.001f)
	// 	discard;

	return Out;
}
PS_OUT PS_BLEND_TEX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 OriginColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 BlendColor = g_vec4_0;
	float4 FinalColor = OriginColor * BlendColor * 2.0f;
	float4 CalcColor = saturate(FinalColor);

	Out.vColor = CalcColor;
	Out.vColor.a = OriginColor.a;

	return Out;
}
PS_OUT PS_BLEND_TEX_HDR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 OriginColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 BlendColor = g_vec4_0;
	float4 FinalColor = OriginColor * BlendColor * 2.0f;
	float4 CalcColor = saturate(FinalColor);
	float4 Help = g_tex_1.Sample(LinearSampler, In.vTexUV);
	
	Out.vColor = CalcHDRColor(CalcColor, g_float_0);
	Out.vColor.a = OriginColor.a;

	return Out;
}
technique11 DefaultTechnique
{
	pass Default //0
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Default_Tex //1
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX();
	}

	pass Blend_Tex //2
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLEND_TEX();
	}

	pass Blend_Tex_HDR //3
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLEND_TEX_HDR();
	}
}