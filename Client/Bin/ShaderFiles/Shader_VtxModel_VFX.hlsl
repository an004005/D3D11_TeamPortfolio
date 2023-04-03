#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

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

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};
struct PS_OUT
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
	float4		vRMA : SV_TARGET3;

};

struct VS_OUT_NORM
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_IN_NORM
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
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

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_NORM VS_MAIN_NORM(VS_IN In)
{
	VS_OUT_NORM		Out = (VS_OUT_NORM)0;

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

PS_OUT_NORM PS_DEFAULT_NORM(PS_IN_NORM In)
{
	PS_OUT_NORM			Out = (PS_OUT_NORM)0;
	float flags = SHADER_DEFAULT;

	float3 vNormal = In.vNormal.xyz;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	// Out.vFlag = flags;

	return Out;
}

PS_OUT_NORM PS_EM220_NORM(PS_IN_NORM In)
{
	PS_OUT_NORM			Out = (PS_OUT_NORM)0;
	float flags = SHADER_DEFAULT;

	float3 vNormal = In.vNormal.xyz;
	float4 Tex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = Tex * Color * 2.0f;
	float4 Final = saturate(Blend);
	Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	// Tex.r >= g_float_1 / 255.f
	if (Out.vRMA.g <= 158.f/255.f && Tex.r >= 130.f / 255.f)
	{
		Out.vColor = Final;
		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, g_float_0, flags);

	}
	else
	{
		Out.vColor = Tex;
		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	}
	vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);


	return Out;
}

PS_OUT_NORM PS_EM0110_BUBLLE(PS_IN_NORM In)
{
	PS_OUT_NORM			Out = (PS_OUT_NORM)0;
	float flags = SHADER_DEFAULT;

	// float2 FlowUV = g_tex_3.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(-g_Time* 0.1f, -g_Time* 0.1f)));



	float3 vNormal = In.vNormal.xyz;
	Out.vColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - g_Time * 0.1f));

	vector		vNormalDesc = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - g_Time * 0.1f));
	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vRMA = g_tex_2.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - g_Time * 0.1f));
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	// Out.vFlag = flags;

	float fDissolve = g_tex_3.Sample(LinearSampler, In.vTexUV).r;

	if (g_float_0 <= fDissolve)
		discard;

	return Out;
}

PS_OUT_NORM PS_650_NORM(PS_IN_NORM In)
{
	PS_OUT_NORM			Out = (PS_OUT_NORM)0;
	float flags = SHADER_DEFAULT;

	float3 vNormal = In.vNormal.xyz;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	vector	vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
	Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);

	float fDissolvePower = g_tex_3.Sample(LinearSampler, In.vTexUV).r;

	if (Out.vRMA.b == 0.f)
	{
		Out.vRMA.b += g_float_0;
		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, Out.vColor.a * g_float_1, flags);
	}
	else
	{
		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, Out.vColor.a * g_float_2, flags);
	}

	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);


	Out.vColor.a = 1.f;

	if (g_float_3 >= fDissolvePower)
	{
		discard;
	}

	return Out;
}

PS_OUT_NORM PS_EM1200_RIBBON(PS_IN_NORM In)
{
	PS_OUT_NORM			Out = (PS_OUT_NORM)0;
	float flags = SHADER_DEFAULT;

	float3 vNormal = In.vNormal.xyz;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	vector	vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
	Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far,0.f, flags);
	vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT_NORM PS_MAIN_NORM (PS_IN_NORM In)
{
	PS_OUT_NORM			Out = (PS_OUT_NORM)0;


	float3 vNormal = In.vNormal.xyz;
	float2 FlipUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.f, 1, 1);

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, FlipUV);
	float4 OriginColor = g_vec4_0;

	float4 BlendColor = DefaultTex * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
	vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float Mask = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vColor.a = Out.vColor.a;

	if(Mask >= 0.f)
	{
		if(g_tex_on_3)
			Out.vRMA = g_tex_3.Sample(LinearSampler, In.vTexUV);
		else
			Out.vRMA = float4(g_float_1, g_float_2, g_float_3, 0.f);

		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, SHADER_DEFAULT);
	}

	if(g_float_4 <= 0.f)
	{
		discard;
	}

	return Out;
}

PS_OUT_NORM PS_EM0800_DIVE(PS_IN_NORM In)
{
	PS_OUT_NORM			Out = (PS_OUT_NORM)0;


	float3 vNormal = In.vNormal.xyz;
	float2 FlipUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.f, 1, 1);

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, FlipUV);
	float4 OriginColor = g_vec4_0;

	float4 BlendColor = DefaultTex * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
	vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));
	float Mask = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vColor.a *= g_float_5;

	if (Mask >= 0.f)
	{
		if (g_tex_on_3)
			Out.vRMA = g_tex_3.Sample(LinearSampler, In.vTexUV);
		else
			Out.vRMA = float4(g_float_1, g_float_2, g_float_3, 0.f);

		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, SHADER_DEFAULT);
	}

	if (g_float_4 <= 0.f)
	{
		discard;
	}

	return Out;
}

PS_OUT_NORM PS_NORM_MONSTER_SPAWN(PS_IN_NORM In)
{
	PS_OUT_NORM			Out = (PS_OUT_NORM)0;


	float3 vNormal = In.vNormal.xyz;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 OriginColor = g_vec4_0;

	float4 BlendColor = DefaultTex * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	vector		vNormalDesc = g_tex_1.Sample(LinearSampler, In.vTexUV);
	vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vColor = FinalColor;
	Out.vColor.a = DefaultTex.r;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, g_float_0, SHADER_DEFAULT);
	Out.vRMA = float4(g_float_1, g_float_2, g_float_3, 0.f);
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, g_float_4);

	if(DefaultTex.r * g_float_5 <= 0.01f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = CalcHDRColor(g_vec4_0, g_float_0);

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_SAS_MASK_PARTICLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 randomNormal = g_tex_0.Sample(LinearSampler, In.vTexUV).xy;
	float2 distortionUV = randomNormal * g_float_0 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(g_Time * 0.f, g_Time * 0.f));

	float4 Default = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float4 Mask = g_tex_2.Sample(LinearSampler, distortionUV);
	float fDissolve = g_tex_3.Sample(LinearSampler, In.vTexUV).r;

	float4 Color = g_vec4_0;
	float4 Blend = Default * Color * 2.0f;
	float4 Final = saturate(Blend);
	Final.a = Mask.r;

	Out.vColor = CalcHDRColor(Final, g_float_1);
	Out.vColor *= g_float_2;

	if (g_float_3 >= fDissolve)
		discard;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_USE_KINETIC_CIRCLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 White = g_tex_4.Sample(LinearSampler, In.vTexUV);

	float2 randomNormal = g_tex_2.Sample(LinearSampler, In.vTexUV * 2.f).xy;
	float2 FlowUV = randomNormal * g_float_0 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(g_float_1, g_Time));
	float2 FlowUV2 = randomNormal * g_float_0 + TilingAndOffset(In.vTexUV, float2(10.f, 10.f), float2(g_Time, g_float_1));

	float4 Default = g_tex_0.Sample(LinearSampler, FlowUV);

	float4 NoiseTex = g_tex_1.Sample(LinearSampler, (FlowUV));
	float4 NoiseTex2 = g_tex_1.Sample(LinearSampler, (FlowUV2));

	float4 Color = g_vec4_0;
	float4 BlendColor = White * Color * 2.0f;

	if (BlendColor.a != 0.f)
	{
		BlendColor -= NoiseTex.r;
		BlendColor -= NoiseTex2.r;
	}

	float4 FinalColor = saturate(BlendColor);


	if(NoiseTex.r >= 0.7f)
	{
		Out.vColor = CalcHDRColor(BlendColor, 10.f);
	}
	else
	{
		Out.vColor = CalcHDRColor(FinalColor, g_float_2);
	}

	float Mask = g_tex_3.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 8,8)).r;

	Out.vColor.a = saturate(g_float_3 * Mask);

	float fDissolvePower = g_tex_5.Sample(LinearSampler, FlowUV).r;

	if (g_float_5 >= fDissolvePower)
		Out.vColor.a = 0.f;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.a * g_float_4);
	return Out;
}

PS_OUT PS_BRON_LASER_MOUTH(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Mask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 BlendColor = Default * Color * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(BlendColor, g_float_0);
	Out.vColor.a = Mask.r * g_float_1;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
	return Out;
}

PS_OUT PS_EM1200_STAMP_IMPACT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x * 2.f,In.vTexUV.y * 2.f));
	// float4 Mask = g_tex_1.Sample(LinearSampler, In.vTexUV * 4);

	float4 Color = g_vec4_0;
	float4 BlendColor = Default * Color * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(BlendColor, g_float_0);
	Out.vColor.a = Mask.r * g_float_1;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_FLOWERLEG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x * g_float_0, In.vTexUV.y));
	Out.vColor.a *= g_float_1;
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.a);

	return Out;
}

PS_OUT PS_BRAINCRUSH_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor.a = 0.f;
	
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, g_float_0);

	return Out;
}


PS_OUT PS_EM1200_SLASH(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float Mask = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x + g_float_0, In.vTexUV.y));

	float4 Default = g_tex_1.Sample(LinearSampler, In.vTexUV);

	Out.vColor = CalcHDRColor(Default , g_float_3);
	Out.vColor.a = Mask* (1 - Default.a)* g_float_1;
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, (1 - Default.a )* g_float_2);

	if (g_float_1 <= 0.f)
		discard;

	// if (Out.vColor.a <= 0.001f)
	// 	discard;


	return Out;
}

PS_OUT PS_EM0200_SPIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 CalcUV = float2(In.vTexUV.x + g_float_0, In.vTexUV.y);

	Out.vColor = g_tex_0.Sample(LinearSampler, CalcUV);
	Out.vColor.a = Out.vColor.r * g_float_1;
	Out.vFlag = float4(0.f, 0.f, 0.f, Out.vColor.a);

	float	fDissolve = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	float	fBlendDissolve = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	float   MixDissolve = saturate(fDissolve * fBlendDissolve);

	if (g_float_2 <= MixDissolve)
		discard;

	return Out;
}

PS_OUT PS_TUTORIALBOSS_SPIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x * g_float_0, In.vTexUV.y));
	Out.vColor.a = Out.vColor * g_float_1;
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.a);

	return Out;
}

PS_OUT PS_DEFAULT_MODEL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 OriginTex = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vColor = CalcHDRColor(OriginTex, g_float_0);

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
	return Out;
}

PS_OUT PS_SAS_DEAD_EF(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 OriginTex = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y - g_float_0));
	float4 Color = g_vec4_0;
	float4 Blend = OriginTex * Color * 2.0f;
	float4 Final = saturate(Blend);
	float BlendAlpha = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	Out.vColor = CalcHDRColor(Final, g_float_1);
	Out.vColor.a = Out.vColor.a * BlendAlpha * g_float_2;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (Out.vColor.a <= 0.001f)
		discard;

	return Out;
}

PS_OUT PS_EM1100_ELEC_BULLET_EXPLODE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 TilltingUV = TilingAndOffset(In.vTexUV, float2(10.f, 10.f), float2(0.f, 0.f));

	float2 TEXUV = Get_FlipBookUV(TilltingUV, g_Time, 0.05, 8, 8);
	float4 OriginTex = g_tex_0.Sample(LinearSampler, TEXUV);

	Out.vColor = CalcHDRColor(OriginTex, g_float_0);
	Out.vColor.a = OriginTex.r;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
	return Out;
}

PS_OUT PS_EM1100_STAMP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 OriginTex = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vColor = OriginTex;
	Out.vColor.a = OriginTex.r * g_float_0;
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_EXPLODE_CROSS_EF(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 FlipUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.03, 8, 8);
	float4 OriginTex = g_tex_0.Sample(LinearSampler, FlipUV);

	Out.vColor = CalcHDRColor(OriginTex, g_float_0);
	// Out.vColor.a = OriginTex.r * g_float_0;
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}


PS_OUT PS_ATTACK_SLASH_LINE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 OriginTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 BlendColor = OriginTex * Color * 2.0f;
	float4 FinalColor = saturate(BlendColor);


	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	Out.vColor.a *= saturate(OriginTex.r * g_float_1);
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
	return Out;
}

// Mesh Trail (Gara)
PS_OUT PS_DEFAULT_MODEL_FLOWUV(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 randomNormal = g_tex_1.Sample(LinearSampler, In.vTexUV).xy;
	float2 FlowUV = randomNormal * g_float_2 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(-g_Time * 2.f, -g_Time* 2.f));
	float Gradient = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	float4 OriginTex = g_tex_0.Sample(LinearSampler, FlowUV);
	float4 ChooseColor = g_vec4_0;
	float4 BlendColor = OriginTex * ChooseColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(BlendColor, g_float_1);
	Out.vColor.a = OriginTex.a * g_float_0 * (In.vTexUV.y) * Gradient;
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (Out.vColor.a <= 0.01f)
		discard;


	return Out;
}

PS_OUT PS_EM1100_WATER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 randomNormal = g_tex_0.Sample(LinearSampler, In.vTexUV).xy;
	float2 FlowUV = randomNormal * g_float_0 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(-g_Time * g_float_3, -g_Time * g_float_3));

	float4 OriginTex = g_tex_1.Sample(LinearSampler, FlowUV);
	float Mask = g_tex_2.Sample(LinearSampler, float2(FlowUV.x, FlowUV.y * 4.f)).r;
	float Gradient = g_tex_3.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y * (g_float_4))).r;
	float4 ChooseColor = g_vec4_0;
	float4 BlendColor = OriginTex * ChooseColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_1);
	Out.vColor.a = Mask * Gradient * g_float_2;

	Out.vFlag = float4(SHADER_DISTORTION_STATIC, 0.f, 0.f, Out.vColor.a);

	if (Out.vColor.a <= 0.01f)
		discard;

	if (g_float_2 <= 0.f)
		discard;

	return Out;
}

// Mesh Trail (Gara)
PS_OUT PS_BRONJON_LASER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 randomNormal = g_tex_1.Sample(LinearSampler, In.vTexUV).xy;

	float2 FlowUV = randomNormal * g_float_2+ 
		TilingAndOffset(In.vTexUV, float2(1.f, 1.f),
			float2(g_Time, g_Time));

	float4 DistortionTex = g_tex_3.Sample(LinearSampler, FlowUV);
	float fWeight = DistortionTex.r * g_float_3;

	float2 FlowGradientUV = randomNormal * g_float_2 +
		TilingAndOffset(In.vTexUV, float2(1.f, 1.f),
			float2(g_Time, g_Time));

	float Gradient = g_tex_2.Sample(LinearSampler, FlowGradientUV + fWeight).r;

	float4 OriginTex = g_tex_0.Sample(LinearSampler, FlowGradientUV + fWeight);

	float4 ChooseColor = g_vec4_0;

	float4 BlendColor = OriginTex * ChooseColor * 2.0f;

	float GradientAlpha = g_tex_4.Sample(LinearSampler, In.vTexUV).r;

	float StartAlpha = g_tex_5.Sample(LinearSampler, float2(In.vTexUV.x * g_float_4, In.vTexUV.y)).r;
	
	Out.vColor = CalcHDRColor(BlendColor, g_float_1);
	Out.vColor.a = saturate(((OriginTex.a * g_float_0 * Gradient) *GradientAlpha) * StartAlpha);
	
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, 0.f);

	if (g_float_4 <= 0.f)
		discard;

	if (Out.vColor.a <= 0.001f)
		discard;

	return Out;
}

// g_float_0 : 모델 텍스쳐 UV.y 값
// g_float_1 : UV.y 값 연동된
// g_float_2 : Emissive
// g_float_3 : Dissolve 커질수록 사라짐

// g_vec4_0 : 섞는 색상

// g_tex_0 : 반절만 나오는 텍스쳐
// g_tex_1 : 흰색 꽉찬 텍스쳐
// g_tex_2 : 디졸브 텍스쳐 

PS_OUT PS_MAIN_DEFAULT_ATTACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 BasicColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x * g_float_0, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = BasicColor * OriginColor * 2.0f;
	

	float4 AllTex = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 BlendColor2 = AllTex * OriginColor * 2.0f;
	float fDissolvePower = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	if(g_float_1 <= 0.f)
	{
		Out.vColor = CalcHDRColor(BlendColor2, g_float_2);
		Out.vColor.a = 1.f;

		Out.vFlag = float4(0.f, SHADER_SCIFI_PLAYER_ATTACK, 0.f, Out.vColor.a);

	}
	else
	{
		Out.vColor = CalcHDRColor((BlendColor *g_float_1) + (BlendColor2 * (1-g_float_1)), g_float_2);
		Out.vColor.a = (BasicColor.r * g_float_1) + (BlendColor2.r * (1 - g_float_1));
		Out.vFlag = float4(0.f, SHADER_SCIFI_PLAYER_ATTACK, 0.f, g_float_1);
	}

	if (g_float_3 >= fDissolvePower)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN_CHARGING_TWIST(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 BasicColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = BasicColor * OriginColor * 2.0f;


	float2 randomNormal = g_tex_1.Sample(LinearSampler, In.vTexUV).xy;
	float2 distortionUV = randomNormal * g_float_0 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, g_Time * 3.f));
	float fDissolvePower = g_tex_2.Sample(LinearSampler, distortionUV).r;

	Out.vColor = CalcHDRColor(BlendColor, g_float_1);
	// Out.vColor.a = 1.f;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (g_float_2 >= fDissolvePower)
	{
		discard;
	}

	return Out;
}

// g_float_0 : 한번에 띄우지 않게 살살 반만 그리는 텍스쳐 UV.y 값
// g_float_1 : 반만 그리는 텍스쳐랑 전부그리는 텍스쳐랑 바꿔치기 위한 float값
// g_float_2 : Emissive
// g_float_3 : Dissolve 커질수록 사라짐
// g_float_4 : PostProcess 디스토션 세기

// g_vec4_0 : 처음 색상
// g_vec4_1 : 최종 색상

// g_tex_0 : 반절만 나오는 텍스쳐
// g_tex_1 : 번개모양 일반적인 텍스쳐
// g_tex_2 : 디졸브 텍스쳐와 섞을 노이즈 텍스쳐
// g_tex_3 : 번개모양 디졸브 텍스쳐
// g_tex_4 : 지직하는 디졸브를 랜덤하게 주기 위한 노이즈 텍스쳐

PS_OUT PS_MAIN_ELEC_ATTACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 BasicColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x * g_float_0, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = BasicColor * OriginColor * 2.0f;


	float4 AllTex = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 BlendColor2 = AllTex * OriginColor * 2.0f;

	float2 randomNormal = g_tex_2.Sample(LinearSampler, In.vTexUV).xy;
	float2 distortionUV = randomNormal * g_float_4 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, g_Time));
	float fDissolvePower = g_tex_3.Sample(LinearSampler, distortionUV).r;
	float fDissolveNoise = g_tex_4.Sample(LinearSampler, distortionUV).r;
	float ND = fDissolvePower *fDissolveNoise;

	if (g_float_1 <= 0.f)
	{
		Out.vColor = CalcHDRColor(BlendColor2, g_float_2);
		Out.vColor.a = BlendColor2.r;

		Out.vFlag = float4(0.f, 0.f, 0.f, (1-g_float_1));
	}
	else
	{
		Out.vColor = CalcHDRColor((BlendColor *g_float_1) + (BlendColor2 * (1 - g_float_1)), g_float_2);
		Out.vColor.a = (BasicColor.r * g_float_1) + (BlendColor2.r * (1 - g_float_1));
		Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, BlendColor2.r * (1-g_float_4));
	}

	Out.vRMA = float4(1.0f, 0.f, 1.0f, 0.f);

	if (g_float_3 >= ND)
	{
		discard;
	}
	return Out;
}



// g_float_0 : 모델 텍스쳐 UV.y 값
// g_float_1 : 바뀔 텍스쳐의 UV.y 값
// g_float_2 : Emissive
// g_float_3 : Dissolve 커질수록 사라짐
// g_float_4 : Distortion 강도

// g_vec4_0 : 초기 색상
// g_vec4_1 : 바뀔 색상

// g_tex_0 : 반절만 나오는 텍스쳐
// g_tex_1 : 흰색 꽉찬 텍스쳐
// g_tex_2 : 디졸브 방향 텍스쳐
// g_tex_3 : 디졸브에 섞을 노이즈 텍스쳐
// g_tex_4 : 디스토션 플로우맵

PS_OUT PS_MAIN_FIRE_ATTACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 BasicColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x * g_float_0, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = BasicColor * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	// FinalColor.a = BasicColor.r;

	float4 AllTex = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x * (1-g_float_1 ), In.vTexUV.y));
	float4 BlendColor2 = AllTex * OriginColor * 2.0f;

	if(g_tex_on_4)
	{
		float2 randomNormal = g_tex_4.Sample(LinearSampler, In.vTexUV).xy;
		float2 distortionUV = randomNormal * g_float_4 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(g_Time, 0.f));

		float fDissolvePower = g_tex_2.Sample(LinearSampler, distortionUV).r;
		float fDissolveNoise = g_tex_3.Sample(LinearSampler, distortionUV).r;
		float ND = fDissolvePower * fDissolveNoise;

		if (g_float_1 <= 0.f)
		{
			Out.vColor = CalcHDRColor(BlendColor2, g_float_2);
			Out.vColor.a = AllTex.r;

			Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, 0.f);
		}
		else
		{
			Out.vColor = CalcHDRColor((FinalColor *g_float_1) + (BlendColor2 * (1 - g_float_1)), g_float_2);
			Out.vColor.a = g_float_5;
			Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
		}

		if (g_float_3 >= ND)
		{
			discard;
		}
	}
	else
	{
		float fDissolvePower = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
		float fDissolveNoise = g_tex_3.Sample(LinearSampler, In.vTexUV).r;
		float ND = fDissolvePower * fDissolveNoise;
		if (g_float_1 <= 0.f)
		{
			Out.vColor = CalcHDRColor(BlendColor2, g_float_2);
			Out.vColor.a = AllTex.r;

			Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
		}
		else
		{
			Out.vColor = CalcHDRColor((FinalColor *g_float_1) + (BlendColor2 * (1 - g_float_1)), g_float_2);
			Out.vColor.a = (BasicColor.r * g_float_1) + (AllTex * (1 - g_float_1));
			Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
		}

		if (g_float_3 >= ND)
		{
			discard;
		}
	}

	return Out;
}

technique11 DefaultTechnique
{
	//0
	pass Empty
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//1
	pass PlayerDefaultAttack
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT_ATTACK();
	}

	//2
	pass PlayerFireAttack
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FIRE_ATTACK();
	}

	//3
	pass PlayerElecAttack
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ELEC_ATTACK();
	}

	//4
	pass Normal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORM();
	}

	//5
	pass ChargingTwist
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CHARGING_TWIST();
	}

	//6
	pass DefaultModel
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT_MODEL();
	}

	//7
	pass DefaultDistortion
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLOWERLEG();
	}

	//8
	pass TexNormal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT_NORM();
	}

	//9
	pass MonsterBulletNormal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_650_NORM(); 
	}

	//10
	pass DefaultModelFlowUV
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT_MODEL_FLOWUV();
	}

	//11
	pass BronjonLaser
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRONJON_LASER();
	}

	//12
	pass BronMouth
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRON_LASER_MOUTH();
	}

	//13
	pass TutorialBoss_Spin
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TUTORIALBOSS_SPIN();
	}

	//14
	pass PlayerUseKineticCircle
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_USE_KINETIC_CIRCLE();
	}


	//15
	pass PlayerSlashLine
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ATTACK_SLASH_LINE();
	}

	//16
	pass MonsterSpawn
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_NORM_MONSTER_SPAWN();
	}

	//17
	pass EM0110BUBBLE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0110_BUBLLE();
	}

	//18
	pass em220Norm
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM220_NORM();
	}

	//19
	pass EM0200Spin
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0200_SPIN();
	}

	//20
	pass em1100Stamp
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_STAMP();
	}

	//21
	pass Em1100_ElecBullet_Explode
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_ELEC_BULLET_EXPLODE();
	}

	//22
	pass SasDead
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_DEAD_EF();
	}

	//23
	pass KineticMeshEffectLikeParticle
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_MASK_PARTICLE();
	}

	//24
	pass Explode_CrossEF
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EXPLODE_CROSS_EF();
	}

	//25
	pass Em1100Water
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_WATER();
	}

	//26
	pass Em1200Slash
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1200_SLASH();
	}

	//27
	pass Em1200StampImp
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1200_STAMP_IMPACT();
	}

	//28
	pass BrainCrushDistortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAINCRUSH_DISTORTION();
	}

	//29
	pass em1200Ribbon
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1200_RIBBON();
	}

	//30
	pass em0800DiveIn
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0800_DIVE();
	}
}
