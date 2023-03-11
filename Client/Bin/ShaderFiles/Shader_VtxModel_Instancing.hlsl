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
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition.xyz, 1.f), In.Matrix);
	vector		vNormal = mul(float4(In.vNormal.xyz, 0.f), In.Matrix);
	vector		vTangent = mul(float4(In.vTangent.xyz, 0.f), In.Matrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vRMA : SV_TARGET3;
	float4		vOutline : SV_TARGET4;
	float4		vFlag : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float flags = SHADER_DEFAULT;

	Out.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	// Out.vFlag = flags;

	return Out;
}

PS_OUT CommonProcess(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (Out.vDiffuse.a < 0.01f)
		discard;

	float3 vNormal;
	if (g_tex_on_1)
	{
		vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;
		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
	}
	else
		vNormal = In.vNormal.xyz;

	float flags = SHADER_DEFAULT;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	// Out.vFlag = flags;
	return Out;
}

// g_tex_0 : diffuse
// g_tex_1 : normal
// g_tex_2 : RMA
PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT Out = CommonProcess(In);

	if (g_tex_on_2)
		Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	else
		Out.vRMA = float4(1.f, 0.f, 1.f, 0.f);

	return Out;
}

// g_tex_0 : diffuse
// g_tex_1 : normal
// g_tex_2 : RMA
// g_tex_3 : detailnormal

PS_OUT PS_DETAIL_N(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (Out.vDiffuse.a < 0.01f)
		discard;

	float3 vNormal;
	float3 vDetialNormal;

	if (g_tex_on_1 && g_tex_on_3)
	{
		vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;

		vector		vDetailNormalDesc = g_tex_3.Sample(LinearSampler, In.vTexUV);
		vDetialNormal = vDetailNormalDesc.xyz * 2.f - 1.f;

		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
		vDetialNormal = normalize(mul(vDetialNormal, WorldMatrix));
	}
	else if (g_tex_on_1)
	{
		vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;

		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
	}
	else
		vNormal = In.vNormal.xyz;


	float flags = SHADER_DEFAULT;

	if (g_tex_on_3)
	{
		vNormal += vDetialNormal * g_float_0;
	}
	else
	{
		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	}
	
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	// Out.vFlag = flags;
	return Out;
}

// g_tex_0 : diffuse
// g_tex_1 : normal
// g_tex_2 : RMA
// g_tex_3 : emissive

PS_OUT PS_EMISSIVE(PS_IN In)
{
	PS_OUT			Out = PS_DEFAULT(In);

	Out.vDepth.z = g_tex_3.Sample(LinearSampler, In.vTexUV).r;

	return Out;
}

// g_tex_0 : diffuse
// g_tex_1 : normal
// g_tex_2 : RMA
PS_OUT PS__ALPHA(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (Out.vDiffuse.a < 0.01f)
		Out.vDiffuse.a = 1.f;

	float3 vNormal;
	if (g_tex_on_1)
	{
		vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;
		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
	}
	else
		vNormal = In.vNormal.xyz;

	float flags = SHADER_DEFAULT;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	// Out.vFlag = flags;
	return Out;
}

technique11 DefaultTechnique
{
	// 0
	pass Empty
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

	// 1
	pass DefaultModel
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT();
	}

	// 2
	pass Detail_N
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DETAIL_N();
	}

	// 3
	pass Emissive
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EMISSIVE();
	}

	// 4
	pass Alpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS__ALPHA();
	}
}