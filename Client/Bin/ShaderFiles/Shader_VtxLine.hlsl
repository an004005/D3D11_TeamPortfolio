#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"

float4 g_color = float4(1.f, 1.f, 1.f, 1.f);

struct VS_IN
{
	float3		vPosition : POSITION;
	float4		vColor : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vColor : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	Out.vPosition = mul(float4(In.vPosition, 1.f), matVP);
	Out.vColor = In.vColor;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vColor : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = In.vColor;
	return Out;
}

technique11 DefaultTechnique
{
	pass Line
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
