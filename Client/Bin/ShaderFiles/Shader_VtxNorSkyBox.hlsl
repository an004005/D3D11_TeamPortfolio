#include "Shader_Defines.h"
#include "Shader_Params.h"
#include "Shader_Utils.h"

// texture2D		g_Flow;
// float			g_Time;

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

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float4 viewPos = mul(float4(In.vPosition, 0), g_ViewMatrix);
    float4 clipSpacePos = mul(viewPos, g_ProjMatrix);

	Out.vPosition = clipSpacePos.xyww;
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
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_int_0 == 0)
	{
		Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	}
	else
	{
		Out.vColor = saturate(g_tex_0.Sample(LinearSampler, In.vTexUV) * float4(0.102f, 0.092f, 0.091, 1.f));
	}

	return Out;
}

PS_OUT PS_BRAIN_FIELD_1(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_1.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(8.f, 5.f), 0.f)) * 5.f;
	return Out;	
}

PS_OUT PS_BLACK_2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor.a = 1.f;
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

	pass BrainField
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);		
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);	

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAIN_FIELD_1();
	}

	pass Black
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);		
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);	

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACK_2();
	}
}
