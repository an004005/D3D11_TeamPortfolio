#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
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
	float4		vFlag : SV_TARGET1;
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
	Out.vColor = CalcHDRColor(g_vec4_0, g_float_0);

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
	return Out;
}

PS_OUT PS_DEFAULT_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV * 2.f);
	Out.vColor = CalcHDRColor(g_vec4_0, g_float_0);

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, 0.f);

	return Out;
}

// g_float_0 : 모델 텍스쳐 UV.y 값
// g_float_1 : Emissive
// g_vec4_0 : 기본 색상

// g_tex_0 : 노이즈

PS_OUT PS_MAIN_DEFAULT_ATTACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV * 2.f);

	float4 NoiseTex = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 MixNoiseTex = g_tex_1.Sample(LinearSampler, float2(In.vTexUV));

	float4 FinalMixTex = NoiseTex * MixNoiseTex;
	

	float4 OriginColor = g_vec4_0;

	float4 CalcColor = saturate(OriginColor * FinalMixTex);

	Out.vColor = CalcHDRColor(CalcColor, g_float_1);
	Out.vColor.a = g_vec4_0.a;
	// Out.vColor.a = NoiseTex.r;

	// Out.vColor.a = NoiseTex.a;
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
	pass PlayerDefaultAttack
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT_ATTACK();
	}

}
