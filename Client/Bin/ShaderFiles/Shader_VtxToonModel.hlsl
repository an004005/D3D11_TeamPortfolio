#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"
#include "Shader_InGame.h"

Texture2D g_WaveTile;
Texture2D g_scl_noise_030;

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
	float4		vWorldPos : TEXCOORD2;
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
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

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
	float4		vAMB : SV_TARGET3;
	float4		vCTL : SV_TARGET4;
	float4		vOutline : SV_TARGET5;
	float4		vFlag : SV_TARGET6;
};

struct PS_OUT_ALPHA
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

// g_vec4_0 : 아웃라인 rgb : 컬러, a : 두께

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float flags = SHADER_NONE_SHADE;

	Out.vDiffuse = (float4)1.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	// Out.vFlag = flags;

	return Out;
}

float4 NormalPacking(PS_IN In)
{
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

	return vector(vNormal * 0.5f + 0.5f, 0.f);
}

// g_int_0 : 칼 색 플래그
// g_float_0 : 앰비언트 비율
// g_float_1 : 아직 사용하지 않음
// g_float_2 : 텔레포트 디솔브
// g_vec4_0 : 아웃라인 색(rbg) 및 두께(a)
PS_OUT PS_TOON_DEFAULT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float fTeleportDissolve = g_float_2;
	if (fTeleportDissolve > 0.f)
	{
		if (saturate(g_scl_noise_030.Sample(LinearSampler, In.vTexUV * 2.f).g + 0.2f) <= fTeleportDissolve)
			discard;
	}

	float flags = SHADER_TOON;
	int iColorFlag = g_int_0;
	float fEmissive = 0.f;

	if (iColorFlag == 1) // 보라(기본)
	{
		flags = SHADER_NONE_SHADE;
		Out.vDiffuse = float4(COL_PURPLE, 1.f);
		fEmissive = 4.f;
	}
	else if (iColorFlag == 2) // 불
	{
		flags = SHADER_NONE_SHADE;
		Out.vDiffuse = float4(COL_FIRE_DEBUF, 1.f);
		fEmissive = 4.f;
	}
	else if (iColorFlag == 3) // 전기
	{
		flags = SHADER_NONE_SHADE;
		Out.vDiffuse = float4(COL_ELEC, 1.f);
		fEmissive = 4.f;
	}
	else
	{
		Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
		if (Out.vDiffuse.a < 0.001f)
			Out.vDiffuse.a = 1.f;

		Out.vNormal = NormalPacking(In);

		if (g_tex_on_2)
			Out.vAMB = g_tex_2.Sample(LinearSampler, In.vTexUV);
		else
			Out.vAMB = Out.vDiffuse * g_float_0;

		Out.vCTL = g_tex_3.Sample(LinearSampler, In.vTexUV);
	}


	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, fEmissive, flags);
	Out.vOutline = g_vec4_0;
	Out.vFlag = float4(0.f, SHADER_POST_TOON, SHADER_TOON_GRAY_INGNORE, fTeleportDissolve);

	return Out;
}

// g_float_1 : 무기 삭제 디솔브
PS_OUT PS_CH0300_WEAPON(PS_IN In)
{
	float fNoise = g_tex_4.Sample(LinearSampler, TilingAndOffset(In.vTexUV, 0.5f, 0.f)).r;
	if (g_float_1 < fNoise)
		discard;

	PS_OUT			Out = PS_TOON_DEFAULT(In);


	return Out;
}

technique11 DefaultTechnique
{
	//0
	pass Default
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

	//1
	pass ToonDefault
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TOON_DEFAULT();
	}

	//2
	pass ch0300_Weapon
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CH0300_WEAPON();
	}

}


