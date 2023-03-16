#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

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

	// Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV * 2.f);
	Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	
	return Out;
}

// [1] TargetaArrow
// g_tex_0 : Original Texture
// g_tex_1 : Emissive Texture
// g_vec4_0 : Color
// g_float_0 : Century
// g_float_1 : ratio
// g_float_2 : Alpha
PS_OUT PS_GreenEmissive(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Emissive = g_tex_1.Sample(LinearSampler, In.vTexUV);

	float Mask = Emissive.g;

	float4 DefaultColor = float4(Emissive.g, Emissive.g, Emissive.g, 0.f);

	float4 BlendColor = DefaultColor  * g_vec4_0 * 2.0f;

	float4 FinalColor = saturate(BlendColor);

	float4 HDRColor = saturate(FinalColor + Texture * g_float_0);

	Out.vColor = CalcHDRColor(HDRColor, g_float_1);

	Out.vColor.a = Mask;

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
	pass GreenEmissive
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GreenEmissive();
	}
	
	
}
