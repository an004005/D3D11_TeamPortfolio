#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

texture2D GrapTexture;
Texture2D g_DepthTex;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct VS_OUT_SOFT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_IN_SOFT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;

};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	return Out;
}

VS_OUT_SOFT VS_MAIN_SOFT(VS_IN In)
{
	VS_OUT_SOFT		Out = (VS_OUT_SOFT)0;


	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	return Out;
}


PS_OUT PS_DRIVEMOD_GLOW(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	float4 Color = g_vec4_0;
	float4 Blend = Default * Color * 2.0f;
	float4 Final = saturate(Blend);
	float Mask1 = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	float Mask2 = g_tex_3.Sample(LinearSampler, In.vTexUV).r;
	float Mask3 = g_tex_4.Sample(LinearSampler, In.vTexUV).r;
	float Mask4 = g_tex_5.Sample(LinearSampler, In.vTexUV).r;
	float Mask5 = g_tex_6.Sample(LinearSampler, In.vTexUV).r;
	float Mask6 = g_tex_7.Sample(LinearSampler, In.vTexUV).r;
	Mask5 = Mask5 * Mask6;
	Final.a = (Mask * g_float_0) + (Mask1 * g_float_1) + (Mask2 * g_float_2) + (Mask3 * g_float_3) + (Mask4 * g_float_4) + (Mask5 * g_float_5);


	Out.vColor = CalcHDRColor(Final, g_float_6);

	Out.vColor.a *= g_float_7;

	return Out;
}

PS_OUT PS_BRON_BITE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Mask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	Default.a = Mask.r;
	float4 Color = g_vec4_0;
	float4 BlendColor = Default * Color * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	float fDissolvePower = g_tex_2.Sample(LinearSampler, In.vTexUV).r;


	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	Out.vColor.a *= g_float_1;

	if (g_float_2 >= fDissolvePower)
		discard;

	return Out;
}

PS_OUT PS_BRON_LASER_START(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float4 ChooseColor = g_vec4_0;
	float4 BlendColor = Default * g_vec4_0 * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	Out.vColor.a = Mask * g_float_1;

	if (g_float_1 == 0.f)
		discard;

	return Out;
}

PS_OUT PS_EM0650_BULLET_BIRTH_BASE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float  FirstMask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	float  SecondMask = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	float4 Color = float4(g_vec4_0.xyz,1.f);

	float4 BlendColor = Default * Color * 2.0f;

	float4 FinalColor = saturate(BlendColor);
	Out.vColor = FinalColor;

	Out.vColor.a = saturate((FirstMask * g_float_0) + (SecondMask * (1 - g_float_0))) * g_float_1;

	

	return Out;
}

// ***** ok su hyeon
// [8] TargetaArrow
// g_tex_0 : Original Texture
// g_tex_1 : Emissive Texture
// g_vec4_0 : Color
// g_float_0 : Century
// g_float_1 : ratio
PS_OUT PS_GreenEmissive(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Emissive = g_tex_1.Sample(LinearSampler, In.vTexUV);

	float Mask = Emissive.g;

	float4 DefaultColor = float4(Emissive.g, Emissive.g, Emissive.g, 0.f);

	float4 BlendColor = DefaultColor  * g_vec4_0 * 2.0f;

	float4 FinalColor = saturate(BlendColor);

	float4 HDRColor = saturate(FinalColor + Texture * g_float_0);

	Out.vColor = CalcHDRColor(HDRColor, g_float_1);

	Out.vColor.a = Mask;

	return Out;
}
// [9] ColorChange
// g_tex_0 : Original Texture
// g_vec4_0 : Color
PS_OUT PS_ColorChange(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 MixTexture = saturate(g_vec4_0 * Texture * 2.0f);
	Out.vColor = MixTexture;

	return Out;
}
// [10] HpGauge (HP)
// g_float_0 : Ratio
// g_vec4_0 : Color
PS_OUT PS_MixTexture(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_float_0 < In.vTexUV.x)
		discard;

	float4 DefaultWhite = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Texture = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float Mask = g_tex_2.Sample(LinearSampler, In.vTexUV).a;

	float4 BlendColor = DefaultWhite * g_vec4_0;

	Out.vColor = saturate(BlendColor + Texture * 0.25f) * 3.0f;
	Out.vColor.a = Mask;

	return Out;
}
// [11] HpGauge (HPBack)
// g_float_0 : Ratio
// g_vec4_0 : Color
PS_OUT PS_UVCut(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_float_0 < In.vTexUV.x)
		discard;

	float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 MixTexture = saturate(g_vec4_0 * Texture * 2.0f);
	Out.vColor = MixTexture;

	return Out;
}
// [12] Psychokinesis Gauge
// g_float_0 : Progress
// g_vec4_0 : Color
// g_tex_0 : White
// g_tex_1 : Original Texture
// g_tex_2 : Mask Texture
PS_OUT PS_Flow(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float fProgress = g_float_0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float ProgressMask = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	if (ProgressMask >= fProgress)
		discard;

	float4 GlowBase = DefaultTex * g_vec4_0;

	float4 PointTex = g_tex_1.Sample(LinearSampler, In.vTexUV);

	Out.vColor = saturate(GlowBase);

	Out.vColor.a = PointTex.r;

	return Out;
}
// [13] Alpha Control
// g_float_0 : Alpha
// g_vec4_0 : Color
PS_OUT PS_Alpha_Color(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 MixTexture = saturate(float4(g_vec4_0.rgb, g_float_0) * Texture);
	Out.vColor = MixTexture;

	return Out;
}
// [14] UV cut
// g_vec2_0 : Print Index
// g_vec2_1 : [x] RowCnt, [y] ColCnt
VS_OUT VS_UVCut(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	In.vTexUV.x = In.vTexUV.x + g_vec2_0.x;
	In.vTexUV.y = In.vTexUV.y + g_vec2_0.y;

	In.vTexUV.x = In.vTexUV.x / g_vec2_1.x;
	In.vTexUV.y = In.vTexUV.y / g_vec2_1.y;

	Out.vTexUV = In.vTexUV;
	return Out;
}
// g_float_0 : Alpha
PS_OUT PS_Alpha_Blend(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.a * g_float_0;
	return Out;
}
// ****** ok su hyeon

struct PS_OUT_Flag
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

PS_OUT_Flag PS_MAIN_Flag(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	// Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	Out.vFlag = float4(1.0f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag EXPLODE_DEFAULT_RECT(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;
	float2 TEXUV;

	if(g_int_0 > 0)
		TEXUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 8, 8);
	else
		TEXUV = Get_FlipBookUV(In.vTexUV, 0, 0.05, 8, 8);

	float4 Default = g_tex_0.Sample(LinearSampler, TEXUV);
	Out.vColor = CalcHDRColor(Default, g_float_0);
	Out.vFlag = float4(SHADER_DISTORTION_STATIC, 0.f, 0.f, Default.a * g_float_1);
	Out.vColor.a *= g_float_2;

	return Out;
}

PS_OUT_Flag PS_MASK_HIT(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 DefaultColor =g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 OriginColor = g_vec4_0;
	// float4 BlendColor = DefaultColor * OriginColor * 2.0f;
	// float4 FinalColor = saturate(BlendColor);
	// float  Mask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	// Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	// Out.vColor.a = Mask;
	Out.vColor = DefaultColor;
	// Out.vColor.a = DefaultColor.a;
	Out.vFlag = float4(0.f, SHADER_HIT_DECAL, 0.f, 0.f);
	// if (Out.vColor.a == 0.f)
	// 	discard;

	return Out;
}

// g_tex_0 : 원 마스크 텍스쳐
// g_vec4_0 : 테스트용 컬러
PS_OUT_Flag PS_DISTORTION(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.r);

	Out.vColor = 0;
	// Out.vColor.a = 1 - Out.vColor.r;
	

	
	return Out;
}

PS_OUT_Flag PS_DISTORTION_PLAYER(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.r * g_float_0);

	Out.vColor = 0;
	Out.vColor.a *= g_float_0;

	return Out;
}

PS_OUT_Flag PS_EM1200_STAMP_DISTORTION(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	float4 Mask = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask.r * g_float_0);

	Out.vColor = g_vec4_0;
	Out.vColor.a *= Mask.r * g_float_1;

	return Out;
}

PS_OUT_Flag PS_EM1200_FEAR(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vFlag = float4(SHADER_DISTORTION_STATIC, 0.f, 0.f, Out.vColor.r * g_float_0);

	Out.vColor = 0;
	Out.vColor.a *= g_float_0;

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_DISTORTION_PLAYER_B(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;
	float4 White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = White * Color * 2.0f;
	float4 Tex = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float4 Noise = g_tex_2.Sample(LinearSampler, In.vTexUV);
	float Mask = g_tex_3.Sample(LinearSampler, In.vTexUV);
	Blend.a = Tex.r;


	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(Noise.xyz, vViewDir.xyz, 0.1f);
	float4 FinalColor = saturate(Blend * fFresnel);

	Out.vColor = CalcHDRColor(FinalColor, g_float_0) * g_float_0;
	Out.vColor.a = Tex.r * Noise.r* g_float_0;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask * g_float_1);

	if (g_float_1 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_USE_SAS_TELEPORT(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;
	float4 Mask = g_tex_3.Sample(LinearSampler, In.vTexUV);
	float fWeight = Mask * g_float_2;

	float4 White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = White * Color * 2.0f;
	float4 Tex = g_tex_1.Sample(LinearSampler, In.vTexUV + fWeight);
	float4 Noise = g_tex_2.Sample(LinearSampler, In.vTexUV);
	Blend.a = Tex.r;


	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(Noise.xyz, vViewDir.xyz, 0.1f);
	float4 FinalColor = saturate(Blend * fFresnel);

	Out.vColor = CalcHDRColor(FinalColor, g_float_0) * g_float_0;
	Out.vColor.a = Tex.r * Noise.r* g_float_0;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask.r * g_float_1);

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_DISTORTION_DEFAULT(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float Mask = Out.vColor.r * g_float_0;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);

	Out.vColor = 0;

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_SPECIAL_G_LARGE_HIT(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float Mask= g_tex_1.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = White * Color * 2.0f;
	float4 Final = saturate(Blend);
	Final.a = Mask * g_float_0;

	float4 AddTex = g_tex_2.Sample(LinearSampler, In.vTexUV);
	AddTex = CalcHDRColor(AddTex, g_float_1);
	float4 AddColor = saturate(Final + AddTex);
	Out.vColor = CalcHDRColor(AddColor, g_float_2);
	Out.vColor.a = saturate(Final.a * (AddTex.r * g_float_5)) * g_float_3;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, AddTex.r * g_float_4);

	if (g_float_3 <= 0.f)
		discard;



	return Out;
}

PS_OUT_Flag PS_EM8200_STAMP_IMPACT(PS_IN_SOFT In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = White * Color * 2.0f;
	float4 Final = saturate(Blend);
	Final.a = Mask * g_float_0;

	float4 AddTex = g_tex_2.Sample(LinearSampler, In.vTexUV);
	AddTex = CalcHDRColor(AddTex, g_float_1);
	float4 AddColor = saturate(Final + AddTex);
	Out.vColor = CalcHDRColor(AddColor, g_float_2);
	Out.vColor.a = saturate(Final.a * (AddTex.r * g_float_5)) * g_float_3;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, AddTex.r * g_float_4);

	if (g_float_3 <= 0.f)
		discard;

	//soft effect 
	float2		vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTex.Sample(LinearSampler, vTexUV);

	float		fOldViewZ = vDepthDesc.y * g_Far;
	float		fViewZ = In.vProjPos.w;

	Out.vColor.a = saturate(Out.vColor.a * (saturate(fOldViewZ - fViewZ) * 2.5f));

	return Out;
}

PS_OUT_Flag PS_EM0110_SPINATTACK_HIT_EF(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = White * Color * 2.0f;
	float4 Final = saturate(Blend);
	Final.a = Mask * g_float_0;

	float4 AddTex = g_tex_2.Sample(LinearSampler, In.vTexUV);
	AddTex = CalcHDRColor(AddTex, g_float_1);

	float4 YellowBase = g_tex_3.Sample(LinearSampler, In.vTexUV);
	float4 BaseColor = g_vec4_1;
	float4 BaseBlend = YellowBase * BaseColor * 2.0f;
	float4 BaseFinal = saturate(BaseBlend);

	float4 AddColor = saturate(Final + AddTex + (BaseFinal * g_float_6));

	float4 WhiteLight = g_tex_4.Sample(LinearSampler, In.vTexUV);
	BaseFinal += WhiteLight.r;
	Out.vColor = CalcHDRColor(AddColor, g_float_2);
	Out.vColor.a = saturate(Final.a * (AddTex.r * g_float_5) + BaseFinal.r) * g_float_3;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, AddTex.r * g_float_4);

	if (g_float_3 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_SPECIAL_G_HBIM_HIT(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	if (g_float_5 >= 0.01f)
	{
		float4 Mask = g_tex_0.Sample(LinearSampler, In.vTexUV);
		float4 Color = g_vec4_0;
		float4 Blend = Mask * Color * 2.0f;
		float4 Final = saturate(Blend);
		Final.a = Mask.r;

		float4 AddTex = g_tex_1.Sample(LinearSampler, In.vTexUV);
		float4 AddTexColor = g_vec4_1;
		float4 AddBlend = AddTex * AddTexColor * 2.0f;
		float4 AddFinal = saturate(AddBlend);
		AddFinal.a = AddFinal.r;

		AddFinal = CalcHDRColor(AddFinal, g_float_0);

		float4 AddColor = saturate(Final * g_float_1 + AddFinal);

		Out.vColor = CalcHDRColor(AddColor, g_float_2);
		Out.vColor.a = saturate((Final.a * g_float_3) + (AddFinal.a * g_float_4)) * g_float_5;

		Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, AddTex.r * g_float_6);
	}
	else
	{
		float2 TEXUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.03, 4, 4);
		float4 FlowTex = g_tex_2.Sample(LinearSampler, TEXUV);
		float4 Color = g_vec4_2;
		float4 Blend = FlowTex * Color * 2.0f;
		float4 Final = saturate(Blend);
		
		Out.vColor = CalcHDRColor(Final, g_float_7);
		Out.vColor.a = FlowTex.r * g_float_8;
		Out.vFlag = float4(SHADER_DISTORTION_STATIC, 0.f, 0.f, FlowTex.r *g_float_6);
	}

	if (g_float_8 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_EM1200_STAMP_IMPACK_LARGE(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	if (g_float_5 >= 0.01f)
	{
		float4 Mask = g_tex_0.Sample(LinearSampler, In.vTexUV);
		float4 Color = g_vec4_0;
		float4 Blend = Mask * Color * 2.0f;
		float4 Final = saturate(Blend);
		Final.a = Mask.r;

		float4 AddTex = g_tex_1.Sample(LinearSampler, In.vTexUV);
		float4 AddTexColor = g_vec4_1;
		float4 AddBlend = AddTex * AddTexColor * 2.0f;
		float4 AddFinal = saturate(AddBlend);
		AddFinal.a = AddFinal.r;

		AddFinal = CalcHDRColor(AddFinal, g_float_0);

		float4 AddColor = saturate(Final + g_float_1  * AddFinal);

		Out.vColor = CalcHDRColor(AddColor, g_float_2);
		Out.vColor.a = saturate((Final.a * g_float_3) + (AddFinal.a * g_float_4)) * g_float_5;

		float fAlpha = saturate((AddTex.r * g_float_6) + (Final.a * g_float_6));

		Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, fAlpha);
	}
	else
	{
		float2 TEXUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.03, 4, 4);
		float4 FlowTex = g_tex_2.Sample(LinearSampler, TEXUV);
		float4 Color = g_vec4_2;
		float4 Blend = FlowTex * Color * 2.0f;
		float4 Final = saturate(Blend);

		Out.vColor = CalcHDRColor(Final, g_float_7);
		Out.vColor.a = FlowTex.r * g_float_8;
		Out.vFlag = float4(SHADER_DISTORTION_STATIC, 0.f, 0.f, FlowTex.r * g_float_6);
	}

	if (g_float_8 <= 0.f)
		discard;

	return Out;
}


// g_tex_0 : Default White
// g_vec4_0 : Origin Color

PS_OUT_Flag PS_DISTORTION_FLIPBOOK(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV); // Not Use Plz Fix

	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 4, 4);
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);

	Out.vColor.a = 0.f;

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_WEAK_HIT_EF(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, g_float_0, g_int_0, g_int_1);
	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = Default_White * Color * 2.0f;
	float4 Final = saturate(Blend);
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;
	float Mask1 = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	Out.vColor = CalcHDRColor(Final, g_float_1);
	Out.vColor.a = Mask * Mask1* g_float_2;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (g_float_1 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_EM8200_ELEC_BILLBOARD(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, g_float_0, g_int_0, g_int_1);
	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = saturate(Default_White * Color * 2.0f);
	float4 Final = saturate(Blend);
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;
	Out.vColor = CalcHDRColor(Final, g_float_1);
	Out.vColor.a = saturate(Mask *  g_float_2);

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	// if (g_float_1 <= 0.f)
		// discard;

	if (Out.vColor.a <= 0.1f)
		discard;


	return Out;
}

PS_OUT_Flag PS_AI_CH0500_BULLET_EF(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, g_float_0, g_int_0, g_int_1);
	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = saturate(Default_White * Color * 2.0f);
	float4 Final = saturate(Blend);
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;
	Out.vColor = CalcHDRColor(Final, g_float_1);
	Out.vColor.a = Mask * g_float_2;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT_Flag PS_DEFAULT_CHARGING_1_DISTORTION(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.02, 4, 4);
	float  Mask = g_tex_0.Sample(LinearSampler, TexUV).r;
	Out.vColor= CalcHDRColor(g_vec4_0, g_float_1);
	Out.vColor.a = Mask * g_float_0;
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.a);

	// Out.vColor.a = 0.f;

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_EM0220_EXPLODE(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float2 TexUV;

	if(g_int_0 > 0)
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.03, 8, 8);
	else
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.00, 8, 8);

	float4 Tex = g_tex_0.Sample(LinearSampler, TexUV);

	Out.vColor = CalcHDRColor(Tex, g_float_0);
	Out.vColor *= g_float_2;
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.a * g_float_1);

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_EM0220_EXPLODE_SOFT(PS_IN_SOFT In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float2 TexUV;

	if (g_int_0 > 0)
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.03, 8, 8);
	else
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.00, 8, 8);

	float4 Tex = g_tex_0.Sample(LinearSampler, TexUV);

	Out.vColor = CalcHDRColor(Tex, g_float_0);
	Out.vColor *= g_float_2;
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.a * g_float_1);

	if (g_float_0 <= 0.f)
		discard;

	//soft effect 
	float2		vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthDesc = g_DepthTex.Sample(LinearSampler, vTexUV);

	float		fOldViewZ = vDepthDesc.y * g_Far;
	float		fViewZ = In.vProjPos.w;

	Out.vColor.a = saturate(Out.vColor.a * (saturate(fOldViewZ - fViewZ) * 2.5f));

	return Out;
}

PS_OUT_Flag PS_EM1100_STAMP_SMOKE(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float2 TexUV;

	if (g_int_0 > 0)
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.03, 8, 8);
	else
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.00, 8, 8);

	float4 Tex = g_tex_0.Sample(LinearSampler, TexUV);

	Out.vColor = CalcHDRColor(Tex, g_float_0);
	Out.vColor *=  g_float_1;
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (g_float_1 <= 0.f)
		discard;

	return Out;
}


PS_OUT_Flag PS_EM1100_STAMP_SMOKE_NOFLIP(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 Tex = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vColor = Tex;
	Out.vColor *= g_float_0;
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (g_float_0 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_EM1100_ELEC_BULLET_START(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 Tex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = Tex * Color * 2.0f;
	float4 Final = saturate(Blend);
	float2 FlipUV;
	if(g_int_0 >0)
		FlipUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 8, 8);
	else
		FlipUV = Get_FlipBookUV(In.vTexUV, 0.f, 0.05, 8, 8);

	float4 FlipTex = g_tex_1.Sample(LinearSampler, FlipUV);

	FlipTex = CalcHDRColor(FlipTex, g_float_3);

	Out.vColor = CalcHDRColor(Final + FlipTex, g_float_0);

	Out.vColor.a = (FlipTex.r * g_float_1) + (Tex.r * g_float_2);

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (g_float_2 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_EM1100_ELEC_BULLET_LOOP(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 Tex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = Tex * Color * 2.0f;
	float4 Final = saturate(Blend);
	float2 FlipUV;
	float4 FlipTex;
	if (g_int_0 == 1)
	{
		FlipUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 8, 8);
		FlipTex = g_tex_1.Sample(LinearSampler, FlipUV);

	}
	else if (g_int_0 == 2)
	{
		FlipUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 3, 3);
		FlipTex = g_tex_2.Sample(LinearSampler, FlipUV);
	}
	else if (g_int_0 == 3)
	{
		FlipUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.1, 4, 4);
		FlipTex = g_tex_3.Sample(LinearSampler, FlipUV);
	}
	else
	{
		FlipUV = Get_FlipBookUV(In.vTexUV, 0.f, 0.05, 1, 1);
		FlipTex = (vector)0.f;
	}

	if (FlipTex.r >= 0.f)
		FlipTex = CalcHDRColor(FlipTex, g_float_3);
	else
		FlipTex = (vector)0.f;

	FlipTex *= g_float_1;
	float4 OutlineTex = g_tex_4.Sample(LinearSampler, In.vTexUV);
	float4 BlendColor = OutlineTex * g_vec4_2 * 2.0f;
	float4 OutFinal = saturate(BlendColor);
	OutlineTex = CalcHDRColor(OutFinal, g_float_5);

	float4 MixColor = saturate( Final + OutlineTex * 2.0f );
	if(FlipTex.r > 0.f)
		Out.vColor = CalcHDRColor(Final + FlipTex, g_float_0);
	else
		Out.vColor = CalcHDRColor(Final, g_float_0);

	Out.vColor += OutlineTex;
	Out.vColor.a = saturate((FlipTex.r * g_float_1) + (Tex.r * g_float_2) + (OutlineTex.r * g_float_4));

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	if (g_float_2 <= 0.f)
		discard;

	return Out;
}

PS_OUT_Flag PS_KINETIC_DEAD_FLIPBOOK(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV); // Not Use Plz Fix
	float4 Color = g_vec4_0;
	float4 Blend = Default_White * Color * 2.0f;
	float4 Final = saturate(Blend);
	//////
	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 4, 4);
	float  Mask = g_tex_1.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 4,4)).r;


	Out.vColor = CalcHDRColor(Final, g_float_0);
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	Out.vColor.a = Mask * g_float_1;

	return Out;
}

PS_OUT_Flag PS_SPECAIL_G_EXPLODE_LIGHT(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;
	float2 TexUV;

	if(g_int_0 >= 1)
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 4, 4);
	else
		TexUV = Get_FlipBookUV(In.vTexUV, 0.f, 0, 4, 4);

	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV); // Not Use Plz Fix
	float4 Color = g_vec4_0;
	float4 Blend = Default_White * Color * 2.0f;
	float4 Final = saturate(Blend);
	//////
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;


	Out.vColor = CalcHDRColor(Final, g_float_0);
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	Out.vColor.a = Mask * g_float_1;

	return Out;
}

PS_OUT_Flag PS_EM1100_BULLET_DEAD(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV); 
	float4 Color = g_vec4_0;
	float4 Blend = Default_White * Color * 2.0f;
	float4 Final = saturate(Blend);


	float2 TEXUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 8, 8);
	float4  Mask = g_tex_1.Sample(LinearSampler, TEXUV);

	Out.vColor = CalcHDRColor(Mask, g_float_0);
	Out.vColor.a = Mask.r * g_float_1;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

// PS_OUT_Flag PS_EM110_BUG(PS_IN In)
// {
// 	PS_OUT_Flag			Out = (PS_OUT_Flag)0;
//
// 	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 2, 1);
//
//
// 	float4 Default_White = g_tex_0.Sample(LinearSampler, TexUV);
// 	float4 Color = g_vec4_0;
// 	float4 Blend = Default_White * Color * 2.0f;
// 	float4 Final = saturate(Blend);
//
// 	Out.vColor = CalcHDRColor(Final, g_float_0);
// 	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
//
// 	Out.vColor.a *= g_float_1;
//
// 	return Out;
// }

PS_OUT_Flag PS_FLOWLEG_FLIPBOOK(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV); // Not Use Plz Fix
																	 //////
	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 4, 4);
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;
	///
	 float4 OriginColor = g_vec4_0;
	 float4 BlendColor = Default_White * OriginColor * 2.0f;
	 float4 FinalColor = saturate(BlendColor);
	 Out.vColor.xyz = FinalColor.xyz;
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask * g_float_0);

	// Out.vColor.r = 1.f; 
	Out.vColor.a = Mask * g_float_0;

	// Out.vColor.a = 1 - Out.vColor.r;



	return Out;
}

PS_OUT_Flag PS_SAS_TELEPORT_EF(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 Default= g_tex_0.Sample(LinearSampler, In.vTexUV); 
																
	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.1, 2, 2);
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;

	Out.vColor = CalcHDRColor(Default, g_float_0);

	Out.vColor.a = Default.a * Mask * g_float_1;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, 0.f);

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT_Flag PS_EM8200_ICE_DECAL(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Noise = g_tex_1.Sample(LinearSampler, In.vTexUV );
	float4 MixTex = defaultColor * Noise;

	float4 OriginColor = g_vec4_0;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	float4 Final = saturate(BlendColor);

	float Mask = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	

	Out.vColor = CalcHDRColor(Final, g_float_0);

	Out.vColor.a = Mask * Noise.r* g_float_1;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}


// g_tex_0 : Default White
// g_vec4_0 : OriginColor
// g_float_0 : Emissive
PS_OUT_Flag PS_SCIFI(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y * 10.f));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = defaultColor * OriginColor;
	BlendColor.a = 1.f;
	Out.vColor = CalcHDRColor(g_vec4_0, g_float_0);
	Out.vFlag = float4(0.f, SHADER_SCIFI, 0.f, Out.vColor.r);
	
	return Out;
}

// g_tex_0 : Default White
// g_tex_1 : Mask

// g_vec4_0 : OriginColor
// g_float_0 : Emissive
// g_float_1 : Mul Alpha 
PS_OUT_Flag PS_MASK_TEX_DISTORTION(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	BlendColor.a = Mask * g_float_1;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	
	// Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_DEFAULT_HIT_EF(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;
	float2 FlipUV;

	if(g_int_0 > 0)
		FlipUV  = Get_FlipBookUV(In.vTexUV, g_Time, 0.03, 4, 4);
	else
		FlipUV = Get_FlipBookUV(In.vTexUV, 0, 0.03, 4, 4);
	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));

	float4 OriginColor = g_vec4_0;

	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;

	float4 Mask1 = g_tex_2.Sample(LinearSampler, FlipUV);

	float4 BlendColor = defaultColor * OriginColor * 2.0f;

	BlendColor.a = saturate((Mask * g_float_0) + (Mask1 * g_float_1));

	float4 FinalColor = saturate(BlendColor);


	Out.vColor = CalcHDRColor(FinalColor, g_float_2);
	Out.vColor.a *= g_float_3;
	// Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_BRAINCRUSH_YELLOW_TEX(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	BlendColor.a = Mask * g_float_0;
	float4 FinalColor = saturate(BlendColor);
	FinalColor = CalcHDRColor(FinalColor, g_float_1);

	float4 LineTex = g_tex_2.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_1;
	float4 LineBlend = LineTex * Color * 2.0f;
	float4 LineFinal = saturate(LineBlend);
	LineFinal = CalcHDRColor(LineFinal, g_float_2);
	float LineMask = g_tex_3.Sample(LinearSampler, In.vTexUV).r;
	LineFinal.a = LineTex.r * LineMask * g_float_3;

	Out.vColor = CalcHDRColor(saturate(FinalColor + (LineFinal * g_float_4)), g_float_5);
	// Out.vColor.a = 
	

	// Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_MASK_TEX_DISTORTION_SMOKE(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 4,4)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	BlendColor.a = Mask * g_float_1;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);

	// Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);
	if (g_float_1 <= 0.f)
		discard;
	return Out;
}

PS_OUT_Flag PS_TEX_DISTORTION_DISCARD_OP(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	BlendColor.a = Mask * g_float_1;
	float4 FinalColor = saturate(BlendColor);
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);

	Out.vColor = 0.f;//CalcHDRColor(FinalColor, g_float_0);
	
	// Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);

	if(g_float_1 <= 0)
	{
		discard;
	}

	return Out;
}

// g_tex_0 : Default White
// g_tex_1 : Mask Texture
// g_vec4_0 : OriginColor
// g_float_0 : Emissive
// g_float_1 : Mul Alpha

PS_OUT_Flag PS_MASK_TEX(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);

	float Pattern = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	Out.vColor.a = Pattern * Mask * g_float_1;

	if (Mask < 0.f)
		discard;

	if (g_float_1 <= 0.f)
		discard;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_SPECIAL_G_TRUCK_EXPLODEBASE(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);

	float Pattern = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	Out.vColor.a = Mask * g_float_1;

	if (Mask < 0.f)
		discard;

	if (g_float_1 <= 0.f)
		discard;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_SPECIAL_G_HBEAM_BASEDARK(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	Out.vColor.a = Mask * g_float_1;

	if (Mask < 0.f)
		discard;

	if (g_float_1 <= 0.f)
		discard;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_USE_SAS_GEAR_TEX(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	Out.vColor.a *= g_float_1;

	if (g_float_1 <= 0.f)
		discard;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_SAS_DEAD_LIGHT(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	float4 FinalColor = saturate(BlendColor);
	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);
	Out.vColor.a = Mask * g_float_1;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_MASK_TEX_DISTORTION_CHARGE(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 defaultColor = g_tex_0.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y));
	float4 OriginColor = g_vec4_0;
	float Mask = g_tex_1.Sample(LinearSampler, float2(In.vTexUV.x, In.vTexUV.y)).r;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;
	// BlendColor.a = Mask * g_float_1;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);

	float Pattern = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	Out.vColor.a = Pattern * Mask * g_float_1;

	if (Mask < 0.f)
		discard;

	if (g_float_1 <= 0.f)
		discard;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vColor.a);


	return Out;
}

// g_tex_0 : Default White
// g_tex_1 : Mask
// g_tex_2 : Distortion Texture;

// g_vec4_0 : OriginColor

// g_float_0 : Emissive
// g_float_1 : Mul Alpha
// 
PS_OUT_Flag PS_SEMI_DISTORTION_FOR_SAS(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float4 DistortionTex = g_float_0 * g_tex_0.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0, g_Time)));
	float fWeight = DistortionTex.r * g_float_1;

	float4 defaultColor = g_tex_1.Sample(LinearSampler, In.vTexUV + fWeight);
	float4 OriginColor = g_vec4_0;
	float4 BlendColor = defaultColor * OriginColor * 2.0f;

	float4 FinalColor = saturate(BlendColor);

	float fDissolvePower = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = CalcHDRColor(FinalColor, g_float_2);

	// if (FinalColor.r <= 0.5f)
	// 	Out.vColor = FinalColor;

	Out.vColor.a = defaultColor.a * g_float_3;

	// if (g_float_3 >= fDissolvePower)
	// {
	// 	discard;
	// }

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_EM0650_BULLET_BIRTH_NOR(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;
	float2 TexUV;

	if(g_int_0 == 1)
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 4, 4);
	else
		TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0, 4, 4);

	float4 Default = g_tex_0.Sample(LinearSampler, TexUV);
																
	float4 NormalTex = g_tex_1.Sample(LinearSampler, TexUV);
	NormalTex.xyz = NormalTex.xyz * 2.f - 1.f;

	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(NormalTex.xyz, vViewDir.xyz, 1.5f);

	float4 Vec4Color = g_vec4_0;
	float4 BlendColor = Default * Vec4Color * 2.0f;
	float4 FinalColor = saturate(BlendColor * fFresnel);

	Out.vColor = FinalColor;
	Out.vColor.a = FinalColor.a *g_float_0;
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_Flag PS_BRON_WATER(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;

	float2 randomNormal = g_tex_3.Sample(LinearSampler, In.vTexUV).xy;
	float2 FlowUV = randomNormal * g_float_2 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(g_Time, g_Time));

	float4 DistortionTex = g_tex_4.Sample(LinearSampler, FlowUV);

	float fWeight = DistortionTex.g * g_float_3;

	float4 White = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 Default = g_tex_1.Sample(LinearSampler, In.vTexUV + fWeight);
	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	float4 NormalTex = g_tex_2.Sample(LinearSampler, FlowUV);

	NormalTex.xyz = NormalTex.xyz * 2.f - 1.f;

	float4 vViewDir = g_vCamPosition - In.vWorldPos;

	float fFresnel = FresnelEffect(NormalTex.xyz, vViewDir.xyz, g_float_0);

	float4 Vec4Color = g_vec4_0;
	float4 BlendColor = Default * Vec4Color * 2.0f;
	float4 FinalColor = saturate(BlendColor * fFresnel);

	Out.vColor = FinalColor;
	Out.vColor.a = saturate( g_float_1 * Mask);
	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

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
	pass Distortion
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	}
	//2
	pass Scifi
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SCIFI();
	}

	//3
	pass MaskTexDistortion
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASK_TEX_DISTORTION();
	}

	//4
	pass DistortionFlipBook
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_FLIPBOOK();
	}

	//5
	pass MaskTex
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASK_TEX();
	}

	//6
	pass Mask_Semi_Distortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SEMI_DISTORTION_FOR_SAS();
	}

	//7
	pass Mask_Hit_Decal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASK_HIT();
	}

	// ok su hyeon
	//8
	pass GreenEmissive
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GreenEmissive();
	}
	//9
	pass ColorChange
	{
		SetRasterizerState(RS_Default);

		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ColorChange();
	}
	//10
	pass HpBar
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MixTexture();
	}
	// 11
	pass UVCut
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UVCut();
	}
	// 12
	pass Flow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaOne, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Flow();
	}
	// 13
	pass Alpha_Color
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha_Color();
	}
	// 14
	pass Alpha_Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UVCut();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha_Blend();
	}
	
	// ok su hyeon

	//15
	pass FlipBook_Normal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0650_BULLET_BIRTH_NOR();
	}

	//16
	pass EM_Bullet_Birth_Base
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0650_BULLET_BIRTH_BASE();
	}

	//17
	pass MaskTexDistortion_Discard
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TEX_DISTORTION_DISCARD_OP();
	}

	//18
	pass Bron_Laser_Start
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRON_LASER_START();
	}

	//19
	pass Bron_Bite
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRON_BITE();
	}

	//20
	pass Bron_Water
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRON_WATER();
	}

	//21
	pass FlowDistortionFlipBook
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLOWLEG_FLIPBOOK();
	}

	//22
	pass MaskTexDistortion_NON_Z_WRITE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASK_TEX_DISTORTION_SMOKE();
	}

	//23
	pass DefaultDistortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_DEFAULT();
	}

	//24
	pass KineticDeadFlip
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_KINETIC_DEAD_FLIPBOOK();
	}

	//25
	pass PlayerDistortionA
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_PLAYER();
	}

	//26
	pass Mask_Semi_Distortion_Charge
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MASK_TEX_DISTORTION_CHARGE();
	}

	//27
	pass PlayerDistortionB
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_PLAYER_B();
	}

	//28
	pass SimpleMaskTex
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_USE_SAS_GEAR_TEX();
	}

	//29
	pass Em0220_Explode
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0220_EXPLODE();
	}

	//30
	pass USESASTELEPORT
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_USE_SAS_TELEPORT();
	}

	//31
	pass PlayerTeleportEf
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_TELEPORT_EF();
	}


	//32
	pass DistortionFlipBook_ColorDistortion
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT_CHARGING_1_DISTORTION();
	}

	//33
	pass Em1100_Stamp_Smoke
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_STAMP_SMOKE();
	}

	//34
	pass Em1100_Stamp_Smoke_NoFlip
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_STAMP_SMOKE_NOFLIP();
	}

	//35
	pass Em1100_ElecBulletStart
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_ELEC_BULLET_START();
	}

	//36
	pass Em1100_ElecBulletLoop
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_ELEC_BULLET_LOOP();
	}

	//37
	pass Em1100BulletDead
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_BULLET_DEAD();
	}

	//38
	pass SASDEAD
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_DEAD_LIGHT();
	}

	//39
	pass SpecialG_LargeHit
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPECIAL_G_LARGE_HIT();
	}

	//40
	pass SpecialG_HBim
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPECIAL_G_HBIM_HIT();
	}

	//41
	pass SpecialG_HBeam_Base
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPECIAL_G_HBEAM_BASEDARK();
	}

	//42
	pass SpecialGExplodeBase
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPECIAL_G_TRUCK_EXPLODEBASE();
	}

	//43
	pass SpecialGExplodeLight
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPECAIL_G_EXPLODE_LIGHT();
	}

	//44
	pass Explode_RECT
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 EXPLODE_DEFAULT_RECT();
	}

	//45
	pass Em1200Fear
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1200_FEAR();
	}

	//46
	pass DriveMode_Glow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DRIVEMOD_GLOW();
	}

	//47
	pass Em1200Stamp
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1200_STAMP_DISTORTION();
	}

	//48
	pass em1200_StampImpact
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1200_STAMP_IMPACK_LARGE();
	}

	//49
	pass Em0110SpinAttackEF
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0110_SPINATTACK_HIT_EF();
	}

	//50
	pass EnemyWeakHitEF
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WEAK_HIT_EF();
	}

	//51
	pass BrainCrush_Yellow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAINCRUSH_YELLOW_TEX();
	}

	// 52
		pass Em8200IceDecal
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM8200_ICE_DECAL();
	}

	//53
	pass EM8200_ElecBillboard
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM8200_ELEC_BILLBOARD();
	}
	//54
	pass MonsterSpawnDistortion
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_PLAYER_B();
	}

	//55
	pass DefaultHit_EF_TEX
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT_HIT_EF();
	}

	//56
	pass CH0500_BulletEf
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_AI_CH0500_BULLET_EF();
	}

	//57
	pass Em0220_Explode_Soft
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOFT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0220_EXPLODE_SOFT();
	}

	//58
	pass EM8200_StampImpact
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOFT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM8200_STAMP_IMPACT();
	}

}
