#include "Shader_Defines.h"
#include "Shader_Params.h"
#include "Shader_Utils.h"

Texture2D g_DepthTex;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

// g_float_0 : dissolve
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

    float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthDesc = g_DepthTex.Sample(LinearSampler, vTexUV);
	if (vDepthDesc.w != 1.f) // map object�� �ƴϸ� decal���� ���� �ʴ´�.
		discard;

	float fViewZ = vDepthDesc.y * g_Far;
	vector vProjPos = (vector)0.f;

    // ��������� ����� ����.
	vProjPos.x = (vTexUV.x *  2.f - 1.f) * fViewZ;
	vProjPos.y = (vTexUV.y * -2.f + 1.f) * fViewZ;
	vProjPos.z = (vDepthDesc.x) * fViewZ;
	vProjPos.w = fViewZ;

	vector vViewPos = mul(vProjPos, g_ProjMatrixInv);
	vector vWorldPos = mul(vViewPos, g_ViewMatrixInv);
	vector vLocalPos = mul(vWorldPos, g_WorldMatrixInv);

	float3	ObjectAbsPos = abs(vLocalPos.xyz);
	clip(0.5f - ObjectAbsPos);

	float2 decalUV = vLocalPos.xz + 0.5f;

	

	Out.vDiffuse = g_tex_0.Sample(LinearSampler, decalUV);
	Out.vDiffuse.a = g_tex_2.Sample(LinearSampler, decalUV).r;

	if (Out.vDiffuse.a < 0.01f)
		discard;

	float4 Normal = g_tex_1.Sample(LinearSampler, decalUV) * 2.f - 1.f;

	Out.vFlag = float4(0.f, 0.f, 0.f, 0.f);


	return Out;
}

PS_OUT PS_HIT_DECAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthDesc = g_DepthTex.Sample(LinearSampler, vTexUV);
	if (vDepthDesc.w != 1.f) // map object�� �ƴϸ� decal���� ���� �ʴ´�.
		discard;

	float fViewZ = vDepthDesc.y * g_Far;
	vector vProjPos = (vector)0.f;

	// ��������� ����� ����.
	vProjPos.x = (vTexUV.x *  2.f - 1.f) * fViewZ;
	vProjPos.y = (vTexUV.y * -2.f + 1.f) * fViewZ;
	vProjPos.z = (vDepthDesc.x) * fViewZ;
	vProjPos.w = fViewZ;

	vector vViewPos = mul(vProjPos, g_ProjMatrixInv);
	vector vWorldPos = mul(vViewPos, g_ViewMatrixInv);
	vector vLocalPos = mul(vWorldPos, g_WorldMatrixInv);

	float3	ObjectAbsPos = abs(vLocalPos.xyz);
	clip(0.5f - ObjectAbsPos);

	float2 decalUV = vLocalPos.xz + 0.5f;

	float4 Default = g_tex_0.Sample(LinearSampler, decalUV);


	float Mask = g_tex_1.Sample(LinearSampler, decalUV).r;
	float4 Line = g_tex_2.Sample(LinearSampler, decalUV);



	Out.vDiffuse.a = Mask;



	if (Line.a != 0.f)
	{
		Line = Line * g_vec4_0;
		Out.vDiffuse = CalcHDRColor(Line, g_float_0);
		float4 Depth = g_DepthTex.Sample(LinearSampler, vTexUV);
		Out.vDiffuse.a = Line.r;
	}
	else
	{
		float4 Color = float4(g_float_1,g_float_2, g_float_3, 1.f);
		Out.vDiffuse = CalcHDRColor(Default * Color, g_float_4);
		Out.vDiffuse.a = Mask * g_float_5;
	}

	Out.vDiffuse.a *= g_float_6;
	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vDiffuse.a * g_float_7 * g_float_6);
	// Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, 1.f);


	return Out;
}

PS_OUT PS_EM650_DECAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthDesc = g_DepthTex.Sample(LinearSampler, vTexUV);
	if (vDepthDesc.w != 1.f) // map object�� �ƴϸ� decal���� ���� �ʴ´�.
		discard;

	float fViewZ = vDepthDesc.y * g_Far;
	vector vProjPos = (vector)0.f;

	// ��������� ����� ����.
	vProjPos.x = (vTexUV.x *  2.f - 1.f) * fViewZ;
	vProjPos.y = (vTexUV.y * -2.f + 1.f) * fViewZ;
	vProjPos.z = (vDepthDesc.x) * fViewZ;
	vProjPos.w = fViewZ;

	vector vViewPos = mul(vProjPos, g_ProjMatrixInv);
	vector vWorldPos = mul(vViewPos, g_ViewMatrixInv);
	vector vLocalPos = mul(vWorldPos, g_WorldMatrixInv);

	float3	ObjectAbsPos = abs(vLocalPos.xyz);
	clip(0.5f - ObjectAbsPos);

	float2 decalUV = vLocalPos.xz + 0.5f;
	float2 TexUV;

	if (g_int_0 > 0)
	{
		TexUV = Get_FlipBookUV(decalUV, g_Time, 0.05, 4, 4);
	}
	else
		TexUV = Get_FlipBookUV(decalUV, 0.f, 0.05, 4, 4);

	float4 Default = g_tex_0.Sample(LinearSampler, TexUV);
	float Mask = Default.r;

	// float Mask = g_tex_1.Sample(LinearSampler, decalUV);

	float4 Color = g_vec4_0;
	float4 Blend = Default * Color * 2.0f;
	float4 Final = saturate(Blend);
	Out.vDiffuse = CalcHDRColor(Final, g_float_0);
	Out.vDiffuse.a = Mask * g_float_1;

	if (g_float_1 <= 0.f)
		discard;

	Out.vFlag = float4(SHADER_DISTORTION, 0.f, 0.f, Out.vDiffuse.a * g_float_2);


	


	return Out;
}


technique11 DefaultTechnique
{
	// 0
	pass DefaultDecal
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1
	pass HittDecal
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HIT_DECAL();
	}
	// 2
	pass Em0650Decal
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EM650_DECAL();
	}

}
