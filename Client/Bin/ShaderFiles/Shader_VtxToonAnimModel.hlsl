#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"
#include "Shader_InGame.h"

matrix			g_BoneMatrices[512];

Texture2D g_WaveTile;
Texture2D g_scl_noise_030;
Texture2D g_scl_noise_004;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	/* 현재 정점에게 곱해져야할 뼈들(최대 4개)의 행렬을 만든다. */
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
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

	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector			vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector			vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vProjPos = Out.vPosition;
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
	float4		vAMB : SV_TARGET3;
	float4		vCTL : SV_TARGET4;
	float4		vOutline : SV_TARGET5;
	float4		vFlag : SV_TARGET6; // post process 플래그
};

struct PS_OUT_FORWARD
{
	float4		vColor : SV_TARGET0;
};

struct PS_OUT_NONLIGHT
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1; // post process 플래그
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float flags = SHADER_TOON;

	Out.vDiffuse = (float4)1.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	Out.vFlag = float4(0.f, SHADER_POST_TOON, SHADER_TOON_GRAY_INGNORE, 0.f);
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

// g_float_0 : 염력 림라이트 밝기
// g_float_1 : 하드바디 디솔브
// g_float_2 : 텔레포트 디솔브
// g_int_0 : 상태이상 플래그
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

	float fEmissive = 0.f;
	float flags = SHADER_TOON;

	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (Out.vDiffuse.a < 0.001f)
		Out.vDiffuse.a = 1.f;

	Out.vNormal = NormalPacking(In);
	if (g_tex_on_2)
		Out.vAMB = g_tex_2.Sample(LinearSampler, In.vTexUV);
	Out.vCTL = g_tex_3.Sample(LinearSampler, In.vTexUV);
	Out.vOutline = g_vec4_0;

	float fHardBody = g_float_1;
	if (fHardBody > 0.f && In.vTexUV.y < fHardBody)
	{
		float3 vNormal = Out.vNormal.xyz * 2.f - 1.f;
		float4 vViewDir = g_vCamPosition - In.vWorldPos;
		float fFresnel = FresnelEffect(vNormal.xyz, vViewDir.xyz, 2.f);
		if (fFresnel > 0.5f)
			Out.vDiffuse.rgb = lerp(COL_BLACK, (float3)1.f, 0.9f);
		else
			Out.vDiffuse.rgb = COL_BLACK;
		Out.vAMB.rgb = 0.0f;
	}

	float fPhysicRimBright = g_float_0;
	int iDebuffState = g_int_0;
	if (iDebuffState == 1 || fPhysicRimBright > 0.f) // fire
	{
		float3 vColor = (float3)1.f;
		if (iDebuffState == 1)
		{
			vColor = COL_FIRE;
			fEmissive = 2.f;
		}
		else
		{
			vColor = COL_PURPLE; // 보라
			fEmissive = fPhysicRimBright;
		}

		float3 vNormal = Out.vNormal.xyz * 2.f - 1.f;
		float4 vViewDir = g_vCamPosition - In.vWorldPos;
		float fFresnel = FresnelEffect(vNormal.xyz, vViewDir.xyz, 1.f);
		Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, vColor , fFresnel);

		fEmissive *= fFresnel;
	}
	else if (iDebuffState == 2) // oil
	{
		float3 vDefaultNormal = Out.vNormal.xyz * 2.f - 1.f;

		float4 vWaveTile = g_WaveTile.Sample(LinearSampler, TilingAndOffset(In.vTexUV, (float2)2.f, float2(0.f, -g_Time * 0.3f)));
		float3 vWetNormal = vWaveTile.xyz * 2.f - 1.f;
		float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
		vWetNormal = normalize(mul(vWetNormal, WorldMatrix));

		vWetNormal = lerp(vDefaultNormal, vWetNormal, vWaveTile.a);
		Out.vNormal = vector(vWetNormal * 0.5f + 0.5f, 0.f);

		Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, COL_OIL, vWaveTile.a);
		fEmissive = (vWaveTile.a);
	}


	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, fEmissive, flags);
	Out.vFlag = float4(0.f, SHADER_POST_TOON, SHADER_TOON_GRAY_INGNORE, fTeleportDissolve);

	return Out;
}

// g_float_0 : empty
// g_float_1 : empty
// g_float_2 : 텔레포트 디솔브
// g_vec4_0 : 아웃라인 색(rbg) 및 두께(a)
PS_OUT PS_WIRE_2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float fTeleportDissolve = g_float_2;
	if (fTeleportDissolve > 0.f)
	{
		if (saturate(g_scl_noise_030.Sample(LinearSampler, In.vTexUV * 2.f).g + 0.2f) <= fTeleportDissolve)
			discard;
	}

	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (Out.vDiffuse.a < 0.001f)
		discard;

	Out.vNormal = NormalPacking(In);
	float3 vNormal = Out.vNormal.xyz * 2.f - 1.f;

	float flags = SHADER_NONE_SHADE;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 2.f, flags);
	// Out.vAMB = g_tex_2.Sample(LinearSampler, In.vTexUV);
	// Out.vCTL = float4(0.5f, 0.8f, 1.f, 0.f);

	float4 vColor = g_vec4_1;
	Out.vDiffuse *= vColor;
	Out.vOutline = g_vec4_0;

	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(vNormal, normalize(vViewDir.xyz), 0.5f);
	float4 vWhite = float4(1.f, 1.f, 1.f, 1.f);
	Out.vDiffuse = lerp(vWhite, Out.vDiffuse, fFresnel);
	Out.vFlag = float4(0.f, SHADER_POST_TOON, SHADER_TOON_GRAY_INGNORE, fTeleportDissolve);

	return Out;
}

// g_float_0 : 염력 림라이트 밝기
// g_int_0 : 상태이상 플래그
// g_vec4_0 : 아웃라인 색(rbg) 및 두께(a)
PS_OUT PS_CH100_HAIR_1_3(PS_IN In)
{
	PS_OUT			Out = PS_TOON_DEFAULT(In);

	float3 vNormal = Out.vNormal.xyz * 2.f - 1.f;
	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(vNormal, normalize(vViewDir), 1.5f);
	const float4 vColor = float4(1.f, 26.f/255.f, 0.f, 1.f);
	Out.vDiffuse = lerp(Out.vDiffuse, vColor, fFresnel);

	return Out;
}

// g_float_0 : 염력 림라이트 밝기
// g_int_0 : 상태이상 플래그
// g_int_1 : 드라이브 모드 플래그(후드 지우기)
// g_vec4_0 : 아웃라인 색(rbg) 및 두께(a)
PS_OUT PS_ch0100_body_0_4(PS_IN In)
{
	int bDriveMode = g_int_1;
	if (bDriveMode && g_tex_on_4)
	{
		float4 vMask = g_tex_4.Sample(LinearSampler, In.vTexUV);
		if (vMask.r == 0.f && vMask.g == 0.f && vMask.b == 0.f)
			discard;
	}

	PS_OUT Out = PS_TOON_DEFAULT(In);

	return Out;
}
/*
 * 드라이브모드 절차
 * 1. hood 렌더 on
 * 2. mask g_float_3을 0~1로 증가, 0.9쯤, hair렌더 off
 * 3. 1되면 body의 g_int_1 = 1 대입
 *
 * 드라이브 끝나면 hood 렌더 off, mask g_float_3 = 0, body의 g_int_1 = 0 대입, hair 렌더 on
 */
// g_float_0 : 염력 림라이트 밝기
// g_float_1 : 하드바디
// g_float_2 : 
// g_float_3 : 마스크 on, off
// g_int_0 : 상태이상 플래그
// g_vec4_0 : 아웃라인 색(rbg) 및 두께(a)
PS_OUT PS_ch0100_mask_0_5(PS_IN In)
{
	float fDissolve = g_float_3; // 0 : mask off/ 1 : mask on
	float4 vColor = (float4)0.f;
	float fEmissive = 0.f;

	if (fDissolve > 0.f && fDissolve < 1.f)
	{
		float fInvDissolve = 1.f - fDissolve;

		float4 vSclNoi = g_tex_4.Sample(LinearSampler, In.vTexUV);
		float fShape = vSclNoi.r;

		if (fDissolve < 0.015)
			discard;

		if (fInvDissolve < fShape)
		{
			vColor = float4(0.f, 0.f, 0.f, 1.f);
			fEmissive = 0.0f;
		}
		else
		{
			float fDiff = abs(fInvDissolve - fShape);
			if (fDiff < 0.2f)
			{
				float fNoise = g_tex_4.Sample(LinearSampler, In.vTexUV).g;
				vColor = float4(62.f / 255.f, 164.f / 255.f, 147.f / 255.f, 1.f);
				fEmissive = fNoise * 12.f;
			}
			else
				discard;
		}
	}
	else if (fDissolve >= 1.f)
	{
		vColor = float4(1.f, 20.f/ 255.f, 0.f, 1.f);
		fEmissive = 7.f;
	}
	else
	{
		discard;
	}


	PS_OUT Out = PS_TOON_DEFAULT(In);

	Out.vDepth.z = fEmissive;
	if (g_float_1 <= 0.f) // 하드바디 없을 때
		Out.vDepth.w = SHADER_NONE_SHADE;

	if (fDissolve >= 1.f)
		Out.vDiffuse *= vColor;
	else
		Out.vDiffuse = vColor;

	return Out;
}

PS_OUT_FORWARD PS_ToonDefault_Forward_6(PS_IN In)
{
	PS_OUT_FORWARD Out = (PS_OUT_FORWARD)0;

	float4 vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 vNormal = NormalPacking(In);
	vNormal.xyz = vNormal.xyz * 2.f - 1.f;

	float4 vAMB = 0.f;
	if (g_tex_on_2)
		vAMB = g_tex_2.Sample(LinearSampler, In.vTexUV);
	float4 vCTL = g_tex_3.Sample(LinearSampler, In.vTexUV);

	float3 vLightDir = float3(1.f, -1.f, 1.f);
	float4 vLightColor = float4(1.f, 1.f, 1.f, 1.f);

	float fNdotL = dot(normalize(vNormal.xyz), normalize(vLightDir.xyz));
	float fDiff = saturate(max(fNdotL, 0.0));
	fDiff = max(vCTL.r * 2.f , min(vCTL.g, fDiff));
	fDiff *= vCTL.b;
	float4 vShade = vLightColor * saturate(fDiff);

	Out.vColor = saturate(vDiffuse * vShade + vAMB * 0.5f);
	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT_FORWARD PS_WIRE_Forward_7(PS_IN In)
{
	PS_OUT_FORWARD			Out = (PS_OUT_FORWARD)0;

	float4 vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (vDiffuse.a < 0.001f)
		discard;

	float4 vNormal = NormalPacking(In);
	vNormal.xyz = vNormal.xyz * 2.f - 1.f;

	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(vNormal.xyz, normalize(vViewDir.xyz), 0.5f);
	Out.vColor.rgb = lerp(COL_WHITE, COL_FIRE, fFresnel);
	Out.vColor.a = 1.f;
	return Out;
}

PS_OUT_NONLIGHT PS_SuperSpeedTrail_8(PS_IN In)
{
	PS_OUT_NONLIGHT			Out = (PS_OUT_NONLIGHT)0;

	float fNoise = g_scl_noise_004.Sample(LinearSampler, In.vTexUV * 2.f).r;
	float fLifeRatio = saturate(g_float_0);
	if (fLifeRatio <= 0.5f)
	{
		float fRemapRatio = Remap(fLifeRatio, float2(0.f, 0.5f), float2(0.f, 1.f));
		if (fRemapRatio< fNoise)
			discard;
	}

	Out.vColor.rgb = COL_PINK;
	Out.vColor.a = 0.3f;

	return Out;
}

// g_float_0 : 케이블 삭제 디솔브 (0 디폴트, 1 사라짐)
// g_float_1 : sas 붉은 색 디솔브
// g_float_2 : 텔레포트 디솔브
// g_vec4_0 : 아웃라인 색(rbg) 및 두께(a)
PS_OUT PS_SAS_CABLE_9(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float fTeleportDissolve = g_float_2;
	if (fTeleportDissolve > 0.f)
	{
		if (saturate(g_scl_noise_030.Sample(LinearSampler, In.vTexUV * 2.f).g + 0.2f) <= fTeleportDissolve)
			discard;
	}

	float fDissolve = g_float_0;
	float fNoiseDissolve = g_tex_4.Sample(LinearSampler, In.vTexUV * 2.f).r;
	if (fNoiseDissolve < fDissolve)
		discard;


	float fEmissive = 0.f;
	float flags = SHADER_TOON;

	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vNormal = NormalPacking(In);
	Out.vAMB = g_tex_2.Sample(LinearSampler, In.vTexUV);
	Out.vCTL = g_tex_3.Sample(LinearSampler, In.vTexUV);
	Out.vOutline = g_vec4_0;


	float fRedDissolve = g_float_1;
	// if (In.vTexUV.y <= fRedDissolve)
	// 	fRedDissolve = 1.f;
	// else
	// 	fRedDissolve = 0.f;

	if (Out.vCTL.a <= 0.001f)
	{
		Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, float3(0.8f, 0.35f, 0.f), fRedDissolve);
		fEmissive += 1.f;
	}
	else
		Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, COL_CABLE_RED, fRedDissolve);



	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, fEmissive, flags);
	Out.vFlag = float4(0.f, SHADER_POST_TOON, SHADER_TOON_GRAY_INGNORE, fTeleportDissolve);

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
	pass Wire_2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WIRE_2();
	}

	//3
	pass ch100HairBridge
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CH100_HAIR_1_3();
	}

	// 4 
	pass ch100Body
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ch0100_body_0_4();
	}

	// 5
	pass PS_ch0100_mask_0_5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ch0100_mask_0_5();
	}

	// 6
	pass PS_ToonDefaultForward
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ToonDefault_Forward_6();
	}

	// 7
	pass PS_ToonWireForward
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WIRE_Forward_7();
	}

	// 8
	pass SuperSpeedTrail_8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SuperSpeedTrail_8();
	}

	// 9
	pass SAS_Cable_8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_CABLE_9();
	}

	// 10
	pass DefaultNonCurl
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TOON_DEFAULT();
	}
}


