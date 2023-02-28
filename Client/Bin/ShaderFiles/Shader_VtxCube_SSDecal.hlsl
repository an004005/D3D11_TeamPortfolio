#include "Shader_Defines.h"
#include "Shader_Params.h"
#include "Shader_Utils.h"

Texture2D g_DiffuseTex;
Texture2D g_NormalTex;
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
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

// g_float_0 : dissolve
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

    float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthDesc = g_DepthTex.Sample(LinearSampler, vTexUV);
	if (vDepthDesc.w != 4.f) // map object가 아니면 decal적용 하지 않는다.
		discard;

	float fViewZ = vDepthDesc.y * g_Far;
	vector vProjPos = (vector)0.f;

    // 투영행렬을 만드는 과정.
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
	Out.vDiffuse.a *= (1.f - saturate(g_float_0));
	if (Out.vDiffuse.a < 0.01f)
		discard;

	Out.vNormal = g_NormalTex.Sample(LinearSampler, vTexUV);
	Out.vDepth = g_DepthTex.Sample(LinearSampler, vTexUV);
	// emissive
	Out.vDepth.z = 0.5f * Out.vDiffuse.a;

	return Out;
}

PS_OUT PS_SPIKE_CRACK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

    float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthDesc = g_DepthTex.Sample(LinearSampler, vTexUV);
	if (vDepthDesc.w != 4.f) // map object가 아니면 decal적용 하지 않는다.
		discard;

	float fViewZ = vDepthDesc.y * g_Far;
	vector vProjPos = (vector)0.f;

    // 투영행렬을 만드는 과정.
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


	// 1 to 0
	float fDissolve = g_float_0;
	float2 centerUV = float2(0.5f, 0.5f);
	
	if (length(decalUV - centerUV) > (1.f - fDissolve) * 0.5)
		discard;

	float2 toCenter = normalize(centerUV - decalUV) * fDissolve * 0.2f;

	float2 offsetUV = TilingAndOffset(decalUV, float2(2.f, 2.f), toCenter);
	float crackNoise = g_tex_1.Sample(LinearSampler, offsetUV).r;

	if (crackNoise <= fDissolve)
		discard;

	Out.vDiffuse = (float4)0.f;
	Out.vDiffuse.a = 1.f;
	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vNormal = (float4)0;
	Out.vDepth = g_DepthTex.Sample(LinearSampler, vTexUV);

	return Out;
}

PS_OUT PS_FIREWALL_BOT(PS_IN In)
{
	// SSD 기본 
	PS_OUT			Out = (PS_OUT)0;

    float2 vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthDesc = g_DepthTex.Sample(LinearSampler, vTexUV);
	if (vDepthDesc.w != 4.f) // map object가 아니면 decal적용 하지 않는다.
		discard;

	float fViewZ = vDepthDesc.y * g_Far;
	vector vProjPos = (vector)0.f;

    // 투영행렬을 만드는 과정.
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

	Out.vDepth = g_DepthTex.Sample(LinearSampler, vTexUV);
	// ~SSD 기본 
	float4 vNormalDesc = g_NormalTex.Sample(LinearSampler, vTexUV);
	vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	clip(dot(vNormal.xyz, float3(0.f, 1.f, 0.f)) - radians(30.f));


	Out.vNormal = float4(0.f, 1.f, 0.f, 0.f);

	float time = g_Time * 0.2f;
	float fDissolve = g_float_0;

	float2 OffsetUV = TilingAndOffset(decalUV, float2(1.f, 1.f), float2(time, time));
	float4 flow = g_tex_2.Sample(LinearSampler, decalUV);
	float2 distortionUV = flow.xy * (float2)0.3 + OffsetUV;

	float4 color =  float4(1.f, 68.f / 255.f, 51.f/255.f, 1.f); // orange red
	float4 mask = g_tex_0.Sample(LinearSampler, distortionUV);
	float4 glow = g_tex_1.Sample(LinearSampler, decalUV);

	if (glow.r <= fDissolve)
		discard;

	Out.vDiffuse = (mask.r + glow.r) * color * 2.f;
	Out.vDiffuse.a = glow.r;
	Out.vDepth.z = 1.f;

	if (Out.vDiffuse.a < 0.03f)
		discard;

	Out.vDiffuse.a = saturate(Out.vDiffuse.a + 0.2f);

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
	pass SpikeCrack
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPIKE_CRACK();
	}

	// 2
	pass FireWallBot
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FIREWALL_BOT();
	}
}
