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
	float4		vWorldPos : TEXCOORD2;
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
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);

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
	float4		vWorldPos : TEXCOORD2;
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

struct PS_OUT_ALPHABLEND
{
	float4		vDiffuse : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float flags = SHADER_DEFAULT;

	Out.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);

	return Out;
}

PS_OUT CommonProcess(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	/*if (Out.vDiffuse.a < 0.01f)
		discard;*/

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
	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);
	return Out;
}

PS_OUT CommonProcess_AlphaTest(PS_IN In)
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
	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);
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

	if (g_tex_on_1 && g_tex_on_3)
	{
		vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;

		vector		vDetailNormalDesc = g_tex_3.Sample(LinearSampler, In.vTexUV);
		float3 vDetialNormal = vDetailNormalDesc.xyz * 2.f - 1.f;

		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
		vDetialNormal = normalize(mul(vDetialNormal, WorldMatrix));

		vNormal += vDetialNormal * g_float_0;
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

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);

	if (g_tex_on_2)
		Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	else
		Out.vRMA = float4(1.f, 0.f, 1.f, 0.f);

	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);
	return Out;
}

// g_tex_0 : diffuse
// g_tex_1 : normal
// g_float_0 : RMA.r
// g_float_1 : RMA.g
// g_float_2 : RMA.b


PS_OUT PS_SANKOKU_A3(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);

	if (Out.vDiffuse.a < 0.01f)
		discard;

	float3 vNormal;

	if (g_tex_on_1 && g_tex_on_3)
	{
		vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;

		vector		vDetailNormalDesc = g_tex_3.Sample(LinearSampler, In.vTexUV);
		float3 vDetialNormal = vDetailNormalDesc.xyz * 2.f - 1.f;

		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vNormal = normalize(mul(vNormal, WorldMatrix));
		vDetialNormal = normalize(mul(vDetialNormal, WorldMatrix));

		vNormal += vDetialNormal * g_float_0;
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

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	
	Out.vRMA = float4(g_float_1, g_float_2, g_float_3, 0.f);

	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);
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
PS_OUT PS_ALPHA(PS_IN In)
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

	if (g_tex_on_2)
		Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	else
		Out.vRMA = float4(1.f, 0.f, 1.f, 0.f);

	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);
	return Out;
}

PS_OUT PS_DISCARD(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	discard;

	return Out;
}

// g_vec2_0 : scale uv
// g_float_0 : rotate uv radian
// g_float_1 : scroll u
PS_OUT_ALPHABLEND PS_RED_LINE_TOWER_6(PS_IN In)
{
	PS_OUT_ALPHABLEND			Out = (PS_OUT_ALPHABLEND)0;
	if (In.vTexUV.x < 0.15f || In.vTexUV.x > 0.85f)
		discard;

	float2 vUV = TilingAndOffset(rotateUV_Radian(In.vTexUV, g_float_0), g_vec2_0, float2(g_float_1 * g_Time, 0.f));
	float4 vLineColor = g_tex_0.Sample(LinearSampler, vUV);
	if (vLineColor.a < 0.01f)
		discard;

	Out.vDiffuse = lerp(float4(1.f, 0.f, 0.f, 0.f), 1.f, vLineColor.a * 0.05f);
	Out.vDiffuse.rgb = Out.vDiffuse.rgb * 2.5f;
	Out.vDiffuse.a = vLineColor.a;

	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);

	return Out;
}

PS_OUT PS_DEFAULT_ALPHATEST_7(PS_IN In)
{
	PS_OUT Out = CommonProcess_AlphaTest(In);

	if (g_tex_on_2)
		Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	else
		Out.vRMA = float4(1.f, 0.f, 1.f, 0.f);

	return Out;
}

PS_OUT PS_BRAIN_FIELD_FAR_BUILDING_8(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (Out.vDiffuse.a < 0.01f)
		discard;

	Out.vDiffuse.rgb = 1.f * g_tex_1.Sample(LinearSampler, In.vTexUV * 5.f) * 3.f;

	float flags = SHADER_NONE_SHADE;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);

	return Out;
}

PS_OUT_ALPHABLEND PS_BRAIN_FIELD_HOLOGRAM_MONITOR_9(PS_IN In)
{
	PS_OUT_ALPHABLEND			Out = (PS_OUT_ALPHABLEND)0;
	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(In.vNormal.xyz, vViewDir.xyz, 0.05f);

	float4 vColor = lerp(float4(1.f, 0.f, 0.f, 0.6f), (float4)1.f, saturate(1.f - fFresnel) * 0.5f);
	Out.vDiffuse = CalcHDRColor(vColor, fFresnel * 2.f);

	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);

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
		PixelShader = compile ps_5_0 PS_ALPHA();
	}

	// 5
	pass Discard
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISCARD();
	}

	// 6
	pass RedLineTower_6
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RED_LINE_TOWER_6();
	}

	// 7
	pass DefaultAlphaTest_7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT_ALPHATEST_7();
	}

	// 8
	pass BrainFieldFarBuilding_8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAIN_FIELD_FAR_BUILDING_8();
	}

	// 9
	pass BrainFieldHologramMonitor_9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAIN_FIELD_HOLOGRAM_MONITOR_9();
	}

	// 10 
	pass SANKOKU_A3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SANKOKU_A3();
	}
}
