#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

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
	float4		vFlag : SV_TARGET1;
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

	// Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV * 2.f);
	Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	
	return Out;
}

// [1] TargetaArrow
// g_tex_0 : Original Texture
// g_tex_1 : Emissive Texture
// g_vec4_0 : Color
// g_float_0 : Century
// g_float_1 : ratio
// g_float_2 : Alpha
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

// [2] 빛나는
PS_OUT PS_Diamond(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor = CalcHDRColor(Texture, g_float_0);
	//Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);
	//Out.vColor.a = Out.vColor.a * g_float_1;

	return Out;
}

// [3] 2 에서 추가적으로 색상과 알파값 까지 조절
PS_OUT PS_UI_Tex_Alpha(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (0 == g_int_0)	// 색상 변경해서
	{
		float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);

		float4 BlendColor = Texture * g_vec4_0 * 2.0f;
		float4 FinalColor = saturate(BlendColor);
		float4 HDRColor = saturate(FinalColor + Texture * g_float_0);
		Out.vColor = CalcHDRColor(HDRColor, g_float_1);
	}
	else if (1 == g_int_0) // 안 보이도록
	{
		return Out;
	}
	else if (2 == g_int_0)	// 원본 텍스처 사용
	{
		float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);

		float4 HDRColor = saturate(Texture * g_float_0);
		Out.vColor = CalcHDRColor(HDRColor, g_float_1);
	}
	if (3 == g_int_0)	// 색상 에서 a 값은 g_float_0 로 조절
	{
		float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);

		float3 BlendColor = Texture.rgb * g_vec4_0.rgb * 2.0f;
		float3 FinalColor = saturate(BlendColor);
		float4 HDRColor;
		HDRColor.rgb = saturate(FinalColor + Texture * g_float_0);
		HDRColor.a = g_float_0;
		Out.vColor = CalcHDRColor(HDRColor, g_float_1);
	}

	return Out;
}

PS_OUT PS_BRAINFIELD_FLOOR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vPlayerPos = g_vec4_0;
	float fDistance = length(vPlayerPos.xyz - In.vWorldPos.xyz);
	if (fDistance > 5.f)
		discard;

	float fLineColor = g_tex_0.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(200.f, 20.f), 0.f)).r * 0.5f;
	fLineColor += g_tex_0.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(200.f, 10.f), float2(0.5f, 0.5f))).r * 0.5f;
	//
	float2 vRotateUV = rotateUV_Degree(In.vTexUV, 90.f);
	fLineColor += g_tex_1.Sample(LinearSampler, TilingAndOffset(vRotateUV, float2(200.f, 100.f), 0.f)).r * 0.2f;
	fLineColor += g_tex_2.Sample(LinearSampler,  TilingAndOffset(vRotateUV, float2(200.f, 50.f), 0.f)).r * 0.2f;
	fLineColor = saturate(fLineColor);

   if (fLineColor < 0.001f)
      discard;

   Out.vColor = lerp(float4(1.f, 0.f, 0.f, 0.f), 1.f, fLineColor * 0.01f);
   Out.vColor.rgb = Out.vColor.rgb * (1.f - fDistance * 0.2f) * 5.f;
   Out.vColor.a = fLineColor * (1.f - fDistance * 0.2f);
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
	pass GreenEmissive
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GreenEmissive();
	}
	
	// 2
	pass Diamond
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Diamond();
	}

	// 3
	pass UI_Tex_Alpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_Tex_Alpha();
	}

	// 4
	pass BrainFieldFloor
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAINFIELD_FLOOR();
	}
	
}
