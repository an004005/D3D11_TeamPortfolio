#include "Shader_Defines.h"
#include "Shader_Params.h"
#include "Shader_Utils.h"

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;	
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
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
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (In.vTexUV.y < 0.6f)
		discard;

	float HeightGradient = Remap(In.vTexUV.y, float2(0.6f, 1.f), float2(0.f, 1.f));
	float fBotGradient = 1.f;

	if (In.vTexUV.y > 0.85f)
	{
		fBotGradient = Remap(In.vTexUV.y, float2(0.85f, 1.f), float2(0.f, 1.f));
		fBotGradient = 1.f - fBotGradient;
	}

	float fNoise = g_tex_0.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(0.5f, 10.f), 0.f)).r;
	float fFlowMask = g_tex_1.Sample(LinearSampler, TilingAndOffset(In.vTexUV, 1.f, float2(g_Time * 0.1f, g_Time * 0.2f)));

	Out.vColor.rgb = fNoise * float3(1.f, 0.3539f , 0.f) * 20.f * HeightGradient * fBotGradient;
	Out.vColor.a = fNoise * fFlowMask * HeightGradient * fBotGradient * g_float_0;
	if (Out.vColor.a < 0.0001f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
