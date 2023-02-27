#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	row_major float4x4	Matrix : WORLD;

	float4		vColor : TEXCOORD1;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vChangeColor : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;
	// matrix			TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector vPosition = mul(float4(In.vPosition, 1.f), In.Matrix);
	Out.vPosition = mul(vPosition, matWVP);

	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vChangeColor : TEXCOORD1;
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
	//
	// Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	//
	// if (Out.vColor.a < 0.1f)
	// 	discard;
	//
	// Out.vColor.a = saturate(1.5f - g_float_0);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default //0
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}