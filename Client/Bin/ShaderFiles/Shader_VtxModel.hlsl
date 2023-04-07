#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"

Texture2D g_KineticWave;
Texture2D g_scl_noise_004;

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

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
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
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vRMA : SV_TARGET3;
	float4		vOutline : SV_TARGET4;
	float4		vFlag : SV_TARGET5;
};

struct PS_OUT_ALPHABLEND
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float flags = SHADER_DEFAULT;

	Out.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	Out.vFlag = 0.f;

	return Out;
}

PS_OUT CommonProcess(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vDiffuse.rgb = g_tex_0.Sample(LinearSampler, In.vTexUV).rgb;
	// if (Out.vDiffuse.a < 0.01f)
	// 	discard;

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

PS_OUT CommonProcess_NoneAlphaTest(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vDiffuse.rgb = g_tex_0.Sample(LinearSampler, In.vTexUV).rgb;
	// if (Out.vDiffuse.a < 0.01f)
		// discard;

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

// g_int_0 : 아웃라인 on, off
// g_float_0 : 염력 캐치
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
// g_tex_2 : roughness
PS_OUT PS_DEFAULT_ROUGHNESS(PS_IN In)
{
	PS_OUT			Out = CommonProcess(In);
	return Out;
}

PS_OUT PS_WIRE_FRAME(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = vector(1.f, 0.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_PSYCHIC_DEFAULT_4(PS_IN In)
{
	float fDissolveDisappear = g_float_1; // 0 ~ 1 로 사라지기
	if (fDissolveDisappear > 0.f)
	{
		float fNoise = g_scl_noise_004.Sample(LinearSampler, In.vTexUV * 2.f).r;
		if (fDissolveDisappear >= fNoise)
			discard;
	}

	PS_OUT Out = CommonProcess_NoneAlphaTest(In);

	// 알파값 없던 텍스쳐 알파 무시하는 부분
	//if (Out.vDiffuse.a == 0.f)
	// ~알파값 없던 텍스쳐 알파 무시하는 부분

	if (g_tex_on_2)
		Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	else
		Out.vRMA = float4(1.f, 0.f, 1.f, 0.f);

	int iOutlineOn = g_int_0;
	float fPsychic = g_float_0;
	if (iOutlineOn)
		Out.vOutline = (float4)1.f;
	else
		Out.vOutline = (float4)0.f;
	
	if (fPsychic > 0.f)
	{
		if (fPsychic >= 1.f)
		{
			float fWave = g_KineticWave.Sample(LinearSampler, TilingAndOffset(In.vTexUV, 1.f, float2(g_Time * 0.25f, 0.f))).r;
			Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, COL_PURPLE, fWave);
		}
		else
		{
			Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, COL_PURPLE, 0.5f);
			Out.vDepth.z = 1.f - fPsychic;
		}
	}

	Out.vDiffuse.a = 1.f;

	return Out;
}

PS_OUT_ALPHABLEND PS_GLASS_5(PS_IN In)
{
	PS_OUT_ALPHABLEND			Out = (PS_OUT_ALPHABLEND)0;

	Out.vColor.rgb = COL_WHITE;
	Out.vColor.a = 0.1f;

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
	pass RoughnessModel
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT_ROUGHNESS();
	}

	// 3
	pass WireFrameModel
	{
		SetRasterizerState(RS_Wireframe);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WIRE_FRAME();
	}

	// 4
	pass PhychicDefault
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PSYCHIC_DEFAULT_4();
	}

	// 5
	pass DefaultGlass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GLASS_5();
	}


}
