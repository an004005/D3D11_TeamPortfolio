#include "Shader_Utils.h"
#include "Shader_Defines.h"
#include "Shader_Params.h"

matrix			g_BoneMatrices[512];

Texture2D g_Weak01;
Texture2D g_Vanish_Noise;
Texture2D g_Weak_Noise;
Texture2D g_WaveTile;
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
	float4		vRMA : SV_TARGET3;
	float4		vOutline : SV_TARGET4;
	float4		vFlag : SV_TARGET5;
};

struct PS_OUT_NONLIGHT
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float flags = SHADER_DEFAULT;
	Out.vDiffuse = (float4)1.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, flags);
	Out.vFlag = 0.f;
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

// g_float_0  : 사망 디졸브
// g_int_0 : 상태이상 플래그
PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float fDissolve = g_float_0;
	float fEmissive = 0.f;
	float flags = SHADER_DEFAULT;


	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (Out.vDiffuse.a < 0.01f)
		discard;

	if (fDissolve > 0.f)
	{
		fEmissive = 2.f;

		flags = SHADER_NONE_SHADE;
		float4 VanishNoise = g_Vanish_Noise.Sample(LinearSampler, TilingAndOffset(In.vTexUV, float2(5.f, 5.f), float2(0.f, 0.f)));
		float chanG = VanishNoise.g;
		float chanB = VanishNoise.b;
		if (fDissolve > 0.3f)
		{
			float remaped = Remap(fDissolve, float2(0.3f, 1.f), float2(0.f, 1.f));
			if (1.f - remaped <= chanG)
				discard;
		}

		if (chanB < fDissolve)
		{
			fEmissive = 5.f;
		}

		Out.vDiffuse.rgb *= COL_BURNOUT;
	}
	else
	{
		Out.vNormal = NormalPacking(In);
		if (g_tex_on_2)
			Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
		else
			Out.vRMA = float4(1.f, 0.f, 1.f, 0.f);

		int iDebuffState = g_int_0;
		if (iDebuffState == 1) // fire
		{
			float3 vNormal = Out.vNormal.xyz * 2.f - 1.f;
			float4 vViewDir = g_vCamPosition - In.vWorldPos;
			float fFresnel = FresnelEffect(vNormal.xyz, vViewDir.xyz, 2.5f);
			Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, COL_FIRE , fFresnel);
			if (fFresnel > 0.5f)
				fEmissive = 2.f;
		}
		else if (iDebuffState == 2) // oil
		{
			float3 vDefaultNormal = Out.vNormal.xyz * 2.f - 1.f;

			float4 vWaveTile = g_WaveTile.Sample(LinearSampler, TilingAndOffset(In.vTexUV, (float2)5.f, float2(g_Time * 0.1f, 0.f)));
			float3 vWetNormal = vWaveTile.xyz * 2.f - 1.f;
			float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal, In.vNormal.xyz);
			vWetNormal = normalize(mul(vWetNormal, WorldMatrix));

			vWetNormal = lerp(vDefaultNormal, vWetNormal, vWaveTile.a);
			Out.vNormal = vector(vWetNormal * 0.5f + 0.5f, 0.f);

			Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, COL_OIL, vWaveTile.a);
			fEmissive = (vWaveTile.a * 0.5f);
		}
	}
	


	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, fEmissive, flags);
	Out.vFlag = float4(0.f, SHADER_POST_ENEMY, 0.f, 0.f);

	return Out;

}

// g_float_0  : 사망 디졸브
// g_float_1 : 재생성 디솔브, 0 -> 1
// g_int_0 : 상태이상 플래그
PS_OUT PS_MAIN_EM320_Water_2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float fDissolve = g_float_0;
	float fRegen = g_float_1;
	if (fDissolve > 0.f)
	{
		fRegen = 1.f - fDissolve;
	}

	if (fRegen < 0.f)
	{
		discard;
	}
	else
	{
		float fEmissive = 0.f;
		float flags = SHADER_DEFAULT;
		Out.vNormal = NormalPacking(In);

		if (1.f - In.vTexUV.y >= fRegen)
			discard;

		float4 flame = 1.f;
		if (fRegen < 1.f)
		{
			float fSmoothness = 0.7f;
			float fLineWidth = 0.2f;
			float2 Distortion = (float2)0.3;
			float time = frac(g_Time * 0.5f);
			float2 OffsetUV = TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, time));
			float2 randomNormal = g_tex_7.Sample(LinearSampler, In.vTexUV).xy;

			float flameUp;
			float flameDown;

			{
				float remap = Remap(fRegen, float2(0, 1), float2(-fSmoothness, 1));
				float smoothDissolve = smoothstep(remap, remap + fSmoothness, 1.f - In.vTexUV.y);

				float2 Distortion = (float2)0.3;
				float2 distortionUV = randomNormal * Distortion + OffsetUV;
				float4 noise = g_tex_7.Sample(LinearSampler, distortionUV);

				flameUp = step(smoothDissolve, noise.a);
			}

			{
				fRegen -= fLineWidth;

				float remap = Remap(fRegen, float2(0, 1), float2(-fSmoothness, 1));
				float smoothDissolve = smoothstep(remap, remap + fSmoothness, 1.f - In.vTexUV.y);

				float2 distortionUV = randomNormal * Distortion + OffsetUV;
				float4 noise = g_tex_7.Sample(LinearSampler, distortionUV);

				flameDown = step(smoothDissolve, noise.a);
			}

			float4 flameUpColor = (flameUp - flameDown) * float4(1.f, 0.26f, 0.f, 1.f); // orange color
			float4 flameDownColor = flameDown * float4(1.f, 0.59f, 0.f, 1.f); // yellow color
			flame = saturate(flameUpColor + flameDownColor);
			if (flame.a <= 0.01)
				discard;
		}


	    float4 caustics = g_tex_6.Sample(LinearSampler, In.vTexUV);

	    float4 noise = g_tex_3.Sample(LinearSampler, In.vTexUV);
		float4 baseColor = float4(0.2f, 0.4f, 0.3f, 1.0f); // Example water base color
	    float causticsIntensity = 0.56f; // Example caustics intensity factor
	    float noiseIntensity = 0.65f; // Example noise intensity factor
	    float3 noiseOffset = float3(1.0f, 2.0f, 3.0f); // Example noise offset
		
		float3 noiseValue = g_tex_4.Sample(LinearSampler, TilingAndOffset(In.vTexUV * 20.f + noiseOffset, float2(1.f, 1.f), float2(g_Time, g_Time * 0.5f)));
		Out.vDiffuse = baseColor + (float4)(causticsIntensity * caustics.r) + (noiseIntensity * float4(noiseValue, 0.f)) * flame;
		Out.vDiffuse.a = 1.f;

		Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, fEmissive, flags);
		// Out.vFlag = flags;
		Out.vRMA = float4(0.3f, 0.5f, 1.f, 0.f);
	}

	Out.vFlag = 0.f;

	return Out;
}

// g_float_0  : 사망 디졸브
// g_int_0 : 상태이상 플래그
PS_OUT PS_MAIN_EM320_Glass0_3(PS_IN In)
{
	PS_OUT			Out = PS_MAIN_DEFAULT(In);

	float3 vNormal = Out.vNormal.xyz * 2.f - 1.f;
	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(vNormal, normalize(vViewDir.xyz), 2.f);
	float4 vWhite = float4(0.f, 0.f, 0.f, 0.f);
	// if (fFresnel > 0.9f)
	// 	discard;

	Out.vDiffuse = lerp(vWhite, Out.vDiffuse, fFresnel);

	return Out;
}


// g_float_0  : 사망 디졸브
// g_float_1 : 약점 공격 받으면 일시적으로 반짝이기용
// g_int_0 : 상태이상 플래그
PS_OUT PS_MAIN_EM320_Weak_4(PS_IN In)
{
	PS_OUT			Out = PS_MAIN_DEFAULT(In);

	float fDissolve = g_float_0;
	if (fDissolve <= 0.f)
	{
		float4 vWeakNoise = g_Weak_Noise.Sample(LinearSampler, In.vTexUV);
		float2 randomNormal = vWeakNoise.yy;
		float2 distortionUV = randomNormal * 0.1f + TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, g_Time * 0.005f));

		float4 vWeak = g_Weak01.Sample(LinearSampler, distortionUV * 5.f);
		if (vWeak.a > 0.1f)
		{
			float time = sin(g_Time * 1.5f);
			Out.vDiffuse.xyz = lerp(Out.vDiffuse.xyz, float3(1.f, 0.3f, 0.f), vWeak.a);
			Out.vDepth.z = vWeak.a * 1.5f * (time + 2.f);
			Out.vRMA.g = 0.f;
			Out.vRMA.r = 1.f;
		}

		float fHit = g_float_1;
		if (fHit > 0.f)
		{
			Out.vDepth.z += fHit * 5.f;
		}
	}
	Out.vFlag = float4(0.f, SHADER_POST_ENEMY, SHADER_MONSTER_WEAK, 0.f);

	return Out;
}

PS_OUT_NONLIGHT PS_MAIN_Invisible_5(PS_IN In)
{
	PS_OUT_NONLIGHT			Out = (PS_OUT_NONLIGHT)0.f;

	float4 vNormalPacked = NormalPacking(In);
	float3 vNormal = vNormalPacked.xyz * 2.f - 1.f;
	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(vNormal.xyz, vViewDir.xyz, 1.f);

	Out.vColor = fFresnel;
	Out.vFlag = float4(SHADER_DISTORTION_STATIC, 0.f, 0.f, fFresnel);

	return Out;
}

// g_float_0  : 사망 디졸브
// g_float_1 : 약점 맞아서 빛나는 디솔브
// g_int_0 : 상태이상 플래그
PS_OUT em800_Weak_6(PS_IN In)
{
	PS_OUT			Out = PS_MAIN_DEFAULT(In);

	float time = sin(g_Time * 1.5f);
	Out.vDiffuse.xyz *= float3(1.f, 1.f, 0.f);
	Out.vDepth.z = time + 2.f;

	float fHit = g_float_1;
	if (fHit > 0.f)
	{
		Out.vDepth.z = fHit + 3.f;
	}
	Out.vDepth.w = SHADER_NONE_SHADE;
	Out.vFlag = float4(0.f, SHADER_POST_ENEMY, SHADER_MONSTER_WEAK, 0.f);

	return Out;
}

PS_OUT PS_MAIN_KINETIC_7(PS_IN In)
{
	float fDissolveDisappear = g_float_1; // 0 ~ 1 로 사라지기
	if (fDissolveDisappear > 0.f)
	{
		float fNoise = g_scl_noise_004.Sample(LinearSampler, In.vTexUV * 2.f).r;
		if (fDissolveDisappear < fNoise)
			discard;
	}

	PS_OUT			Out = (PS_OUT)0;

	float fDissolve = g_float_0;
	float fEmissive = 0.f;
	float flags = SHADER_DEFAULT;


	Out.vDiffuse = g_tex_0.Sample(LinearSampler, In.vTexUV);
	if (Out.vDiffuse.a < 0.01f)
		discard;

	Out.vNormal = NormalPacking(In);
	if (g_tex_on_2)
		Out.vRMA = g_tex_2.Sample(LinearSampler, In.vTexUV);
	else
		Out.vRMA = float4(1.f, 0.f, 1.f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, fEmissive, flags);
	Out.vFlag = 0.f;

	int iOutlineOn = g_int_0;
	float fPsychic = g_float_0;

	if (iOutlineOn)
		Out.vOutline = (float4)1.f;
	else
		Out.vOutline = (float4)0.f;

	if (fPsychic > 0.f)
	{
		if (fPsychic >= 1.f)
		{
			// kinetic wave texture(T_ef_scl_noi_054)
			float fWave = g_tex_3.Sample(LinearSampler, TilingAndOffset(In.vTexUV, 1.f, float2(g_Time * 0.25f, 0.f))).r;
			Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, COL_PURPLE, 0.5f);
		}
		else
		{
			Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, COL_PURPLE, 0.5f);
			Out.vDepth.z = 1.f - fPsychic;
		}
	}

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

// g_float_0  : 사망 디졸브
// g_float_1 : 투명화 풀리는 디졸브(0~1)
// g_int_0 : 상태이상 플래그
PS_OUT PS_em0210_Invisible_9(PS_IN In)
{
	PS_OUT			Out = PS_MAIN_DEFAULT(In);

	float fInvisibleOutDissolve = g_float_1;
	if (fInvisibleOutDissolve > 0.f && fInvisibleOutDissolve < 1.f)
	{
		float fInvisibleOutDissolveSin = sin(fInvisibleOutDissolve * PI);
		float4 vWeak = g_Weak01.Sample(LinearSampler, In.vTexUV * 3.f);
		if (vWeak.a > 0.f)
		{
			Out.vDepth.z += fInvisibleOutDissolveSin * 3.f;
			Out.vDiffuse += vWeak * fInvisibleOutDissolveSin * 1.5f;
			Out.vDiffuse.a = 1.f;
		}
	}

	if (fInvisibleOutDissolve >= 1.f)
		Out.vFlag = float4(0.f, SHADER_POST_ENEMY, SHADER_DEFAULT, 0.f);
	else
		Out.vFlag = float4(0.f, SHADER_POST_ENEMY, SHADER_MONSTER_INVISIBLE, 0.f);
	
	return Out;
}

// g_float_0  : 사망 디졸브
// g_float_1 : 아머 반짝이는 디솔브
// g_float_2 : 아머 부서지는
// g_int_0 : 상태이상 플래그
PS_OUT PS_EnemyArmor_10(PS_IN In)
{
	PS_OUT			Out = PS_MAIN_DEFAULT(In);

	float fArmorDissolve = g_float_1;
	if (fArmorDissolve > 0.f)
	{
		float4 vViewDir = g_vCamPosition - In.vWorldPos;
		float3 vNormal = Out.vNormal.xyz * 2.f - 1.f;
		float fFresnel = FresnelEffect(vNormal, normalize(vViewDir.xyz), 1.f);
		Out.vDepth.z = fFresnel * 6.5f * fArmorDissolve;
	}
	float fBreakDissolve = g_float_2;
	if (fBreakDissolve > 0.f)
	{
		float fNoise = g_scl_noise_004.Sample(LinearSampler, In.vTexUV * 2.f).r;
		if (fBreakDissolve >= fNoise)
			discard;
	}

	return Out;
}

// g_float_0  : 사망 디졸브
// g_float_1 : 약점 맞아서 빛나는 디솔브
// g_int_0 : 상태이상 플래그

// g_vec4_0 : 약점색
// g_vec2_0 : x: 약점 최소 밝기, y 약점 히트 밝기 더하기 값
PS_OUT PS_em220_Weak_11(PS_IN In)
{
	PS_OUT			Out = PS_MAIN_DEFAULT(In);

	float time = abs(sin(g_Time * 1.5f));

	Out.vDiffuse.xyz *= g_vec4_0.rgb;
	Out.vDepth.z = time + g_vec2_0.x;

	float fHit = g_float_1;
	if (fHit > 0.f)
	{
		Out.vDepth.z += g_vec2_0.y * fHit;
	}
	Out.vDepth.w = SHADER_NONE_SHADE;
	Out.vFlag = float4(0.f, SHADER_POST_ENEMY, SHADER_MONSTER_WEAK, 0.f);

	return Out;
}

// g_float_0  : 사망 디졸브
// g_float_1 : 약점 맞아서 빛나는 디솔브
// g_int_0 : 상태이상 플래그
PS_OUT PS_em1100_Weak_12(PS_IN In)
{
	PS_OUT			Out = PS_MAIN_DEFAULT(In);

	float3 mask = g_tex_3.Sample(LinearSampler, In.vTexUV).rgb;
	if (mask.r == 1.f && mask.g == 1.f && mask.b == 1.f)
	{
		float time = abs(sin(g_Time * 1.5f));
	
		float4 vWeak = g_Weak01.Sample(LinearSampler, In.vTexUV * 20.f);
		Out.vDiffuse.xyz += vWeak.rgb;
		Out.vDepth.z = time + vWeak.a * 3.f;
	
		float fHit = g_float_1;
		if (fHit > 0.f)
		{
			Out.vDepth.z += g_vec2_0.y * fHit + 1.f;
		}
		Out.vDepth.w = SHADER_NONE_SHADE;
		Out.vFlag = float4(0.f, SHADER_POST_ENEMY, SHADER_MONSTER_WEAK, 0.f);	
	}

	return Out;
}

// g_float_0  : 사망 디졸브
// g_float_1 : 약점 맞아서 빛나는 디솔브
// g_int_0 : 상태이상 플래그
PS_OUT PS_em1200_Weak_13(PS_IN In)
{
	PS_OUT			Out = PS_MAIN_DEFAULT(In);

	float time = abs(sin(g_Time * 1.5f));

	Out.vDepth.z = time * 2.f;

	float fHit = g_float_1;
	if (fHit > 0.f)
	{
		Out.vDepth.z += g_vec2_0.y * fHit + 1.f;
	}
	Out.vDepth.w = SHADER_NONE_SHADE;
	Out.vFlag = float4(0.f, SHADER_POST_ENEMY, SHADER_MONSTER_WEAK, 0.f);	

	return Out;
}

technique11 DefaultTechnique
{
	//0
	pass Empty
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
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
	}

	//2
	pass em320_Water_2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EM320_Water_2();
	}

	//3
	pass em320_Glass0_3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EM320_Glass0_3();
	}

	//4
	pass em320_Weak_4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EM320_Weak_4();
	}

	//5
	pass Monster_Invisible
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_Invisible_5();
	}

	//6
	pass em800_Weak_6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 em800_Weak_6();
	}

	//7
	pass Kinetic_Object
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_KINETIC_7();
	}

	//8
	pass SuperSpeedTrail
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

	//9
	pass em0210_Invisible_9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_em0210_Invisible_9();
	}

	//10
	pass EnemyArmor_10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EnemyArmor_10();
	}
	//11
	pass em220_Weak_11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_em220_Weak_11();
	}

	//12
	pass em1100_Weak_12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_em1100_Weak_12();
	}

	//13
	pass em1200_Weak_13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_em1200_Weak_13();
	}
}
