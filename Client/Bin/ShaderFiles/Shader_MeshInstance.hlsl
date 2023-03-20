#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"
/*
* pos.xyz = 로컬 또는 월드 위치
* pos.w = 현재 life(시간)
* up.w = 총 life
* up.xy = p사이즈 xy
*
* look.xyz : 이동 방향
* look.w : 이동 속력
*/
int g_bLocal = 0;
int g_bSizeDecreaseByLife = 0;
int g_bSizeIncreaseByLife = 0;
float g_fIncreaseMaxSize = 1.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	row_major float4x4	Matrix : WORLD;
	float4		vControlData : TEXCOORD1;

};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	//float4		vChangeColor : TEXCOORD1;

	float		RamainLifeRatio : TEXCOORD1;
};

struct VS_OUT_NORM
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;

	float		RamainLifeRatio : TEXCOORD2;
};

struct PS_IN_NORM
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;

	float		RamainLifeRatio : TEXCOORD2;

};

struct PS_OUT_NORM
{
	float4		vColor : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vRMA : SV_TARGET3;
	float4		vOutline : SV_TARGET4;
	float4		vFlag : SV_TARGET5;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matVP;
	matrix			TransformMatrix = In.Matrix;

	//matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	
	Out.RamainLifeRatio = saturate((In.vControlData.y - In.vControlData.x )/ In.vControlData.y);

	if(g_bLocal == 1)
	{
		Out.vPosition = mul(mul(float4(In.vPosition,1.f), TransformMatrix), g_WorldMatrix);
	}
	else
	{
		Out.vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
	}

	//vector vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(Out.vPosition, matVP);

	Out.vTexUV = In.vTexUV;


	return Out;
}

VS_OUT_NORM VS_MAIN_NORM(VS_IN In)
{
	VS_OUT_NORM		Out = (VS_OUT_NORM)0;

	matrix			matWV, matVP;
	matrix			TransformMatrix = In.Matrix;
	// matrix			TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out.RamainLifeRatio = saturate((In.vControlData.y - In.vControlData.x )/ In.vControlData.y);

	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	if (g_bLocal == 1)
	{
		Out.vPosition = mul(mul(float4(In.vPosition, 1.f), TransformMatrix), g_WorldMatrix);
	}
	else
	{
		Out.vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
	}

	Out.vPosition = mul(Out.vPosition, matVP);

	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	//float4		vChangeColor : TEXCOORD1;
	float		RamainLifeRatio : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
	// vector		vFlag : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	
	// 수명 던지기
	Out.vColor = CalcHDRColor(g_vec4_0, g_float_0 * In.RamainLifeRatio);

	Out.vColor.a = g_vec4_0.a;

	return Out;
}

PS_OUT_NORM PS_PLAYER_KINETIC_PARTICLE(PS_IN_NORM In)
{
	PS_OUT_NORM		Out = (PS_OUT_NORM)0;
	float flags = SHADER_DEFAULT;

	float3 vNormal = In.vNormal.xyz;

	float4 BaseTex = g_tex_0.Sample(LinearSampler, In.vTexUV) * 0.1f;
	float4 Color = g_vec4_0;
	float4 BlendColor = BaseTex * Color ;

	Out.vColor = saturate(Color);

	vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
	vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, g_float_0 * In.RamainLifeRatio, flags);

	// Out.vColor.a = g_vec4_0.a;

	Out.vColor.a = 1.f;
	Out.vRMA = float4(g_float_1, g_float_2, g_float_3, 0.f);

	return Out;
}

PS_OUT PS_MAIN_TEX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 OriginColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vColor = OriginColor;
	// Out.vColor.a = 1.f;
	// if (Out.vColor.a < 0.001f)
	// 	discard;

	return Out;
}
PS_OUT PS_BLEND_TEX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 OriginColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 BlendColor = g_vec4_0;
	float4 FinalColor = OriginColor * BlendColor * 2.0f;
	float4 CalcColor = saturate(FinalColor);

	Out.vColor = CalcColor;
	Out.vColor.a = OriginColor.a;

	return Out;
}
PS_OUT PS_BLEND_TEX_HDR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 OriginColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 BlendColor = g_vec4_0;
	float4 FinalColor = OriginColor * BlendColor * 2.0f;
	float4 CalcColor = saturate(FinalColor);
	float4 Help = g_tex_1.Sample(LinearSampler, In.vTexUV);
	
	Out.vColor = CalcHDRColor(CalcColor, g_float_0);
	Out.vColor.a = OriginColor.a;

	return Out;
}
technique11 DefaultTechnique
{
	pass Default //0
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Default_Tex //1
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEX();
	}

	pass Blend_Tex //2
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLEND_TEX();
	}

	pass Blend_Tex_HDR //3
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_NonCulling);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLEND_TEX_HDR();
	}

	pass Player_KineticParticle //4
	{
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PLAYER_KINETIC_PARTICLE();
	}
}