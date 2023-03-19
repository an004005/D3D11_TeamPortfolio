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
// �ܼ� ���ĺ��� �ؽ�ó
// g_tex_0 : UI �ؽ�ó
VS_OUT VS_UI_Tex_Alpha(VS_IN In)	// �� 0
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	return Out;
}

PS_OUT PS_UI_Tex_Alpha(PS_IN In)	// �� 0	���� �������� �׸���
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);
	return Out;
}

VS_OUT VS_UI_Tex_Alpha_Flip(VS_IN In)	// �� 1 ����
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
// HPUI���� �н�
// g_tex_0 : UI �ؽ�ó
// g_float_0 : ���� ����
VS_OUT VS_UI_LowStateBg(VS_IN In)	// �� 2
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	return Out;
}

PS_OUT PS_UI_LowStateBg(PS_IN In)	// �� 2 ���������� ���İ� ����
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV) * float4(1.f, 0.f, 0.f, g_float_0);
	return Out;
}

/*******************
 * UI_SkillIcon
 /********************/
// g_tex_0 : UI �ؽ�ó
// g_float_0 : ���� ����
VS_OUT VS_UI_SkillIcon(VS_IN In)	// �� 3
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	return Out;
}

PS_OUT PS_UI_SkillIcon(PS_IN In)	// �� 3 ������ �������� ���İ� ����
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
// g_tex_0 : �ø��� �ؽ�ó
// g_tex_1 : �÷� �׷����Ʈ �ؽ�ó
VS_OUT VS_CSkill_UseAnim(VS_IN In)	// �� 4
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = Get_FlipBookUV(In.vTexUV, g_float_0, g_float_1, 6, 2);

	return Out;
}

PS_OUT PS_CSkill_UseAnim(PS_IN In)	// �� 4
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
VS_OUT VS_UI_Alpha_Color(VS_IN In)	// �� 5
{
	VS_OUT		Out = (VS_OUT)0;
	matrix matWP = mul(g_WorldMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWP);
	Out.vTexUV = In.vTexUV;
	if (g_int_0 == 1)
		Out.vTexUV.x = 1.f - Out.vTexUV.x;
	return Out;
}

PS_OUT PS_UI_Alpha_Color(PS_IN In)	// �� 5
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
PS_OUT PS_UI_Alpha_Mask_Color(PS_IN In)	// �� 6
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
// �����ʿ��� �������� �پ��� ���α׷��� �� ���̴�
VS_OUT VS_UI_ProgressBar(VS_IN In)	// �� 7
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


PS_OUT PS_UI_ProgressBar(PS_IN In)	// �� 7 (���İ� ó���� 0 �̴� ������ �־�� �Ѵ�)
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
// g_int_0 : bFlip Y Axis (VS_UI_Alpha_Color ���)
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
// g_tex_0 : �ø��� �ؽ�ó
// g_int_0 : �ø��� ���� ����
// g_int_1 : �ø��� ���� ����

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
* UVCut �� 13 : UV ��ǥ�� �߶� ���ϴ� �κи� ����� �ִ� �Լ�
/********************/
// g_vec2_0 : ����� �ε���
// g_vec2_1 : [x] ���η� �ڸ� ����, [y] ���η� �ڸ� ����
VS_OUT VS_UVCut(VS_IN In)	// �� 13
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

// g_int_0 : [0] �̹��� ���� ���, [1] ���� ������ ���� ���
// g_vec4_0 : ������ ����� ���İ�
PS_OUT PS_Alpha_Color(PS_IN In)	// �� 13
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_int_0)
		Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV) * g_vec4_0;
	else
		Out.vColor = g_tex_0.Sample(LinearSampler, In.vTexUV);

	return Out;
}

/*******************
* UVCut �� 15 : ȸ���ϸ鼭 UV �� �����Ѵ�.
/********************/
// g_tex_0 : �ؽ�ó
// g_float_0 : ������ ����
// g_vec4_0 : ���� ����
PS_OUT PS_SASSkillGauge1(PS_IN In) // �� 15
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

	// BOT ���� RIGHTBOT ������ �������.
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
* 16 : �� ���� �ؽ�ó�� �ڸ���, �� ���� �ؽ�ó�� �̿��� �۷ο츦 �ش�.
/********************/
// g_vec2_0 : ù�� ° ����� �ε���
// g_vec2_1 : [x] ���η� �ڸ� ����, [y] ���η� �ڸ� ����
// g_vec2_2 : �ι� ° ����� �ε���
// g_vec2_3 : [x] ���η� �ڸ� ����, [y] ���η� �ڸ� ����
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
// g_int_0 : [0] �̹��� ���� ���, [1] ���� ������ ���� ���
// g_vec4_0 : ������ ����� ���İ�
// g_tex_1 : g_tex_0 �� ���� �ؽ�ó
// g_float_0 : Glow �� ����

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
* UVCut �� 17 : 2���� �ؽ�ó�� �����ͼ� ���´�.

g_tex_0 : ���� ������ �ؽ�ó
g_tex_1 : �̹̽ú� �ؽ�ó
g_vec4_0 : ���� ����
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
* 18 : ���δ� �ؽ�ó�� ��� �̵��ϰ�, ���ηδ� ������ ��ŭ ��������.

// g_float_0 : frame time
// g_tex_0 : �ø��� �ؽ�ó
// g_int_0 : �ø��� ���� ����
// g_int_1 : �ø��� ���� ����
// g_vec2_0 : [x] UV.y �� ���� �� / [y] �ؽ�ó ���� ��
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
* UVCut �� 19 : �ؽ�ó 2���� ����, UV �� �����Ѵ�. 

g_tex_0 : ��� �ؽ�ó
g_tex_1 : ����� �ؽ�ó
g_float_0 : ������ ����
g_vec4_0 : ���� ����
/********************/
PS_OUT PS_SASSkillGauge0(PS_IN In)	// �� 19
{
	PS_OUT         Out = (PS_OUT)0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);

	float4 GlowBase = DefaultTex * g_vec4_0;

	float4 PointTex = g_tex_1.Sample(LinearSampler, In.vTexUV);

	// BOT ���� RIGHTBOT ������ �������.=======
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
* UVCut �� 20 : �ؽ�ó�� ���� �������� ���� �پ��� �Ѵ�.
g_float_0(fProgress) : Gauge (�ִ� 1 �ּ� 0)
g_vec4_0 : ����
/********************/
PS_OUT PS_Flow(PS_IN In)	// �� 20
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
* UVCut �� 21 : ����� �������� �ؽ�ó�� �������� ���ְ� ������ ������ �� �ֽ��ϴ�.
g_vec4_0 : ���� ���� (�ʷϻ� ���)
/********************/
PS_OUT PS_MaskTexture(PS_IN In)	// �� 21
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
* PS_ChoiceTextureReverseFlipBook �� 22 : ���� �ؽ�ó�� ������ �����ߴٰ� �ٽ� ó������ ���ư��� �ڵ� �Դϴ�. -> <- (�ؽ�ó �ѱ�, �۷ο�, UV ����)
g_Time : ���� �ð�.
g_float_0 : Gauge	-> Ŭ���̾�Ʈ���� �Է� �޾ƾ� �Ѵ�.
g_float_1 : FrameTime
g_float_2 : Alpha
g_int_0 : ���η� �ڸ� ����	-> Ŭ���̾�Ʈ���� �Է� �޾ƾ� �Ѵ�.
g_int_1 : ���η� �ڸ� ����
g_int_2 : ����� �ؽ�ó	-> Ŭ���̾�Ʈ���� �Է� �޾ƾ� �Ѵ�.
g_vec4_0 : ���� ����
g_tex_0 : g_int_2 -> 0	Hp
g_tex_3 : g_int_2 -> 0	Hp ���
g_tex_2 : g_int_2 -> 1
g_tex_4 : g_int_2 -> 1
g_tex_2 : g_int_2 -> 2	�ؽ�ó �ѱ� X
g_tex_5 : g_int_2 -> 2	�ؽ�ó �ѱ� X
g_tex_6 : g_int_2 -> 2	��� ���
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
* PS_ReverseFlipBook �� 23 : ���� �ؽ�ó�� ������ �����ߴٰ� �ٽ� ó������ ���ư��� �ڵ� �Դϴ�. -> <- (�ؽ�ó �ѱ�, UV ����)
g_Time : ���� �ð�.
g_float_0 : Gauge	-> Ŭ���̾�Ʈ���� �Է� �޾ƾ� �Ѵ�.
g_float_1 : FrameTime
g_int_0 : ���η� �ڸ� ����	-> Ŭ���̾�Ʈ���� �Է� �޾ƾ� �Ѵ�.
g_int_1 : ���η� �ڸ� ����
g_int_2 : ����� �ؽ�ó	-> Ŭ���̾�Ʈ���� �Է� �޾ƾ� �Ѵ�.
g_vec4_0 : ���� ����
g_tex_0 : g_int_2 -> 0	Hp
g_tex_1 : g_int_2 -> 1
g_tex_2 : g_int_2 -> 0	Hp ���
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
* UVCut �� 24 : ���� ������ int���� ���� ���Ѵ�.
g_vec4_0 : ���� ���� (�ʷϻ� ���)
g_float_0 : VS ���� Gauge ������ ���
g_float_2 : �ؽ�ó �ѱ�� �� (������)
g_int_0 : �ؽ�ó ����
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
* UVCut �� 25 : �ؽ�ó �̹��� �״�� ����ϸ鼭 ���İ� �� ����
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
* UVCut �� 26 : �۷ο�
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
* UVCut �� 27 : �̹̽ú� (������ �ʷϻ�)
g_float_0 : ���� ����
g_vec4_0 : ���� ����
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

// [28] ���� ���� ���� �ϸ鼭 �ؽ�ó ����
PS_OUT PS_TextureChange(PS_IN In)	// �� 28
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

// [29] �ʷϻ� �̹̽ú� �ؽ�ó
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
* MixinColors �� 30 : Main UI ����

/********************/
PS_OUT PS_RedAndGreen(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 color = g_tex_0.Sample(LinearSampler, In.vTexUV);

	// �������� ���, ���� ����� ���İ� ����
	if (color.r >= 0.95 && color.g <= 0.05 && color.b <= 0.05)
	{
		color.r = 0.5; // �������� R ���� ����
		color.g = 0.0; // �������� G ���� 0���� ����
		color.b = 0.0; // �������� B ���� 0���� ����
		color.a *= 0.5; // �������� ���� ���� ����
	}

	// �ʷϻ��� ���, �ٸ� �������� ����
	if (color.r <= 0.05 && color.g >= 0.95 && color.b <= 0.05)
	{
		color.rgb = float3(0.0, 1.0, 1.0); // �ʷϻ��� Cyan���� ����
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
	//0 : ���� �������� �׸���
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

		VertexShader = compile vs_5_0 VS_UVCut();		// �ؽ�ó�� ���ϴ� �κи� ���
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha_Color();	// ���� ����
	}

	//14
	pass ColorChange
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();		// �ƹ��͵� �� �Ѵ�.
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Alpha_Color();	// ���� ����
	}

	//15: Texture 3���� ����, �ܰ����� �ѷ��ϰ��ϰ�, UV �� �ð�������� ���� �����ϴ�.
	pass SASSkillGauge1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SASSkillGauge1();	// ���� ���� �ϸ鼭 �ð���� ���� uv�� �پ��� �þ��.
	}

	//16: �ؽ�ó�� �۷ο� ȿ�� �ֱ�
	pass Glow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_UVCut1();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Glow();	// 0��° �ؽ�ó�� 1��° �ؽ�ó�� ���´�.
	}

	//17: �̹̽ú� (������ �̹����� ���� �̹��� ����
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

	//18: ���ηδ� Time �� �޾Ƽ� �ؽ�ó�� �̵��ϰ�, ���δ� �Է¹޴� int �� ���� �������� ���� �þ�� �Ѵ�.
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

	//19: Texture �� 2�� ����, �ð�������� UV�� �����Ѵ�.
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

	//20 : ������ �ؽ�ó�� ���� ���̰� �� ���̰�
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

	//21 : ����� �������� �ؽ�ó�� ����� ��
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

	//22 : 2���� �ؽ�ó�� �����ؼ� ó������ ������ �ٽ� ó�����θ� �ݺ��Ѵ�. �� ���� ���� (Hp)
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

	//23 : 2���� �ؽ�ó�� �����ؼ� ó������ ������ �ٽ� ó�����θ� �ݺ��Ѵ�. �� ���� ���� (HpBack)
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

	//24 : ���� ���������� ���� �н�..
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

	//25 : ���İ��� ����
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

	//26 : �۷ο� (���� �Ŷ� UV �ٸ��ϴ�.)
	pass GlowTexture
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GlowTexture();	// 0��° �ؽ�ó�� 1��° �ؽ�ó�� ���´�.
	}

	//27: �̹̽ú� (�������� ���������� �̷���� �ؽ�ó)
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

	//28: �ؽ�ó ����
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
	
	//29 : �ʷϻ� �̹̽ú� �ؽ�ó
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
	//30 : ������ �ʷϻ�
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
