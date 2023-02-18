#include "Shader_Utils.h"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv, g_ViewMatrixInv;

matrix			g_LightViewMatrix;
matrix			g_LightProjMatrix;

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;

vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

vector			g_vCamPosition;

vector			g_vMtrlAmbient = (vector)1.f;
vector			g_vMtrlSpecular = (vector)0.f;

texture2D		g_Texture; /* 디버그용텍스쳐*/
texture2D		g_NormalTexture; 
texture2D		g_DepthTexture;
texture2D		g_ShadowDepthTexture;

texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

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
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	
	return Out;
}


struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;	
	float4		vSpecular : SV_TARGET1;
};


PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * 300.f;

	/* 0 ~ 1 => -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	// Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal))));
	Out.vShade = Out.vShade * 0.5f + 0.5f;
	Out.vShade.a = 1.f;

	/* 화면에 그려지고 있는 픽셀들의 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / z */
	// vector		vWorldPos;
	// vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	// vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	// vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	// vWorldPos.w = 1.0f;
	//
	// /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	// vWorldPos *= fViewZ;
	//
	// /* 로컬위치 * 월드행렬 * 뷰행렬 */
	// vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	//
	// /* 로컬위치 * 월드행렬  */
	// vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
	//
	// vector		vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	// vector		vLook = vWorldPos - g_vCamPosition;
	//
	// Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f);
	Out.vSpecular.a = 0.f;
	// specular 안씀
	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * 300.f;

	/* 0 ~ 1 => -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* 화면에 그려지고 있는 픽셀들의 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / z */
	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬  */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;

	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

	// Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal)))) * fAtt;
	Out.vShade = Out.vShade * 0.5f + 0.5f;
	Out.vShade.a = 1.f;	

	vector		vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f) * fAtt;

	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	// vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepth = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vColor = CalcHDRColor(vDiffuse, vDepth.b) * vShade /*+ vSpecular*/;
	if (0.0f == Out.vColor.a)
		discard;

	// 그림자 연산
	float fViewZ = vDepth.y * 300.f;

	vector vPixelWorld;
	vPixelWorld.x = In.vTexUV.x * 2.f - 1.f;
	vPixelWorld.y = In.vTexUV.y * -2.f + 1.f;
	vPixelWorld.z = vDepth.x; /* 0 ~ 1 */
	vPixelWorld.w = 1.0f;
	vPixelWorld *= fViewZ; // pixcel clip space

	vPixelWorld = mul(vPixelWorld, g_ProjMatrixInv); // pixel view space
	vPixelWorld = mul(vPixelWorld, g_ViewMatrixInv); // pixel world space

	vector		vLightViewSpace = mul(vPixelWorld, g_LightViewMatrix);;
	vector		vLightClipSpace = mul(vLightViewSpace, g_LightProjMatrix);
	float2		vNewUV;
	vNewUV.x = (vLightClipSpace.x / vLightClipSpace.w) * 0.5f + 0.5f;
	vNewUV.y = (vLightClipSpace.y / vLightClipSpace.w) * -0.5f + 0.5f;

	// float fBias = 0.1f;
	float2 TexcelSize = float2( 1.0 / 8192.0,  1.0 / 8192.0);
	float fShadowRate = 0.f;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x, y) * TexcelSize;
			vector		vShadowDepthInfo = g_ShadowDepthTexture.Sample(LinearSampler, vNewUV + offset);
			if (vLightViewSpace.z - 0.1f > vShadowDepthInfo.x * 300.0f)
				fShadowRate += 1.f;
		}
	}

	fShadowRate /= 9.f;
	fShadowRate *= 0.3f;

	Out.vColor *= (1.f - fShadowRate);

	return Out;
}


RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};


RasterizerState RS_CW
{
	CullMode = Front;
	FrontCounterClockwise = false;
};

DepthStencilState DS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DS_ZEnable_ZWriteEnable_FALSE
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = Add;
};

BlendState BS_One
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = Add;
};


technique11 DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

}
