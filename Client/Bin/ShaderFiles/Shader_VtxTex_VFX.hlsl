#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

texture2D GrapTexture;

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

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
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

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	// Out.vColor = float4(1.f, 1.f, 1.f, 1.f);

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

	Out.vColor = saturate(BlendColor + Texture * 0.25f) * 3.8f;
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

// g_tex_0 : Default White
// g_vec4_0 : Origin Color

PS_OUT_Flag PS_DISTORTION_FLIPBOOK(PS_IN In)
{
	PS_OUT_Flag			Out = (PS_OUT_Flag)0;


	float4 Default_White = g_tex_0.Sample(LinearSampler, In.vTexUV); // Not Use Plz Fix
	//////
	float2 TexUV = Get_FlipBookUV(In.vTexUV, g_Time, 0.05, 4, 4);
	float  Mask = g_tex_1.Sample(LinearSampler, TexUV).r;
	/////
	// float4 OriginColor = g_vec4_0;
	// float4 BlendColor = Default_White * OriginColor * 2.0f;
	// float4 FinalColor = saturate(BlendColor);

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Mask);

	// Out.vColor.r = 1.f; 
	Out.vColor.a = 0.f;

	// Out.vColor.a = 1 - Out.vColor.r;



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
	Out.vFlag = float4(0.f, 0.f, 0.f, Mask);

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
	// BlendColor.a = Mask * g_float_1;
	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0);

	float Pattern = g_tex_2.Sample(LinearSampler, In.vTexUV).r;
	Out.vColor.a = Pattern * Mask * g_float_1;

	if (Mask < 0.f)
		discard;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);

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
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
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
		SetRasterizerState(RS_Default);
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
		SetDepthStencilState(DS_Default, 0);
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
		SetBlendState(BS_AlphaOne, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

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
		SetBlendState(BS_AlphaOne, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

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

	//14
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

	//15
	pass EM_Bullet_Birth_Base
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM0650_BULLET_BIRTH_BASE();
	}

}
