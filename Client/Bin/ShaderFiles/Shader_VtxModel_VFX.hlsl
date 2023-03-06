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
// g_float_1 : UV.y 값 연동된
// g_float_2 : Emissive
// g_float_3 : Dissolve 커질수록 사라짐

// g_vec4_0 : 섞는 색상

// g_tex_0 : 반절만 나오는 텍스쳐
// g_tex_1 : 흰색 꽉찬 텍스쳐
// g_tex_2 : 디졸브 텍스쳐 

PS_OUT PS_MAIN_DEFAULT_ATTACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 BasicColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x * g_float_0, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = BasicColor * OriginColor * 2.0f;
	

	float4 AllTex = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 BlendColor2 = AllTex * OriginColor * 2.0f;
	float fDissolvePower = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	if(g_float_1 <= 0.f)
	{
		Out.vColor = CalcHDRColor(BlendColor2, g_float_2);
		Out.vColor.a = 1.f;

		Out.vFlag = float4(0.f, SHADER_SCIFI_PLAYER_ATTACK, 0.f, Out.vColor.a);

	}
	else
	{
		Out.vColor = CalcHDRColor((BlendColor *g_float_1) + (BlendColor2 * (1-g_float_1)), g_float_2);
		Out.vColor.a = (BasicColor.r * g_float_1) + (BlendColor2.r * (1 - g_float_1));
		Out.vFlag = float4(0.f, SHADER_SCIFI_PLAYER_ATTACK, 0.f, g_float_1);
	}

	if (g_float_3 >= fDissolvePower)
	{
		discard;
	}

	return Out;
}

// g_float_0 : 모델 텍스쳐 UV.y 값
// g_float_1 : UV.y 값 연동된
// g_float_2 : Emissive
// g_float_3 : Dissolve 커질수록 사라짐

// g_vec4_0 : 섞는 색상

// g_tex_0 : 반절만 나오는 텍스쳐
// g_tex_1 : 흰색 꽉찬 텍스쳐
// g_tex_2 : 디졸브 텍스쳐

PS_OUT PS_MAIN_FIRE_ATTACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 BasicColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x * g_float_0, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = BasicColor * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	// FinalColor.a = BasicColor.r;

	float4 AllTex = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x * (1-g_float_1 ), In.vTexUV.y));
	float4 BlendColor2 = AllTex * OriginColor * 2.0f;

	if(g_tex_on_4)
	{
		float2 randomNormal = g_tex_4.Sample(LinearSampler, In.vTexUV).xy;
		float2 distortionUV = randomNormal * g_float_4 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(g_Time, 0.f));

		float fDissolvePower = g_tex_2.Sample(LinearSampler, distortionUV).r;
		float fDissolveNoise = g_tex_3.Sample(LinearSampler, distortionUV).r;
		float ND = fDissolvePower * fDissolveNoise;

		if (g_float_1 <= 0.f)
		{
			Out.vColor = CalcHDRColor(BlendColor2, g_float_2);
			Out.vColor.a = AllTex.r;

			Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, 0.f);
		}
		else
		{
			Out.vColor = CalcHDRColor((FinalColor *g_float_1) + (BlendColor2 * (1 - g_float_1)), g_float_2);
			Out.vColor.a = g_float_5;
			Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
		}

		if (g_float_3 >= ND)
		{
			discard;
		}
	}
	else
	{
		float fDissolvePower = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
		float fDissolveNoise = g_tex_3.Sample(LinearSampler, In.vTexUV).r;
		float ND = fDissolvePower * fDissolveNoise;
		if (g_float_1 <= 0.f)
		{
			Out.vColor = CalcHDRColor(BlendColor2, g_float_2);
			Out.vColor.a = AllTex.r;

			Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
		}
		else
		{
			Out.vColor = CalcHDRColor((FinalColor *g_float_1) + (BlendColor2 * (1 - g_float_1)), g_float_2);
			Out.vColor.a = (BasicColor.r * g_float_1) + (AllTex * (1 - g_float_1));
			Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
		}

		if (g_float_3 >= ND)
		{
			discard;
		}
	}

	

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

	//2
	pass PlayerFireAttack
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FIRE_ATTACK();
	}
}
