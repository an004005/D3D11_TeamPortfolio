#include "Shader_Defines.h"
#include "Shader_Utils.h"
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
int g_bRotate = 0;
int g_bLocal = 0;
int g_bSizeDecreaseByLife = 0;
int g_bSizeIncreaseByLife = 0;
float g_fIncreaseMaxSize = 1.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
	row_major float4x4	Matrix : WORLD;

	float4		vRotRight : TEXCOORD0;
	float4		vRotUp : TEXCOORD1;
	float4		vRotLook : TEXCOORD2;
	float4		vControlData : TEXCOORD3;

};

struct VS_OUT
{
	float3		vPosition : POSITION;
	float2		vSize : TEXCOORD0;
	float		CurLife : TEXCOORD1;
	float		RamainLifeRatio : TEXCOORD2;

	float4x4    RotMatrix : TEXCOORD3;
};

struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vSize : TEXCOORD0;
	float		CurLife : TEXCOORD1;
	float		RamainLifeRatio : TEXCOORD2;
	float4x4    RotMatrix : TEXCOORD3;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		CurLife : TEXCOORD1;
	float		RamainLifeRatio : TEXCOORD2;
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		CurLife : TEXCOORD1;
	float		RamainLifeRatio : TEXCOORD2;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

struct PS_OUT_FLAG
{
	float4		vColor : SV_TARGET0;
	float4		vFlag  : SV_TARGET1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix InstanceData = In.Matrix;

	float4x4 RotationMatrix = float4x4(In.vRotRight, float4(matrix_look(InstanceData), 0.f), In.vRotLook, vector(0.f,0.f,0.f,1.f));


	Out.CurLife = InstanceData[3][3];
	Out.RamainLifeRatio = saturate(((InstanceData[1][3] - InstanceData[3][3] )/ InstanceData[1][3]));
	Out.vSize = float2(InstanceData[1][0], InstanceData[1][1]);
	Out.vPosition = matrix_postion(InstanceData);

	if (g_bSizeDecreaseByLife)
		Out.vSize *= Out.RamainLifeRatio;
	else if (g_bSizeIncreaseByLife)
		Out.vSize = lerp(Out.vSize, float2(g_fIncreaseMaxSize, g_fIncreaseMaxSize), 1.f - Out.RamainLifeRatio);

	if (g_bLocal == 1)
	{
		Out.vPosition = mul(float4(Out.vPosition, 1.f), g_WorldMatrix).xyz;
	}

	Out.RotMatrix = RotationMatrix;

	return Out;
}

[maxvertexcount(6)]
void GS_MAIN( point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	float2 PSize = In[0].vSize;
	float3		vLook;
	float3		vRight;
	float3		vUp;

	if (g_bRotate == 0)
	{
		vLook = g_vCamPosition.xyz - In[0].vPosition;
		vRight = (normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * PSize.x * 0.5f);
		vUp = (normalize(cross(vLook, vRight)) * PSize.y * 0.5f);
	}
	else if(g_bRotate == 1)
	{
		vLook = normalize(g_vCamPosition.xyz - In[0].vPosition);
		vRight = normalize(cross(normalize(In[0].RotMatrix[1].xyz), vLook)) * PSize.x * 0.5f;
		vUp = normalize(cross(vLook, vRight)) * PSize.y * 0.5f;
	}
	else if (g_bRotate == 2)
	{
		vLook = (In[0].RotMatrix[0].xyz - In[0].vPosition);
		vRight = (normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * PSize.x * 0.5f);
		vUp = (normalize(cross(vLook, vRight)) * PSize.y * 0.5f);
	}

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].CurLife = In[0].CurLife;
	Out[0].RamainLifeRatio = In[0].RamainLifeRatio;

	vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].CurLife = In[0].CurLife;
	Out[1].RamainLifeRatio = In[0].RamainLifeRatio;

	vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].CurLife = In[0].CurLife;
	Out[2].RamainLifeRatio = In[0].RamainLifeRatio;

	vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].CurLife = In[0].CurLife;
	Out[3].RamainLifeRatio = In[0].RamainLifeRatio;



	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float gradient = g_tex_0.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = gradient * g_vec4_0 * g_float_0; // color * emissive
	Out.vColor.a = gradient;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT_FLAG PS_DRIVEMODE_ONPARTICLE(PS_IN In)
{
	PS_OUT_FLAG			Out = (PS_OUT_FLAG)0;
	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV);
	float fDissolve = g_tex_2.Sample(LinearSampler, In.vTexUV).r;

	float4 Color = g_vec4_0;
	float4 Blend = Default * Color * 2.0f;
	float4 Final = saturate(Blend);
	Final.a = Mask.r * (fDissolve * In.RamainLifeRatio);
	Out.vColor = CalcHDRColor(Final, g_float_0 * In.RamainLifeRatio);
	Out.vColor.a *= In.RamainLifeRatio;

	Out.vFlag = float4(0.f, 0.f, 0.f, Out.vColor.a);


	// if (g_float_1 <= fDissolve)
		// discard;

	if (Out.vColor.a < 0.001f)
		discard;

	return Out;
}

PS_OUT PS_FLIPBOOK_EM320BULLET_TRAIL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 TEXUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05, 4, 4);

	float4 Default = g_tex_0.Sample(LinearSampler, TEXUV);

	float4 Color = g_vec4_0;

	float4 BlendColor = Default * Color * 2.0;


	float4 NormalTex = g_tex_1.Sample(LinearSampler, TEXUV);
	NormalTex.xyz = NormalTex.xyz * 2.f - 1.f;

	float4 vViewDir = g_vCamPosition - In.vPosition;
	float fFresnel = FresnelEffect(NormalTex.xyz, vViewDir.xyz, 3.f);

	float4 FinalColor = saturate(BlendColor * fFresnel);

	Out.vColor = CalcHDRColor(FinalColor, g_float_0); // color * emissive

	Out.vColor.a = Default.r * g_float_1;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_EM1100_WATER_PARTICLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 TEXUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05f, 8, 8);
	float4 Default = g_tex_0.Sample(LinearSampler, TEXUV);

	float4 Color = g_vec4_0;

	float4 BlendColor = Default * Color * 2.0;

	float4 FinalColor = saturate(BlendColor );
	FinalColor.a = Default.r;
	Out.vColor = CalcHDRColor(FinalColor, g_float_0); // color * emissive

	Out.vColor.a = Default.r* g_float_1;

	if (Out.vColor.a < 0.001f)
		discard;

	return Out;
}

PS_OUT PS_EM1100_WATER_Dead(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 TEXUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05f, 4, 4);
	float4 Default = g_tex_0.Sample(LinearSampler, TEXUV);

	float4 Color = g_vec4_0;

	float4 BlendColor = Default * Color * 2.0;


	float4 NormalTex = g_tex_1.Sample(LinearSampler, TEXUV);
	NormalTex.xyz = NormalTex.xyz * 2.f - 1.f;

	float4 vViewDir = g_vCamPosition - In.vPosition;
	float fFresnel = FresnelEffect(NormalTex.xyz, vViewDir.xyz, 0.1f);

	float4 FinalColor = saturate(BlendColor * fFresnel);
	FinalColor.a = Default.r;
	Out.vColor = CalcHDRColor(FinalColor, g_float_0); // color * emissive

	Out.vColor.a = Out.vColor.a *  In.RamainLifeRatio ;

	if (Out.vColor.a < 0.001f)
		discard;

	return Out;
}

PS_OUT PS_PARTICLE_DEFAULT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	// float GradientTex = g_tex_1.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = DefaultTex * g_vec4_0 * g_float_0; // color * emissive

	if(g_tex_on_1)
	{
		float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
		Out.vColor.a = Mask * In.RamainLifeRatio;
	}
	else
	{
		// Out.vColor.a = GradientTex;
		Out.vColor = DefaultTex;

		if (Out.vColor.a < 0.01f)
			discard;
	}


	return Out;
}

PS_OUT PS_PARTICLE_EM0650(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	// float GradientTex = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	float4 Choose = g_vec4_0;
	float4 BlendColor = DefaultTex * Choose * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(FinalColor,g_float_0 * In.RamainLifeRatio); // color * emissive

	if (g_tex_on_1)
	{
		float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
		Out.vColor.a = Mask;
		if (Mask <= 0.f)
			discard;
	}
	else
	{
		// Out.vColor.a = GradientTex;
		Out.vColor = DefaultTex;

		if (Out.vColor.a < 0.01f)
			discard;
	}

	if (Out.vColor.a <= 0.1f)
		discard; 

	return Out;
}

PS_OUT PS_EM1100_RAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	// float GradientTex = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	float4 Choose = g_vec4_0;
	float4 BlendColor = DefaultTex * Choose * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(FinalColor, g_float_0 * In.RamainLifeRatio); // color * emissive

	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor.a = Mask * In.RamainLifeRatio;

	if (Mask <= 0.f)
		discard;

	if (Out.vColor.a <= 0.1f)
		discard;
	
	return Out;
}

PS_OUT PS_TRAIN_HDR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 TEXUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.1, 4, 2);
	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	// float GradientTex = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	float4 Choose = g_vec4_0;
	float4 BlendColor = DefaultTex * Choose * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(FinalColor, g_float_0 * In.RamainLifeRatio); // color * emissive

	if (g_tex_on_1)
	{
		float Mask = g_tex_1.Sample(LinearSampler, TEXUV).r;
		Out.vColor.a = Mask;
		if (Mask <= 0.f)
			discard;
	}
	else
	{
		// Out.vColor.a = GradientTex;
		Out.vColor = DefaultTex;

		if (Out.vColor.a < 0.01f)
			discard;
	}

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_PARTICLE_KINETICOBJECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 DefaultTex = g_tex_0.Sample(LinearSampler, In.vTexUV);
	// float GradientTex = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
	float4 Choose = g_vec4_0;
	float4 BlendColor = DefaultTex * Choose * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(FinalColor, g_float_0); // color * emissive

	if (g_tex_on_1)
	{
		float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;
		Out.vColor.a = Mask * In.RamainLifeRatio;
		if (Mask <= 0.f)
			discard;
	}
	else
	{
		// Out.vColor.a = GradientTex;
		Out.vColor = DefaultTex;

		if (Out.vColor.a < 0.01f)
			discard;
	}

	if (Out.vColor.a <= 0.f)
		discard;

	return Out;
}

PS_OUT PS_FLIPBOOK_SMOKE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0, 1, 1));
	float4 vColor = g_vec4_0;

	float4 BlendColor = flipBook * vColor * 2.0f;

	float4 FinalColor = saturate(BlendColor);

	Out.vColor = FinalColor;
	Out.vColor.a = flipBook.a * In.RamainLifeRatio;

	return Out;
}

PS_OUT PS_FLIPBOOK_SMOKE_8x8(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05, 8, 8));
	float4 vColor = g_vec4_0;

	float4 BlendColor = flipBook * vColor * 2.0f;

	float4 FinalColor = saturate(BlendColor);

	Out.vColor = FinalColor;
	Out.vColor.a = flipBook.a * In.RamainLifeRatio * g_float_0;

	return Out;
}

PS_OUT PS_FLIPBOOK_SMOKE_4x4(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05, 4, 4));
	float4 vColor = g_vec4_0;

	float4 BlendColor = flipBook * vColor * 2.0f;

	float4 FinalColor = saturate(BlendColor);

	Out.vColor = FinalColor;
	Out.vColor.a = flipBook.a * In.RamainLifeRatio * g_float_0;

	return Out;
}

PS_OUT PS_EM110_BUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.03f, 1, 2));
	float4 vColor = g_vec4_0;

	if (flipBook.r <= 0.f)
		discard;

	float4 BlendColor = flipBook * vColor * 2.0f;

	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(FinalColor, g_float_0) ;
	Out.vColor.a = flipBook.a * In.RamainLifeRatio;

	if (Out.vColor.a <= 0.001f)
		discard;

	return Out;
}


PS_OUT PS_SAS_FIRE_PARTICLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.03, 4, 4));
	Out.vColor = CalcHDRColor(flipBook, g_float_0) ;

	float4 flipAlpha = g_tex_1.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.03, 4, 4));

	Out.vColor.a = flipAlpha.r * In.RamainLifeRatio;

	if (flipAlpha.r <= 0.2f)
		discard;

	return Out;
}

PS_OUT PS_SPECIAL_TRUCK_EXPLODE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.02, 8, 8));
	Out.vColor = CalcHDRColor(flipBook, g_float_0);
	// Out.vColor.a *= In.RamainLifeRatio;
	// float4 flipAlpha = g_tex_1.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.03, 8, 8));

	// Out.vColor.a = flipAlpha.r * In.RamainLifeRatio;

	// if (Out.vColor.a <= 0.01f)
	// 	discard;

	return Out;
}


PS_OUT PS_EM1100_Dust(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.03f, 1, 2));
	float4 vColor = g_vec4_0;

	if (flipBook.r <= 0.f)
		discard;

	float4 BlendColor = flipBook * vColor * 2.0f;

	float4 FinalColor = saturate(BlendColor);
	Out.vColor = CalcHDRColor(vColor, g_float_0);
	Out.vColor.a = flipBook.a * In.RamainLifeRatio;

	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}

PS_OUT PS_EM1100_ELEC_BULLET_HDRDIRST(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 TEXUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05, 4, 4);
	float4 flipBook = g_tex_0.Sample(LinearSampler, TEXUV);
	float4 Color = g_vec4_0;
	float4 Blend = flipBook * Color * 2.0f;
	float4 Final = saturate(Blend);
	float Mask = g_tex_1.Sample(LinearSampler, TEXUV).r;

	Out.vColor = CalcHDRColor(Final, g_float_0);

	Out.vColor.a = Mask * In.RamainLifeRatio;

	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}

PS_OUT PS_KINETIC_PARTICLE_CURVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 TEXUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05, 8, 8);
	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = Default * Color * 2.0f;
	float4 Final = saturate(Blend);
	float Mask = g_tex_1.Sample(LinearSampler, TEXUV).r;

	Out.vColor = CalcHDRColor(Final, g_float_0);

	Out.vColor.a = saturate(Mask * In.RamainLifeRatio * g_float_2);

	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}

PS_OUT PS_KINETIC_TRUCK_OIL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 TEXUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.1, 4, 4);
	float4 Default = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = Default * Color * 2.0f;
	float4 Final = saturate(Blend);
	float Mask = g_tex_1.Sample(LinearSampler, TEXUV).r;

	Out.vColor = CalcHDRColor(Final, g_float_0);

	Out.vColor.a = saturate(Mask * In.RamainLifeRatio * g_float_1);

	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}

PS_OUT PS_EM1100_ELEC_BULLET_EXPLODE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	float2 TEXUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.02, 8, 8);
	float4 flipBook = g_tex_0.Sample(LinearSampler, TEXUV);
	float4 Color = g_vec4_0;
	float4 Blend = flipBook * Color * 2.0f;
	float4 Final = saturate(Blend);
	float Mask = g_tex_1.Sample(LinearSampler, TEXUV).r;

	Out.vColor = CalcHDRColor(Final, g_float_0);

	Out.vColor.a = Mask * In.RamainLifeRatio;

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT PS_SAS_FIRE_WEAPON_PARTICLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05, 3, 3));

	Out.vColor = CalcHDRColor(flipBook, g_float_0);

	float4 flipAlpha = g_tex_1.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05, 3, 3));

	Out.vColor.a = flipAlpha.r* In.RamainLifeRatio;

	if (flipAlpha.r <= 0.2f)
		discard;

	return Out;
}

PS_OUT PS_SAS_DEFAULT_WEAPON_PARTICLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = White * Color * 2.0f;
	float4 Final = saturate(Blend) ;
	
	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = CalcHDRColor(Final, g_float_0);
	Out.vColor.a = Mask * g_float_1 * In.RamainLifeRatio;

	if(g_float_1 <= 0.f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_USE_MESHCURVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 White = g_tex_0.Sample(LinearSampler, In.vTexUV);
	float4 Color = g_vec4_0;
	float4 Blend = White * Color * 2.0f;
	float4 Final = saturate(Blend);

	float Mask = g_tex_1.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = CalcHDRColor(Final, g_float_0);
	Out.vColor.a = Mask * g_float_1 * In.RamainLifeRatio;

	if (g_float_1 <= 0.f || Out.vColor.a <= 0.2f)
	{
		discard;
	}

	return Out;
}


PS_OUT PS_SPIKE_CHARGE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float gradient = g_tex_0.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = (float4)1.f * gradient * 3.f;
	Out.vColor.a = gradient;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_PHOENIX_ULT_RETURN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, g_float_0, 4, 4));
	float4 vColor = float4(1.f, 100.f / 255.f, 0.1f, 1.f);
	
	Out.vColor = (flipBook.r) * vColor * 2.5f;
	Out.vColor.a = flipBook.a;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_PHOENIX_FIREBALL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 flipBook = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.CurLife, g_float_0, 2, 2));
	float4 vColor = float4(1.f, 100.f / 255.f, 0.1f, 1.f);

	float4 glow = g_tex_1.Sample(LinearSampler, In.vTexUV) * In.RamainLifeRatio;
	
	Out.vColor = (flipBook.a) * vColor * 2.f + glow;
	Out.vColor.a = flipBook.a * In.RamainLifeRatio;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

// g_tex_0 : 그라디언트와 동시에 디퓨즈
// g_tex_1 : 문자형상을 가진 텍스쳐
// g_float_0 : Emissive
// g_float_1 : Alpha

PS_OUT PS_MAIN_PLAYER_TEXT_PARTICLE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 MixUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 1 , 4, 4);

	float4 MixColor = g_tex_0.Sample(LinearSampler, MixUV);
	float4 Color = g_vec4_0;

	float4 BlendColor = MixColor * Color * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(FinalColor, saturate(g_float_0 *In.RamainLifeRatio));

	Out.vColor.a = saturate(MixColor.r * In.RamainLifeRatio);

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_SAS_LINE_TRAIL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 MixUV = Get_FlipBookUV(In.vTexUV, In.CurLife, 0.05, 4, 4);

	float4 MixColor = g_tex_0.Sample(LinearSampler, MixUV);
	float4 Color = g_vec4_0;

	float4 BlendColor = MixColor * Color * 2.0f;
	float4 FinalColor = saturate(BlendColor);

	Out.vColor = CalcHDRColor(FinalColor, g_float_0);

	Out.vColor.a = saturate(MixColor.r * In.RamainLifeRatio);

	return Out;
}

technique11 DefaultTechnique
{
	//0
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//1
	pass SpikeCharge
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPIKE_CHARGE();
	}

	//2
	pass PhoenixUltReturn
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PHOENIX_ULT_RETURN();
	}

	//3
	pass PhoenixFireBall
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PHOENIX_FIREBALL();
	}

	//4
	pass Player_Text_Particle
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PLAYER_TEXT_PARTICLE();
	}

	//5
	pass Flow_Particle_Mask_Tex
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PARTICLE_DEFAULT();
	}

	//6
	pass BulletTrailParticle
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PARTICLE_EM0650();
	}

	//7
	pass EMTUTORIALBOSS_BULLET_TRAIL
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLIPBOOK_EM320BULLET_TRAIL();
	}

	//8
	pass FlipSmoke
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLIPBOOK_SMOKE();
	}

	//9
	pass SasFireParticle
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_FIRE_PARTICLE();
	}

	//10
	pass SasFireWeaponParticle
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_FIRE_WEAPON_PARTICLE();
	}

	//11
	pass KineticObjectTrail
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PARTICLE_KINETICOBJECT();
	}

	//12
	pass SasDefaultWeaponParticle
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_DEFAULT_WEAPON_PARTICLE();
	}

	//13
	pass USEMESHCURVE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_USE_MESHCURVE();
	}

	//14
	pass Em110Bug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM110_BUG();
	}

	//15
	pass Em1100HdrDust
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_ELEC_BULLET_HDRDIRST();
	}

	//16
	pass Em1100Dust
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_Dust();
	}

	//17
	pass Em1100Explode
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_ELEC_BULLET_EXPLODE();
	}

	//18
	pass SAS_TRAIL_PARTICLE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SAS_LINE_TRAIL();
	}

	//19
	pass KineticParticleCurve
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_KINETIC_PARTICLE_CURVE();
	}

	//20
	pass KineticTruckOil
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_KINETIC_TRUCK_OIL();
	}

	//21
	pass FlipSmoke8x8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLIPBOOK_SMOKE_8x8();
	}

	//22
	pass TrainHdr
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TRAIN_HDR();
	}


	//23
	pass SpecialTruckExplode
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPECIAL_TRUCK_EXPLODE();
	}

	//24
	pass EM1100WaterParticle
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_WATER_PARTICLE();
	}

	//25
	pass Em1100Water_RainParticle
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_RAIN();
	}

	//26
	pass EM1100WaterDead
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM1100_WATER_Dead();
	}

	//27
	pass FlipSmoke4x4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLIPBOOK_SMOKE_4x4();
	}

	//28
	pass Sas_DriveMode_OnParticle
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DRIVEMODE_ONPARTICLE();
	}
}
