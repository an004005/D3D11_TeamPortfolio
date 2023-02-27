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
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

struct VS_OUT1
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float2		vTexUV1 : TEXCOORD1;
};

struct PS_IN1
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float2		vTexUV1 : TEXCOORD1;
};

/*******************
 * UI_Tex_Alpha 
 /********************/
// 단순 알파블렌드 텍스쳐
// g_tex_0 : UI 텍스쳐
VS_OUT VS_UI_Tex_Alpha(VS_IN In)	// → 0
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	return Out;
}

PS_OUT PS_UI_Tex_Alpha(PS_IN In)	// → 0	알파 블랜딩으로 그리기
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	return Out;
}

VS_OUT VS_UI_Tex_Alpha_Flip(VS_IN In)	// → 1 반전
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	Out.vTexUV.x = 1.f - Out.vTexUV.x;
	return Out;
}

/*******************
 * UI_LowStateBg
 /********************/
// HPUI전용 패스
// g_tex_0 : UI 텍스쳐
// g_float_0 : 알파 비율
VS_OUT VS_UI_LowStateBg(VS_IN In)	// → 2
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	return Out;
}

PS_OUT PS_UI_LowStateBg(PS_IN In)	// → 2 빨간색으로 알파값 조절
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV) * float4(1.f, 0.f, 0.f, g_float_0);
	return Out;
}

/*******************
 * UI_SkillIcon
 /********************/
// g_tex_0 : UI 텍스쳐
// g_float_0 : 알파 비율
VS_OUT VS_UI_SkillIcon(VS_IN In)	// → 3
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	return Out;
}

PS_OUT PS_UI_SkillIcon(PS_IN In)	// → 3 본연의 색상으로 알파값 조절
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.a * g_float_0;
	return Out;
}

/*******************
 * CSkill_UseAnim
 /********************/
// g_float_0 : cur time
// g_float_1 : frame time
// g_tex_0 : 플립북 텍스쳐
// g_tex_1 : 컬러 그레디언트 텍스쳐
VS_OUT VS_CSkill_UseAnim(VS_IN In)	// → 4
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = Get_FlipBookUV(In.vTexUV, g_float_0, g_float_1, 6, 2);

	return Out;
}

PS_OUT PS_CSkill_UseAnim(PS_IN In)	// → 4
{
	PS_OUT			Out = (PS_OUT)0;

	float4 color = g_tex_0.Sample(LinearSampler, In.vTexUV);

	Out.vColor = g_tex_1.Sample(LinearSampler, Get_ColorGradientUV(color.r));
	if (color.r == 0.f)
		Out.vColor.a = 0.f;
	else
		Out.vColor.a = saturate(Out.vColor.a * 0.2f);

	return Out;
}

/*******************
 * UI_Alpha_Color
 /********************/
// g_vec4_0 : Color
// g_tex_0 : ui texture
// g_int_0 : bFlip Y Axis
VS_OUT VS_UI_Alpha_Color(VS_IN In)	// → 5
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	if (g_int_0 == 1)
		Out.vTexUV.x = 1.f - Out.vTexUV.x;
	return Out;
}

PS_OUT PS_UI_Alpha_Color(PS_IN In)	// → 5
{
	PS_OUT			Out = (PS_OUT)0;
	if (g_tex_on_0)
		Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV) * g_vec4_0;
	else
		discard;
	return Out;
}

/*******************
 * UI_Alpha_Mask_Color
 /********************/
// g_vec4_0 : Color
// g_tex_0 : Mask UI texture
PS_OUT PS_UI_Alpha_Mask_Color(PS_IN In)	// → 6
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV).r * g_vec4_0;
	return Out;
}

/*******************
 * UI_ProgressBar
 /********************/
// g_tex_0 : ui texture
// g_float_0 : progress ratio
// g_vec4_0 : Color
// 오른쪽에서 왼쪽으로 줄어드는 프로그레스 바 쉐이더
VS_OUT VS_UI_ProgressBar(VS_IN In)	// → 7
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	float3 vPosition = In.vPosition;
	Out.vTexUV = In.vTexUV;
	if (vPosition.x > 0.f)
	{
		Out.vTexUV.x = g_float_0;
		vPosition.x = -0.5f + g_float_0;
	}

	Out.vPosition = mul(float4(vPosition, 1.f), matWP);

	return Out;
}


PS_OUT PS_UI_ProgressBar(PS_IN In)	// → 7 (알파값 처음에 0 이니 조정해 주어야 한다)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV) * g_vec4_0;
	return Out;
}

/*******************
 * UI_Alpha_Mask_Color_AlphaGradient
 /********************/
// g_vec4_0 : Color
// g_tex_0 : Mask UI texture
// g_tex_1 : alpha gradient
// g_int_0 : bFlip Y Axis (VS_UI_Alpha_Color 사용)
PS_OUT PS_UI_Alpha_Mask_Color_AlphaGradient(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV).r * g_vec4_0;
	Out.vColor.a = g_tex_1.Sample(LinearSampler, In.vTexUV).a;

	return Out;
}


/*******************
 * FlipBook
 /********************/
// g_float_0 : cur time
// g_float_1 : frame time
// g_tex_0 : 플립북 텍스쳐
// g_int_0 : 플릭북 가로 개수
// g_int_1 : 플립북 세로 개수

VS_OUT VS_FlipBook(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = Get_FlipBookUV(In.vTexUV, g_float_0, g_float_1, g_int_0, g_int_1);

	return Out;
}

PS_OUT PS_FlipBook(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	return Out;
}


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_COLOR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_vec4_0;
	return Out;
}

// g_float_0 : border size
// g_float_1 : ui width
// g_float_2 : ui height
// g_vec4_0 : border color
// g_vec4_1 : inner color
PS_OUT PS_COLOR_BORDER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float fWidthBorderRatio = g_float_0 / g_float_1;
	float fHeightBorderRatio = g_float_0 / g_float_2;
	if (In.vTexUV.x > fWidthBorderRatio
		&& 1.f - In.vTexUV.x > fWidthBorderRatio
		&& In.vTexUV.y > fHeightBorderRatio
		&& 1.f - In.vTexUV.y > fHeightBorderRatio)
		Out.vColor = g_vec4_1;
	else
		Out.vColor = g_vec4_0;

	return Out;
}

PS_OUT PS_EOG_LOGO(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor = float4(0.278431f, 1.f, 0.74117f, 1.f);
	float4 vColorFill = float4(0.278431f, 1.f, 0.74117f, 0.5f);

	float4 fill = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 mask = g_tex_1.Sample(LinearSampler, In.vTexUV);

	Out.vColor = saturate((fill * vColorFill) + (mask.r * vColor));
	return Out;
}

/*******************
* UVCut → 13 : UV 좌표를 잘라서 원하는 부분만 출력해 주는 함수
/********************/
// g_vec2_0 : 출력할 인덱스
// g_vec2_1 : [x] 가로로 자를 개수, [y] 세로로 자를 개수
VS_OUT VS_UVCut(VS_IN In)	// → 13
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);

	In.vTexUV.x = In.vTexUV.x + g_vec2_0.x;
	In.vTexUV.y = In.vTexUV.y + g_vec2_0.y;

	In.vTexUV.x = In.vTexUV.x / g_vec2_1.x;
	In.vTexUV.y = In.vTexUV.y / g_vec2_1.y;

	Out.vTexUV = In.vTexUV;
	return Out;
}
// g_int_0 : [0] 이미지 색상 사용, [1] 내가 지정한 색상 사용
// g_vec4_0 : 변경할 색상과 알파값
PS_OUT PS_Alpha_Color(PS_IN In)	// → 13
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_int_0)
		Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV) * g_vec4_0;
	else
		Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	return Out;
}

/*******************
* UVCut → 15 : 회전하면서 UV 를 조정한다.
/********************/
// g_tex_0 : 텍스처
// g_float_0 : 게이지 정도
// g_vec4_0 : 색상 조절
PS_OUT PS_RotationGauge(PS_IN In) // → 15
{
	PS_OUT         Out = (PS_OUT)0;

	float4 fillColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float3 center = float3(0.5f, 0.5f, 0.0f);
	float3 top = float3(0.5f, 1.0f, 0.0f);
	float3 curUV = float3(In.vTexUV.xy, 0.0f);
	float angle = 0;

	float3 centerToTop = top - center;
	float3 centerToCurUV = curUV - center;

	centerToTop = normalize(centerToTop);
	centerToCurUV = normalize(centerToCurUV);

	angle = acos(dot(centerToTop, centerToCurUV));
	angle = angle * (180.0f / 3.141592654f); // radian to degree

	angle = (centerToTop.x * centerToCurUV.x - centerToTop.y * centerToCurUV.x > 0.0f) ? angle : (-angle) + 360.0f;

	float condition = 360 * g_float_0;

	if (angle >= condition)
		discard;

	Out.vColor = fillColor * g_vec4_0;

	return Out;
}

/*******************
* UVCut → 16 : 2장의 텍스처를 원하는 크기로 잘라서 쓰고, 섞는다.
/********************/
// g_vec2_0 : 첫번 째 출력할 인덱스
// g_vec2_1 : [x] 가로로 자를 개수, [y] 세로로 자를 개수
// g_vec2_2 : 두번 째 출력할 인덱스
// g_vec2_3 : [x] 가로로 자를 개수, [y] 세로로 자를 개수
VS_OUT1 VS_UVCut2(VS_IN In)
{
	VS_OUT1		Out = (VS_OUT1)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);

	//Out.vTexUV1 = In.vTexUV;

	//In.vTexUV.x = In.vTexUV.x + g_vec2_0.x;
	//In.vTexUV.y = In.vTexUV.y + g_vec2_0.y;

	//In.vTexUV.x = In.vTexUV.x / g_vec2_1.x;
	//In.vTexUV.y = In.vTexUV.y / g_vec2_1.y;

	//Out.vTexUV = In.vTexUV;

	Out.vTexUV1 = In.vTexUV;

	In.vTexUV.x = In.vTexUV.x + g_vec2_0.x;
	In.vTexUV.y = In.vTexUV.y + g_vec2_0.y;

	In.vTexUV.x = In.vTexUV.x / g_vec2_1.x;
	In.vTexUV.y = In.vTexUV.y / g_vec2_1.y;

	Out.vTexUV = In.vTexUV;

	return Out;
}
// g_int_0 : [0] 이미지 색상 사용, [1] 내가 지정한 색상 사용
// g_vec4_0 : 변경할 색상과 알파값
// g_tex_1 : g_tex_0 과 섞을 텍스처
// g_float_0 : Glow 의 정도
PS_OUT PS_Glow(PS_IN1 In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4  vTextureColor;
	float4  vGlowColor;

	vTextureColor = g_tex_1.Sample(LinearSampler, In.vTexUV1) * g_vec4_0;
	vGlowColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor = saturate(vTextureColor + (vGlowColor * g_float_0));

	return Out;
}

technique11 DefaultTechnique
{
	//0 : 알파 블랜딩으로 그리기
	pass UI_Tex_Alpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_Tex_Alpha();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_Tex_Alpha();
	}

	//1
	pass UI_Tex_Alpha_Flip
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_Tex_Alpha_Flip();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_Tex_Alpha();
	}

	// HPUI
	//2
	pass UI_LowStateBg
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_LowStateBg();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_LowStateBg();
	}

	//3
	pass UI_SkillIcon
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_SkillIcon();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_SkillIcon();
	}

	//4
	pass CSkill_UseAnim
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CSkill_UseAnim();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CSkill_UseAnim();
	}

	//5
	pass UI_Alpha_Color
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_Alpha_Color();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_Alpha_Color();
	}

	//6
	pass UI_Alpha_Mask_Color
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_Tex_Alpha();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_Alpha_Mask_Color();
	}

	//7
	pass UI_ProgressBar
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_ProgressBar();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_ProgressBar();
	}

	//8
	pass UI_Alpha_Mask_Color_AlphaGradient
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_Alpha_Color();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_Alpha_Mask_Color_AlphaGradient();
	}

	//9
	pass FlipBook
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_FlipBook();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FlipBook();
	}

	//10
	pass ColorOnly
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_COLOR();
	}

	//11
	pass ColorOnlyBorder
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_COLOR_BORDER();
	}

	//12
	pass EOG_Logo
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EOG_LOGO();
	}

	// sh
	//13
	pass UVCut
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UVCut();		// 텍스처의 원하는 부분만 출력
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha_Color();	// 색상 조정
	}

	//14
	pass ColorChange
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();		// 아무것도 안 한다.
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha_Color();	// 색상 조정
	}

	//15: 스킬 게이지, 아이템 게이지
	pass RotationGauge
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RotationGauge();	// 색상 조정 하면서 시계방향 으로 uv가 줄어들고 늘어난다.
	}

	//16: 텍스처에 글로우 효과 주기
	pass Rsdfasdfdasdfsd
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UVCut2();	// 2개의 텍스처를 섞는다.
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Glow();	// 0번째 텍스처에 1번째 텍스처를 섞는다.
	}

}
