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
// 단순 알파블렌드 텍스처
// g_tex_0 : UI 텍스처
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
// g_tex_0 : UI 텍스처
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
// g_tex_0 : UI 텍스처
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
// g_tex_0 : 플립북 텍스처
// g_tex_1 : 컬러 그레디언트 텍스처
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
// g_float_0 : frame time
// g_tex_0 : 플립북 텍스처
// g_int_0 : 플릭북 가로 개수
// g_int_1 : 플립북 세로 개수

VS_OUT VS_FlipBook(VS_IN In)	// ->9
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_FlipBook(PS_IN In)	// ->9
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, g_Time, g_float_0, g_int_0, g_int_1));
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
PS_OUT PS_SASSkillGauge1(PS_IN In) // → 15
{
	PS_OUT         Out = (PS_OUT)0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 DefaultTexAnother = DefaultTex;
	
	float4 GlowBase = DefaultTex * g_vec4_0;

	float4 glow = g_tex_1.Sample(LinearSampler, In.vTexUV);

	GlowBase.a = glow.a;
	float4 LineTex = DefaultTexAnother * g_vec4_1;

	float4 PointTex = g_tex_2.Sample(LinearSampler, In.vTexUV);
	
	LineTex.a = PointTex.r;

	// BOT 에서 RIGHTBOT 순서로 사라진다.
	float3 center = float3(0.5f, 0.5f, 0.0f);
	float3 top = float3(0.5f, 0.0f, 0.0f);
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

	Out.vColor = saturate(GlowBase + LineTex * g_float_1);
	Out.vColor.a = PointTex.r;
	return Out;
}

/*******************
* 16 : 한 장의 텍스처를 자르고, 두 장으 텍스처를 이용해 글로우를 준다.
/********************/
// g_vec2_0 : 첫번 째 출력할 인덱스
// g_vec2_1 : [x] 가로로 자를 개수, [y] 세로로 자를 개수
// g_vec2_2 : 두번 째 출력할 인덱스
// g_vec2_3 : [x] 가로로 자를 개수, [y] 세로로 자를 개수
VS_OUT1 VS_UVCut1(VS_IN In)
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

/*******************
* UVCut → 17 : 2장의 텍스처를 가져와서 섞는다.

g_tex_0 : 섞을 색상의 텍스처
g_tex_1 : 이미시브 텍스처
g_vec4_0 : 섞을 색상
/********************/
float4 g_vEmissiveColor : EMISSIVE;

VS_OUT1 VS_MAIN1(VS_IN In)
{
	VS_OUT1		Out = (VS_OUT1)0;

	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);
	Out.vTexUV1 = In.vTexUV;

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_Emissive(PS_IN1 In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vEdiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 vEmissive = g_tex_1.Sample(LinearSampler, In.vTexUV1);
	vEmissive = lerp(vEmissive, g_vec4_0, 0.85f);

	Out.vColor = vEdiffuse * vEmissive;

	return Out;

	//PS_OUT			Out = (PS_OUT)0;

	//float4 vTexture0 = g_tex_0.Sample(LinearSampler, In.vTexUV);
	//float4 vTexture1 = g_tex_1.Sample(LinearSampler, In.vTexUV1);

	//vTexture1.a = 0.0f;

	//Out.vColor = saturate(vTexture0 + vTexture1) * g_vec4_0;

	//return Out;
}

/*******************
* 18 : 가로는 텍스처가 계속 이동하고, 세로로는 지정한 만큼 보여진다.

// g_float_0 : frame time
// g_tex_0 : 플립북 텍스처
// g_int_0 : 플릭북 가로 개수
// g_int_1 : 플립북 세로 개수
// g_vec2_0 : [x] UV.y 에 더할 값 / [y] 텍스처 나눌 값
/********************/

VS_OUT VS_FlipBookCut(VS_IN In)	// ->18
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);

	In.vTexUV.y = In.vTexUV.y + g_vec2_0.x;

	In.vTexUV.y = In.vTexUV.y / g_vec2_0.y;

	Out.vTexUV = In.vTexUV;
	return Out;
}

PS_OUT PS_FlipBookCut(PS_IN In)	// ->18
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, g_Time, g_float_0, g_int_0, g_int_1));
	return Out;
}

/*******************
* UVCut → 19 : 텍스처 2장을 섞고, UV 를 조정한다. 

g_tex_0 : 흰색 텍스처
g_tex_1 : 출력할 텍스처
g_float_0 : 게이지 정도
g_vec4_0 : 색상 조절
/********************/
PS_OUT PS_SASSkillGauge0(PS_IN In)	// → 19
{
	PS_OUT         Out = (PS_OUT)0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 GlowBase = DefaultTex * g_vec4_0;

	float4 PointTex = g_tex_1.Sample(LinearSampler, In.vTexUV);

	// BOT 에서 RIGHTBOT 순서로 사라진다.=======
	float3 center = float3(0.5f, 0.5f, 0.0f);
	float3 top = float3(0.5f, 0.0f, 0.0f);
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
	// ==============================

	Out.vColor = saturate(GlowBase);
	Out.vColor.a = PointTex.r;
	return Out;
}

/*******************
* UVCut → 20 : 텍스처에 따라서 게이지가 차고 줄어들고 한다.
g_float_0(fProgress) : Gauge (최대 1 최소 0)
g_vec4_0 : 색상
/********************/
PS_OUT PS_Flow(PS_IN In)	// → 20
{
	PS_OUT			Out = (PS_OUT)0;

	float fProgress = g_float_0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float ProgressMask = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	if (1.f - ProgressMask >= fProgress)
		discard;

	float4 GlowBase = DefaultTex * g_vec4_0;

	float4 PointTex = g_tex_1.Sample(LinearSampler, In.vTexUV);

	Out.vColor = saturate(GlowBase);

	Out.vColor.a = PointTex.r;

	return Out;
}

/*******************
* UVCut → 21 : 배경이 검정색인 텍스처를 검정색을 없애고 색상을 변경할 수 있습니다.
g_vec4_0 : 색상 변경 (초록색 배경)
/********************/
PS_OUT PS_MaskTexture(PS_IN In)	// → 21
{
	PS_OUT         Out = (PS_OUT)0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 GlowBase = DefaultTex * g_vec4_0;

	float4 PointTex = g_tex_1.Sample(LinearSampler, In.vTexUV);

	Out.vColor = saturate(GlowBase);
	Out.vColor.a = PointTex.g;
	return Out;
}

/*******************
* PS_ChoiceTextureReverseFlipBook → 22 : 나눈 텍스처의 끝까지 도달했다가 다시 처음으로 돌아가는 코드 입니다. -> <- (텍스처 넘김, 글로우, UV 조절)
g_Time : 현재 시간.
g_float_0 : Gauge	-> 클라이언트에서 입력 받아야 한다.
g_float_1 : FrameTime
g_float_2 : Alpha
g_int_0 : 가로로 자를 개수	-> 클라이언트에서 입력 받아야 한다.
g_int_1 : 세로로 자를 개수
g_int_2 : 사용할 텍스처	-> 클라이언트에서 입력 받아야 한다.
g_vec4_0 : 색상 변경
g_tex_0 : g_int_2 -> 0	Hp
g_tex_3 : g_int_2 -> 0	Hp 배경
g_tex_2 : g_int_2 -> 1
g_tex_4 : g_int_2 -> 1
g_tex_2 : g_int_2 -> 2	텍스처 넘김 X
g_tex_5 : g_int_2 -> 2	텍스처 넘김 X
g_tex_6 : g_int_2 -> 2	흰색 배경
/********************/
PS_OUT PS_ChoiceTextureReverseFlipBook(PS_IN In)	// ->22
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Texture;
	float Mask;

	float4 DefaultWhite = g_tex_6.Sample(LinearSampler, In.vTexUV);
	float4 OriginColor = g_vec4_0;

	if (0 == g_int_2)
	{
		Texture = g_tex_0.Sample(LinearSampler, Get_ReverseFlipBookUV(In.vTexUV, g_Time, g_float_1, g_int_0, g_int_1));
		Mask = g_tex_3.Sample(LinearSampler, Get_ReverseFlipBookUV(In.vTexUV, g_Time, g_float_1, g_int_0, g_int_1)).a;

	}
	else if (1 == g_int_2)
	{
		Texture = g_tex_1.Sample(LinearSampler, Get_ReverseFlipBookUV(In.vTexUV, g_Time, g_float_1, g_int_0, g_int_1));
		Mask = g_tex_4.Sample(LinearSampler, Get_ReverseFlipBookUV(In.vTexUV, g_Time, g_float_1, g_int_0, g_int_1)).a;

	}
	else if (2 == g_int_2)
	{
		Texture = g_tex_2.Sample(LinearSampler, In.vTexUV);
		Mask = g_tex_5.Sample(LinearSampler, In.vTexUV).a;
	
	}

	float4 BlendColor = DefaultWhite * OriginColor;
	Out.vColor = saturate(BlendColor + Texture * g_float_2);
	Out.vColor.a = Mask * 1.0f;

	return Out;
}

/*******************
* PS_ReverseFlipBook → 23 : 나눈 텍스처의 끝까지 도달했다가 다시 처음으로 돌아가는 코드 입니다. -> <- (텍스처 넘김, UV 조절)
g_Time : 현재 시간.
g_float_0 : Gauge	-> 클라이언트에서 입력 받아야 한다.
g_float_1 : FrameTime
g_int_0 : 가로로 자를 개수	-> 클라이언트에서 입력 받아야 한다.
g_int_1 : 세로로 자를 개수
g_int_2 : 사용할 텍스처	-> 클라이언트에서 입력 받아야 한다.
g_vec4_0 : 색상 변경
g_tex_0 : g_int_2 -> 0	Hp
g_tex_1 : g_int_2 -> 1
g_tex_2 : g_int_2 -> 0	Hp 배경
/********************/
PS_OUT PS_ReverseFlipBook(PS_IN In)	// ->23
{
	PS_OUT			Out = (PS_OUT)0;

	if (0 == g_int_2)
	{
		Out.vColor = g_tex_0.Sample(LinearSampler, Get_ReverseFlipBookUV(In.vTexUV, g_Time, g_float_1, g_int_0, g_int_1)) * g_vec4_0;
	}
	else if (1 == g_int_2)
	{
		Out.vColor = g_tex_1.Sample(LinearSampler, Get_ReverseFlipBookUV(In.vTexUV, g_Time, g_float_1, g_int_0, g_int_1)) * g_vec4_0;
	}
	else if (2 == g_int_2)
	{
		Out.vColor = g_tex_2.Sample(LinearSampler, In.vTexUV)  * g_vec4_0;
	}

	return Out;


	//PS_OUT			Out = (PS_OUT)0;
	//
	//Out.vColor = g_tex_0.Sample(LinearSampler, Get_ReverseFlipBookUV(In.vTexUV, g_Time, g_float_0, g_int_0, g_int_1)) * g_vec4_0;

	//return Out;
}

/*******************
* UVCut → 24 : 염력 게이지 int값에 따라서 변한다.
g_vec4_0 : 색상 변경 (초록색 배경)
g_float_0 : VS 에서 Gauge 정도로 사용
g_float_2 : 텍스처 넘기는 값 (프레임)
g_int_0 : 텍스처 선택
/********************/
PS_OUT PS_PsychokinesisFlipBook(PS_IN In)	// ->24
{
	PS_OUT			Out = (PS_OUT)0;

	float4 OriginalTexture;
	float4  vGlowColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	if (0 == g_int_0)
	{
		OriginalTexture = g_tex_1.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, g_Time, g_float_1, 10, 1));
	}
	else if (1 == g_int_0)
	{
		OriginalTexture = g_tex_2.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, g_Time, g_float_1, 10, 1));
	}
	else if (2 == g_int_0)
	{
		OriginalTexture = g_tex_3.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, g_Time, g_float_1, 8, 1));
	}

	float4 TopColor = g_vec4_0;
	float4 MixTexture = saturate(TopColor * OriginalTexture * 2.0f);
	float4 BottomColor = g_vec4_1 * (1.0f - In.vTexUV.y);
	float3 FinalTexture = MixTexture.rgb * In.vTexUV.y;

	Out.vColor.rgb = saturate((FinalTexture + BottomColor.rgb) + vGlowColor * g_float_2);
	Out.vColor.a = OriginalTexture.a;

	return Out;
}

/*******************
* UVCut → 25 : 텍스처 이미지 그대로 사용하면서 알파값 만 조절
g_float_0 : Alpha
/********************/
PS_OUT PS_UI_ChangeAlpha(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.a - (1.f - g_float_0);
	return Out;
}

/*******************
* UVCut → 26 : 글로우
g_float_0 : Alpha
/********************/
PS_OUT PS_GlowTexture(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4  vTextureColor = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float4  vGlowColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor = saturate(vTextureColor + (vGlowColor * g_float_0))  * g_vec4_0;

	return Out;
}

/*******************
* UVCut → 27 : 이미시브 (빨간색 초록색)
g_float_0 : 섞는 정도
g_vec4_0 : 색상 변경
/********************/
PS_OUT PS_RedEmissive(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Emissive = g_tex_1.Sample(LinearSampler, In.vTexUV);
	Emissive = lerp(Emissive, g_vec4_0, g_float_0);

	Out.vColor = Texture * Emissive;

	return Out;

	//PS_OUT			Out = (PS_OUT)0;

	//float4 vTexture0 = g_tex_0.Sample(LinearSampler, In.vTexUV);
	//float4 vTexture1 = g_tex_1.Sample(LinearSampler, In.vTexUV);

	//vTexture1.a = 0.0f;

	//Out.vColor = saturate(vTexture0 + vTexture1) * g_vec4_0;

	//return Out;
}

// [28] 색상 조정 가능 하면서 텍스처 변경
PS_OUT PS_TextureChange(PS_IN In)	// → 28
{
	PS_OUT			Out = (PS_OUT)0;

	if (0 == g_int_0)
	{

		float fProgress = g_float_0;

		float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
		float ProgressMask = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

		if (1.f - ProgressMask >= fProgress)
			discard;

		float4 GlowBase = DefaultTex * g_vec4_0;

		float4 PointTex = g_tex_1.Sample(LinearSampler, In.vTexUV);

		Out.vColor = saturate(GlowBase);

		Out.vColor.a = PointTex.r;

	}
	else if (1 == g_int_0)
		Out.vColor = g_tex_3.Sample(LinearSampler, In.vTexUV);

	return Out;
}

// [29] 초록색 이미시브 텍스처
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

/*******************
* MixinColors → 30 : Main UI 에서

/********************/
PS_OUT PS_RedAndGreen(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 color = g_tex_0.Sample(LinearSampler, In.vTexUV);

	// 빨간색의 경우, 색상값 변경과 알파값 조절
	if (color.r >= 0.95 && color.g <= 0.05 && color.b <= 0.05)
	{
		color.r = 0.5; // 빨간색의 R 값을 조절
		color.g = 0.0; // 빨간색의 G 값을 0으로 변경
		color.b = 0.0; // 빨간색의 B 값을 0으로 변경
		color.a *= 0.5; // 빨간색의 알파 값을 조절
	}

	// 초록색의 경우, 다른 색상으로 변경
	if (color.r <= 0.05 && color.g >= 0.95 && color.b <= 0.05)
	{
		color.rgb = float3(0.0, 1.0, 1.0); // 초록색을 Cyan으로 변경
	}

	Out.vColor = color;

	//float fProgress = g_float_0;

	//float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	//float ProgressMask = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	//if (1.f - ProgressMask >= fProgress)
	//	discard;

	//float4 GlowBase = DefaultTex * g_vec4_0;

	//float4 PointTex = g_tex_1.Sample(LinearSampler, In.vTexUV);

	//Out.vColor = saturate(GlowBase);

	//Out.vColor.a = PointTex.r;

	return Out;

	//PS_OUT			Out = (PS_OUT)0;

	//float4 Texture = g_tex_0.Sample(LinearSampler, In.vTexUV);
	//float4 Emissive = g_tex_1.Sample(LinearSampler, In.vTexUV);
	//Emissive = lerp(Emissive, g_vec4_0, g_float_0);

	//Out.vColor = Texture * Emissive;

	//return Out;
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
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

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

	//15: Texture 3장을 섞고, 외곽선을 뚜렸하게하고, UV 를 시계방향으로 조절 가능하다.
	pass SASSkillGauge1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SASSkillGauge1();	// 색상 조정 하면서 시계방향 으로 uv가 줄어들고 늘어난다.
	}

	//16: 텍스처에 글로우 효과 주기
	pass Glow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UVCut1();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Glow();	// 0번째 텍스처에 1번째 텍스처를 섞는다.
	}

	//17: 이미시브 (검정색 이미지와 원본 이미지 섞기
	pass Emissive
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN1();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Emissive();
	}

	//18: 가로로는 Time 을 받아서 텍스처가 이동하고, 세로는 입력받는 int 에 따라가 보여지는 값이 늘어나야 한다.
	pass FlipBookCut
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_FlipBookCut();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FlipBookCut();
	}

	//19: Texture 을 2장 섞고, 시계방향으로 UV를 조절한다.
	pass SASSkillGauge0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SASSkillGauge0();
	}

	//20 : 빨간색 텍스처에 따라서 보이고 안 보이고
	pass MaskedProgress
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Flow();
	}

	//21 : 배경이 검정색인 텍스처를 사용할 때
	pass MaskTexture
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MaskTexture();
	}

	//22 : 2개의 텍스처를 선택해서 처음부터 끝에서 다시 처음으로를 반복한다. 색 선택 가능 (Hp)
	pass ChoiceTextureReverseFlipBook
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaOne, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_ProgressBar();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ChoiceTextureReverseFlipBook();
	}

	//23 : 2개의 텍스처를 선택해서 처음부터 끝에서 다시 처음으로를 반복한다. 색 선택 가능 (HpBack)
	pass ReverseFlipBook
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_ProgressBar();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ReverseFlipBook();
	}

	//24 : 염력 게이지만을 위한 패스..
	pass TelekinesisGauge
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UI_ProgressBar();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PsychokinesisFlipBook();
	}

	//25 : 알파값만 조절
	pass ChangeAlpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UI_ChangeAlpha();
	}

	//26 : 글로우 (위에 거랑 UV 다릅니다.)
	pass GlowTexture
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GlowTexture();	// 0번째 텍스처에 1번째 텍스처를 섞는다.
	}

	//27: 이미시브 (검정색과 빨간색으로 이루어진 텍스처)
	pass RedEmissive
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RedEmissive();
	}

	//28: 텍스처 변경
	pass TextureChange
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TextureChange();
	}
	
	//29 : 초록색 이미시브 텍스처
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



	// Main UI
	//30 : 빨간색 초록색
	pass MixinColors
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff); // BS_BlackCut

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RedAndGreen();
	}
}
