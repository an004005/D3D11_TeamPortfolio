#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"

//For Effect
texture2D		g_FlagTexture;

// ~For Effect

texture2D		g_FlagTextureNonAlpha;

texture2D		g_LDRTexture;
texture2D		g_NormalTexture; 
texture2D		g_DepthTexture;
texture2D		g_DepthMaintainTexture;
texture2D		g_PortraitTexture;

texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;

float g_iWinCX;
float g_iWinCY;

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

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);	

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

// g_tex_1 = noise
// g_tex_0 = distortiontex

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vFlags = g_FlagTexture.Sample(PointSampler, In.vTexUV);
	float4 vFlagsNonAlpha = g_FlagTextureNonAlpha.Sample(PointSampler, In.vTexUV);
	float4 LDR = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	if(vFlags.x == SHADER_DISTORTION)
	{
		float2 randomNormal = g_tex_1.Sample(LinearSampler, In.vTexUV).xy;
		float2 distortionUV = randomNormal * g_float_0 + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(g_Time * 0.5f, g_Time * 0.5f));
		float4 DistortionTex = g_tex_0.Sample(LinearSampler, distortionUV);
		float fWeight = DistortionTex.r * g_float_1;

		float4 OriginColor = g_LDRTexture.Sample(LinearSampler, (In.vTexUV + fWeight));

		Out.vColor = LDR * (1.f - vFlags.a) + OriginColor * vFlags.a;

		Out.vColor.a = 1.f;

		// return Out;
	}
	else if (vFlags.x == SHADER_DISTORTION_STATIC || vFlagsNonAlpha.x == SHADER_DISTORTION_STATIC)
	{
		float2 randomNormal = g_tex_1.Sample(LinearSampler, In.vTexUV).xy;
		float2 distortionUV = randomNormal * g_float_0 + TilingAndOffset(In.vTexUV, float2(5.f, 5.f), float2(0.f, 0.f));
		float4 DistortionTex = g_tex_0.Sample(LinearSampler, distortionUV);
		float fWeight = DistortionTex.r * g_float_1 * 5.f;

		float4 OriginColor = g_LDRTexture.Sample(LinearSampler, (In.vTexUV + fWeight));

		Out.vColor = LDR * (1.f - vFlags.a) + OriginColor * vFlags.a;

		Out.vColor.a = 1.f;

		// if (vFlags.a <= 0.001f)
		// 	discard;
	}
	else if(vFlags.y == SHADER_SCIFI_PLAYER_ATTACK)
	{
		// float2 randomNormal = g_tex_0.Sample(LinearSampler, In.vTexUV).xy;

		// float2 TiltingUV = TilingAndOffset(In.vTexUV, float2(40.f, 40.f), float2( 0.f, 0.f));


		float4 ScifiTex = g_tex_0.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(g_Time * 0.1f, g_Time)) );
		float fWeight = ScifiTex.r * g_float_0;

		float4 ScifiNoiseTex = g_tex_1.Sample(LinearSampler, (In.vTexUV + fWeight));

		float4 InputColor = g_vec4_0;
		float4 Blend = (ScifiTex * ScifiNoiseTex + InputColor);

		float4 OriginColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

		Out.vColor = ((saturate(Blend * OriginColor * (1-vFlags.a) *2)) + LDR *  vFlags.a);

		Out.vColor.a = 1.f;
	}
	else if(vFlags.y == SHADER_HIT_DECAL)
	{
		float4 ScifiTex = g_tex_0.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(g_Time * 0.1f, g_Time)));
		float fWeight = ScifiTex.r * g_float_0;

		float4 ScifiNoiseTex = g_tex_1.Sample(LinearSampler, (In.vTexUV + fWeight));

		float4 InputColor = g_vec4_0;
		float4 Blend = (ScifiTex * ScifiNoiseTex + InputColor);

		float4 OriginColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

		Out.vColor = ((saturate(Blend * OriginColor * (1 - vFlags.a) * 2)) + LDR *  vFlags.a);

		Out.vColor.a = 1.f;
	}
	else
		Out.vColor = LDR;

	return Out;
}


PS_OUT PS_MAIN_WHITE_OUT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	if (g_int_0 == 0)
	{
		float WhiteRatio = saturate(g_float_0);
		float4 vColor =  (float4)1.f;

		Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV) * (1.f - WhiteRatio) + vColor * WhiteRatio;
	}
	else
	{
		float blackratio = saturate(g_float_0);
		float4 vColor =  (float4)0.f;

		Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV) * (1.f - blackratio) + vColor * blackratio;
	}

	return Out;
}


// https://www.shadertoy.com/view/XsfSDs
// g_vec2_0 : 블러 센터 UV값
PS_OUT PS_MAIN_BLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	const int nsamples = 10;
    float2 vBlurCenterUV = g_vec2_0;

	float blurStart = 1.0;
    float blurWidth = 0.15;
    
	float2 uv = In.vTexUV;
    
    uv -= vBlurCenterUV;
    float precompute = blurWidth * (1.0 / float(nsamples - 1));
    
    float4 color = (float4)0;
    for(int i = 0; i < nsamples; i++)
    {
        float scale = blurStart + (float(i)* precompute);
        color += g_LDRTexture.Sample(LinearSampler, uv * scale + vBlurCenterUV);
    }
    
    color /= float(nsamples);
    
	Out.vColor = color;

	return Out;
}

PS_OUT PS_MAIN_XRAY_VISION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	float4 depth = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float4 depthMaintain = g_DepthMaintainTexture.Sample(LinearSampler, In.vTexUV);

	if (depthMaintain.y == 1.f)
		return Out;

	if (depth.y >= depthMaintain.y) // Maintain보다 깊이가 크다 : Maintain이 가려지지 않았다.
		return Out;

	// g_TeamXRayIdx 체크
	if (depth.w == 4.f) // 비쳐 보이는 대상 오브젝트의 ID가 4(맵오브젝트)일 때만 비쳐보인다.
	{
		if (depthMaintain.w == 3.f) // 적군(빨강)
			Out.vColor = float4(241.f / 255.f, 18.f / 255.f, 12.f / 255.f, 1.f);
		else if (depthMaintain.w == 2.f) // 아군(초록)
			Out.vColor = float4(71.f / 255.f, 1.f, 189.f / 255.f, 1.f);
	}


	return Out;
}


PS_OUT PS_MAIN_RADIAL_MASK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float fRadius = g_float_0;
	float3 vPos = g_vec4_0.xyz;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * 300.f;

	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;
	vWorldPos *= fViewZ;

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	if (length(vPos - vWorldPos.xyz) <= fRadius)
	{
		Out.vColor *= 0.3f;
	}

	return Out;
}

// https://halisavakis.com/my-take-on-shaders-color-grading-with-look-up-textures-lut/
PS_OUT PS_MAIN_COLOR_GRADING_LUT_6(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// float blend = g_float_0;
	// float blend_Toon = g_float_1;

	float COLORS = 16.f; // LUT 가로 개수?

	float maxColor = COLORS - 1.0;
	float4 col = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
	float halfColX = 0.5f / 256.f; // LUT텍스쳐 가로 픽셀 사이즈
	float halfColY = 0.5f / 16.f; // LUT텍스쳐 세로 픽셀 사이즈
	float threshold = maxColor / COLORS;

	float xOffset = halfColX + col.r * threshold / COLORS;
	float yOffset = halfColY + col.g * threshold;
	float cell = floor(col.b * maxColor);

	float2 lutPos = float2(cell / COLORS + xOffset, yOffset);

	// float4 gradedCol = g_tex_0.Sample(LinearSampler, lutPos);

	float4 gradedCol = 0.f;
	float blend = 0.f;
	float4 vFlagsNonAlpha = g_FlagTextureNonAlpha.Sample(PointSampler, In.vTexUV);
	 if (vFlagsNonAlpha.y == SHADER_POST_TOON)
	 {
	 	gradedCol = g_tex_1.Sample(LinearSampler, lutPos);
	 	blend = g_float_1;
	 }
	 else if (vFlagsNonAlpha.y == SHADER_POST_ENEMY)
	 {
	 	gradedCol = g_tex_2.Sample(LinearSampler, lutPos);
	 	blend = g_float_2;
	 }
	 // else if (vFlagsNonAlpha.y == SHADER_POST_OBJECTS)
	 // {
	 // 	gradedCol = g_tex_2.Sample(LinearSampler, lutPos);
	 // 	blend = g_float_2;	
	 // }
	 else
	 {
	 	gradedCol = g_tex_0.Sample(LinearSampler, lutPos);
	 	blend = g_float_0;	
	 }

	 
	Out.vColor = lerp(col, gradedCol, blend);

	return Out;
}

static const float3x3 ACESInputMat =
{
    {0.59719, 0.35458, 0.04823},
    {0.07600, 0.90834, 0.01566},
    {0.02840, 0.13383, 0.83777}
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367},
    {-0.10208,  1.10813, -0.00605},
    {-0.00327, -0.07276,  1.07602}
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

PS_OUT PS_MAIN_ACESFitted_7(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

    float3 color = mul(ACESInputMat, vColor.rgb);

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = mul(ACESOutputMat, color);

    color = saturate(color);

	Out.vColor.rgb = color;
	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_MAIN_Penetate_8(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vFlags = g_FlagTextureNonAlpha.Sample(PointSampler, In.vTexUV);
	float4 LDR = g_LDRTexture.Sample(LinearSampler_Clamp, In.vTexUV);
	float fDissolve = g_float_0;
	
	if (vFlags.z == SHADER_TOON_GRAY_INGNORE)
	{
		Out.vColor = LDR;
		Out.vColor.a = 1.f;
	}
	else 
	if (vFlags.z == SHADER_MONSTER_WEAK)
	{
		float3 vWeak = dot(LDR.rgb, float3(0.299, 0.587, 0.114)) * float3(1.f, 1.f, 169.f / 255.f) * 1.5f;
		Out.vColor.rgb = lerp(LDR.rgb, vWeak, fDissolve);
		Out.vColor.a = 1.f;
	}
	else if (vFlags.z == SHADER_MONSTER_INVISIBLE)
	{
		float3 vInvisible = dot(LDR.rgb, float3(0.299, 0.587, 0.114)) * COL_GREEN;
		Out.vColor.rgb = lerp(LDR.rgb, vInvisible, fDissolve);
		Out.vColor.a = 1.f;
	}
	else
	{
		float3 vGray = dot(LDR.rgb, float3(0.299, 0.587, 0.114));
		Out.vColor.rgb = lerp(LDR.rgb, vGray, fDissolve);
		Out.vColor.a = 1.f;
	}

	float fRadius = length(In.vTexUV - float2(0.5f, 0.5f));
	if (fRadius >= 0.4f)
	{
		float2 randomNormal = g_tex_1.Sample(LinearSampler, In.vTexUV).xy;
		float2 distortionUV = randomNormal * fDissolve * 0.001f + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, g_Time/* * 0.2f*/));
		float4 DistortionTex = g_tex_0.Sample(LinearSampler, distortionUV);
		float fWeight = DistortionTex.r * fDissolve * g_float_1 * 0.2;

		float4 OriginColor = g_LDRTexture.Sample(LinearSampler, (In.vTexUV + fWeight));

		float fRatio = Remap(fRadius, float2(0.4f, 0.707f), float2(0.f, 1.f));

		Out.vColor = lerp(Out.vColor, OriginColor, fRatio);
	}

	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_MAIN_Portrait_9(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor = g_vec4_0;
	float4 vOutlineColor = g_vec4_1;
	float fDissolve = g_float_2;
	float fGradient =  g_tex_3.Sample(LinearSampler, In.vTexUV).r;
	bool bUseDissolve = fDissolve > 0.f;
	float fColorRatio = g_float_1;
	float fOutlineRatio = g_float_3;

	const float coord[3] = { -4.f, 0.f, 4.f };
	float fColor = 0.f;

	float4 vPortrait = g_PortraitTexture.Sample(LinearSampler, In.vTexUV);
	if (vPortrait.a != 0.f)
	{
		if (!bUseDissolve)
		{
			[unroll]
			for (int i = 0; i < 3; i++)
			{
				[unroll]
				for (int j = 0; j < 3; j++)
				{
					float fWidthBorderRatio = 4.f / g_iWinCX;

					float2 calcCood = float2(coord[j] / g_iWinCX, coord[i] / g_iWinCY);
					float2 checkUV = In.vTexUV + calcCood;
					fColor += g_PortraitTexture.Sample(LinearSampler, checkUV).a;
				}
			}
			fColor /= 9.f;
		}

		if (!bUseDissolve && fColor != 1.f)
		{

			Out.vColor = vOutlineColor * (1.f - fOutlineRatio);
		}
		else
		{
			float fGrain = g_tex_0.Sample(LinearSampler, TilingAndOffset(In.vTexUV, (float2)10.f, (float2)0.f));
			Out.vColor = lerp(vPortrait, vColor, fColorRatio) * fGrain;

			if (bUseDissolve)
			{
				float noise = g_tex_1.Sample(LinearSampler, TilingAndOffset(In.vTexUV, (float2)4.f, (float2)0.f)).r;
				noise *= g_tex_2.Sample(LinearSampler, In.vTexUV).r;
				noise *= fGradient;
				if (noise < fDissolve)
				{
					Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
				}
				else
				{
					Out.vColor *= noise;
				}
			}
		}
	}
	else
		Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor.a = 1.f;
	return Out;
}

PS_OUT PS_MAIN_Teleport_10(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vFlags = g_FlagTextureNonAlpha.Sample(PointSampler, In.vTexUV);
	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = 1.f;

	float alpha = vFlags.a;

	if (vFlags.z == SHADER_TOON_GRAY_INGNORE && alpha > 0.f)
	{
		float3 colorNoise = g_tex_0.Sample(LinearSampler, In.vTexUV * 50.f).rgb;
		Out.vColor.rgb = lerp(Out.vColor.rgb, colorNoise, alpha);
	}

	return Out;
}

PS_OUT PS_MAIN_SuperSpeed_11(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vFlags = g_FlagTextureNonAlpha.Sample(PointSampler, In.vTexUV);
	Out.vColor = g_LDRTexture.Sample(LinearSampler_Clamp, In.vTexUV);

	if (vFlags.z != SHADER_TOON_GRAY_INGNORE)
	{
		float fRadius = length(In.vTexUV - float2(0.5f, 0.5f));
		if (fRadius >= 0.35f)
		{
			float2 randomNormal = g_tex_1.Sample(LinearSampler, In.vTexUV).xy;
			float2 distortionUV = randomNormal * g_float_0 * 0.1f + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, g_Time * 0.4f));
			float4 DistortionTex = g_tex_0.Sample(LinearSampler, distortionUV);
			float fWeight = DistortionTex.r * g_float_0 * g_float_1 * 0.5f;
		
			float4 OriginColor = g_LDRTexture.Sample(LinearSampler_Clamp, (In.vTexUV + fWeight));
		
			float fRatio = Remap(fRadius, float2(0.35f, 0.707f), float2(0.f, 1.f));
		
			Out.vColor = lerp(Out.vColor, OriginColor, fRatio);
		}

		float blend = g_float_0 * 0.5f; // 0.5를 최대 값으로 사용하기 위함

		float COLORS = 16.f; // LUT 가로 개수?

		float maxColor = COLORS - 1.0;
		float4 col = Out.vColor;
		float halfColX = 0.5f / 256.f; // LUT텍스쳐 가로 픽셀 사이즈
		float halfColY = 0.5f / 16.f; // LUT텍스쳐 세로 픽셀 사이즈
		float threshold = maxColor / COLORS;

		float xOffset = halfColX + col.r * threshold / COLORS;
		float yOffset = halfColY + col.g * threshold;
		float cell = floor(col.b * maxColor);

		float2 lutPos = float2(cell / COLORS + xOffset, yOffset);
		float4 gradedCol = g_tex_2.Sample(LinearSampler, lutPos);
		 
		Out.vColor = lerp(col, gradedCol, blend);

		Out.vColor.a = 1.f;
	}

	return Out;
}

PS_OUT PS_MAIN_DEFAULT_12(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
	
	return Out;
}

float3 ChromaticAberration(float2 vUV, float3 vOffset)
{
	float3 vColor = (float3)0.f;

    float rFlag = g_FlagTextureNonAlpha.Sample(LinearSampler, vUV - (float2)vOffset.x).y;
    float gFlag = g_FlagTextureNonAlpha.Sample(LinearSampler, vUV - (float2)vOffset.y).y;
    float bFlag = g_FlagTextureNonAlpha.Sample(LinearSampler, vUV - (float2)vOffset.z).y; 

	float2 rUV = vUV - (float2)vOffset.x;
	float2 gUV = vUV - (float2)vOffset.y;
	float2 bUV = vUV - (float2)vOffset.z;


	if (rFlag == SHADER_POST_TOON)
		vColor.r = g_LDRTexture.Sample(LinearSampler, rUV).r;
	else
		vColor.r = g_LDRTexture.Sample(LinearSampler, vUV).r;
	
	if (gFlag == SHADER_POST_TOON)
		vColor.g = g_LDRTexture.Sample(LinearSampler, gUV).g;
	else
		vColor.g = g_LDRTexture.Sample(LinearSampler, vUV).g;

	if (bFlag == SHADER_POST_TOON)
		vColor.b = g_LDRTexture.Sample(LinearSampler, bUV).b;
	else
		vColor.b = g_LDRTexture.Sample(LinearSampler, vUV).b;

	return vColor;
}

PS_OUT PS_BRAINFIELD_13(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_int_0 == 0)
	{
		// 색수차
		if (In.vTexUV.x < 0.02 || In.vTexUV.x >= 0.98 || In.vTexUV.y < 0.02 || In.vTexUV.y > 0.98)
		{
			Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
			Out.vColor.a = 1.f;
			return Out;
		}

		if (g_float_0 > 0.f)
		{
			float3 vOffset = float3(-0.007f, 0.007f, 0.02f) * g_float_0;
			Out.vColor.rgb = ChromaticAberration(In.vTexUV, vOffset);
		    Out.vColor.a = 1.f;
			return Out;
		}

		// ~색수차

		Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
		Out.vColor.a = 1.f;		
	}
	else
	{
		if (In.vTexUV.x < 0.02 || In.vTexUV.x >= 0.98 || In.vTexUV.y < 0.02 || In.vTexUV.y > 0.98)
		{
			Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
		}
		else
		{
			float3 vOffset = float3(-0.007f, 0.007f, 0.02f) * g_float_0;
			Out.vColor.rgb = ChromaticAberration(In.vTexUV, vOffset);
		}
		Out.vColor.a = 1.f;

		float blend = g_float_0; // 0.5를 최대 값으로 사용하기 위함

		float COLORS = 16.f; // LUT 가로 개수?

		float maxColor = COLORS - 1.0;
		float4 col = Out.vColor;
		float halfColX = 0.5f / 256.f; // LUT텍스쳐 가로 픽셀 사이즈
		float halfColY = 0.5f / 16.f; // LUT텍스쳐 세로 픽셀 사이즈
		float threshold = maxColor / COLORS;

		float xOffset = halfColX + col.r * threshold / COLORS;
		float yOffset = halfColY + col.g * threshold;
		float cell = floor(col.b * maxColor);

		float2 lutPos = float2(cell / COLORS + xOffset, yOffset);
		float4 gradedCol = g_tex_1.Sample(LinearSampler, lutPos); // blue hue
		 
		Out.vColor.rgb = lerp(col.rgb, gradedCol.rgb, blend);

		Out.vColor.rgb += g_tex_0.Sample(LinearSampler, In.vTexUV);
		Out.vColor.rgb = saturate(Out.vColor.rgb);
	}

	
	return Out;
}

PS_OUT PS_BRAINFIELD_MAP_14(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vFlag = g_FlagTexture.Sample(LinearSampler, In.vTexUV);
	float4 vFlagNonAlpha = g_FlagTextureNonAlpha.Sample(LinearSampler, In.vTexUV);
	float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float fViewZ = vDepthDesc.y * g_Far;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = 1.f;

	if (vFlag.x == SHADER_BRAINFIELD_EFFECT || vFlag.z == SHADER_BRAINFIELD_EFFECT)
		return Out;

	if (vFlag.y == SHADER_POST_OBJECTS || vFlagNonAlpha.y == SHADER_POST_OBJECTS || fViewZ >= g_Far)
	{
		if (g_int_0 == 0) 
		{
			if (g_float_2 > 0.f) // 브레인 필드 뒤 잠깐 보이기용
			{
				vector		vWorldPos;
				vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
				vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
				vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
				vWorldPos.w = 1.0f;
			 
				vWorldPos *= fViewZ;
				vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
				vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

				float fDistance = length(vWorldPos - g_vCamPosition);
				if (fDistance > 50.f && fDistance < g_Far)
				{
					Out.vColor.rgb = lerp(0.f, float3(1.f, 0.f, 0.f), g_float_2);
				}
				else
				{
					Out.vColor.rgb = 0.f;
				}
			}
			else// 기존 맵 검게 지우기
			{
				if (g_float_0 >= 1.f)
					Out.vColor.rgb = 0.f;
				else if (g_float_0 >= 0.7f)
				{
					float fRemap = Remap(g_float_0, float2(0.7f, 1.f), float2(0.f, 1.f));
					Out.vColor.rgb = lerp(float3(1.f, 0.2f, 0.1f), (float3)0.f, fRemap);
				}
				else if (g_float_0 > 0.f)
				{
					float fRemap = Remap(g_float_0, float2(0.f, 0.7f), float2(0.f, 1.f));
					Out.vColor.rgb = lerp(Out.vColor.rgb, float3(1.f, 0.2f, 0.1f), fRemap);
				}
			}
		}
		else if (g_int_0 == 1 && g_float_0 < 1.f)// 맵 다시 보이게 하기(기존 맵 복원 및 브레인 필드맵 보이기용)
		{
			if (vFlag.y == SHADER_POST_OBJECTS)
			{
				Out.vColor.rgb = lerp((float3)0.f, Out.vColor.rgb, g_float_0);
			}
			else
			{
				vector		vWorldPos;
				vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
				vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
				vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
				vWorldPos.w = 1.0f;
			 
				vWorldPos *= fViewZ;
				vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
				vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

				float fRange = g_float_0 * g_Far;

				float fScanRange = g_float_1;
				float fDistance = length(vWorldPos - g_vCamPosition);

				if (fDistance > fRange + fScanRange)
				{
					Out.vColor.rgb = 0.f;
				}
				else if (fDistance <= fRange + fScanRange && fDistance > fRange)
				{
					float fRatio = Remap(fDistance, float2(fRange, fRange + fScanRange), float2(0.f, 1.f));
					Out.vColor.rgb = lerp(float3(1.f, 0.15f, 0.f), (float3)0.f, fRatio);
				}
				else if (fDistance <= fRange && fDistance > fRange - fScanRange)
				{
					float fRatio = Remap(fDistance, float2(fRange - fScanRange, fRange), float2(0.f, 1.f));
					Out.vColor.rgb = lerp(Out.vColor.rgb, float3(1.f, 0.15f, 0.f), fRatio);
				}
			}
		}
	}

	
	return Out;
}

PS_OUT PS_EM8200_BRAINFIELD_MAP_15(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vFlag = g_FlagTexture.Sample(LinearSampler, In.vTexUV);
	float4 vFlagNonAlpha = g_FlagTextureNonAlpha.Sample(LinearSampler, In.vTexUV);
	float4 vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float fViewZ = vDepthDesc.y * g_Far;

	Out.vColor = g_LDRTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = 1.f;

	if (g_int_0 == 0 && g_float_0 > 0.f) // 흑백
	{
		if (g_float_0 >= 1.f) // 흑백
		{
			if (vFlagNonAlpha.z == SHADER_TOON_GRAY_INGNORE || vFlag.z == SHADER_BRAINFIELD_EFFECT)
				Out.vColor.rgb = 1.f;
			else
				Out.vColor.rgb = 0.f;
		}
		else // 점점 흰색
		{
			Out.vColor.rgb = lerp(Out.vColor.rgb, COL_WHITE, g_float_0);
		}
	}
	else if (g_int_0 == 1 && g_float_0 < 1.f) // 기존 맵 천천히 보이기
	{
		if (g_float_0 < 0.1)
		{
			Out.vColor.rgb = lerp(0.f, Out.vColor.rgb, g_float_0 * 10.f);
		}
		else if (vFlag.y == SHADER_POST_OBJECTS || vFlagNonAlpha.y == SHADER_POST_OBJECTS || fViewZ >= g_Far)
		{
			vector		vWorldPos;
			vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
			vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
			vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
			vWorldPos.w = 1.0f;
		 
			vWorldPos *= fViewZ;
			vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
			vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

			float fRange = g_float_0 * g_Far;

			float fScanRange = g_float_1;
			float fDistance = length(vWorldPos - g_vCamPosition);

			if (fDistance > fRange + fScanRange)
			{
				Out.vColor.rgb = 0.f;
			}
			else if (fDistance <= fRange + fScanRange && fDistance > fRange)
			{
				float fRatio = Remap(fDistance, float2(fRange, fRange + fScanRange), float2(0.f, 1.f));
				Out.vColor.rgb = lerp(COL_PURPLE, (float3)0.f, fRatio);
			}
			else if (fDistance <= fRange && fDistance > fRange - fScanRange)
			{
				float fRatio = Remap(fDistance, float2(fRange - fScanRange, fRange), float2(0.f, 1.f));
				Out.vColor.rgb = lerp(Out.vColor.rgb, COL_PURPLE, fRatio);
			}
		}
	}

	return Out;
}


PS_OUT PS_LOADING_16(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vFlag = g_FlagTexture.Sample(LinearSampler, In.vTexUV);
	float4 vFlagNonAlpha = g_FlagTextureNonAlpha.Sample(LinearSampler, In.vTexUV);
	

	if (vFlagNonAlpha.z == SHADER_TOON_GRAY_INGNORE)
	{
		float fGrain = g_tex_0.Sample(LinearSampler, TilingAndOffset(In.vTexUV, (float2)10.f, (float2)0.f));
		Out.vColor.rgb = fGrain * float3(181.f/ 255.f, 173.f/ 255.f, 155.f/ 255.f);
		
	}
	else if (vFlag.z == SHADER_TOON_GRAY_INGNORE)
	{
		Out.vColor.rgb = float3(1.f, 0.f, 0.f);
	}
	else
	{
		Out.vColor = 0.f;
	}

	Out.vColor.rgb = lerp(COL_BLACK, Out.vColor.rgb, g_float_0);

	Out.vColor.a = 1.f;
	return Out;
}

technique11 DefaultTechnique
{
	pass Default_Test
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	// 1
	pass FlashOut
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WHITE_OUT();
	}

	// 2
	pass SimpleBlur
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR();
	}

	// 3
	pass XRayVision
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_XRAY_VISION();
	}

	// 4
	pass RadialMask
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RADIAL_MASK();
	}
	//5
	pass Default_Real
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//6
	pass ColorGradingLUT
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR_GRADING_LUT_6();
	}

	//7
	pass ACESFitted_7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ACESFitted_7();
	}

	//8
	pass Penetrate_8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_Penetate_8();
	}

	//9
	pass Portrait_9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_Portrait_9();
	}

	//10
	pass Teleport_10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_Teleport_10();
	}

	//11
	pass SuperSpeed_11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SuperSpeed_11();
	}

	//12
	pass DefaultFullQuad_12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT_12();
	}

	//13
	pass BrainField_13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAINFIELD_13();
	}

	//14
	pass BrainField_MAP_14
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAINFIELD_MAP_14();
	}

	//15
	pass EM8200_BrainField_MAP_15
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM8200_BRAINFIELD_MAP_15();
	}

	//16
	pass Loading_16
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LOADING_16();
	}
}
