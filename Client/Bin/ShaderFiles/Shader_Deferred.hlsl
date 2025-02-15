#include "Shader_Utils.h"

float   g_Far = 1000.f;

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv, g_ViewMatrixInv;

matrix			g_LightViewMatrix;
matrix			g_LightProjMatrix;

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;
vector			g_vCapsuleStart;
vector			g_vCapsuleEnd;

vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

vector			g_vCamPosition;

vector			g_vMtrlAmbient = (vector)1.f;
vector			g_vMtrlSpecular = (vector)0.5f;

texture2D		g_Texture; /* 디버그용텍스처*/
texture2D		g_NormalTexture; 
texture2D		g_DepthTexture;
texture2D		g_ShadowDepthTexture;

texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;
texture2D		g_RMATexture;
texture2D		g_AMBTexture;
texture2D		g_CTLTexture;
texture2D		g_OutlineFlagTexture;

TextureCube     g_IrradianceTexture;
TextureCube     g_RadianceTexture;

Texture2D<float> g_AOTexture;

float3			g_vFogColor;
float3			g_vHighlightColor;
float3			g_vDirToSun;
float			g_fStartDepth;
float			g_fGlobalDensity;
float			g_fHeightFalloff;
int				g_bFog;


float g_Gamma = 2.2f;

float g_iWinCX;
float g_iWinCY;

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

sampler CubeSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	ComparisonFunc = NEVER;
};

#include "Shader_PBRCommon.h"

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

float GetSpecular(float3 normal, float3 eyeVec, float3 lightDir, float specularPower)
{
    float3 halfVec = normalize(normalize(eyeVec) + lightDir);
    return pow(max(dot(normal, halfVec), 0.0), specularPower);
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * g_Far;

	/* 0 ~ 1 => -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;
 
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	float fShaderFlag = g_DepthTexture.Sample(PointSampler, In.vTexUV).w;

	if (fShaderFlag == SHADER_DEFAULT)
	{
		vector		vRMA = g_RMATexture.Sample(LinearSampler, In.vTexUV);

		float3 albedo = pow(abs(vDiffuse.rgb), g_Gamma);
		// float3 albedo = vDiffuse.rgb;
		float metalness = vRMA.g;
		float roughness = vRMA.r;
		float AO = vRMA.b;

	    const float3 V = normalize(g_vCamPosition.xyz - vWorldPos.xyz); // view vector

		float fSSAO = g_AOTexture.Sample(LinearSampler, In.vTexUV);

		Out.vShade.rgb = fSSAO * LightSurface(V, normalize(vNormal.xyz), g_vLightDiffuse.xyz, g_vLightDir.xyz, albedo.rgb, roughness, metalness, AO);
		Out.vShade.a = 1.f;

		return Out;

	}
	else if (fShaderFlag == SHADER_TOON)
	{
		float4 vCTL = g_CTLTexture.Sample(LinearSampler, In.vTexUV);

		float fNdotL = dot(normalize(vNormal.xyz), normalize(g_vLightDir.xyz));
		float fDiff = saturate(max(fNdotL, 0.0));
	
		fDiff = max(vCTL.r * 2.f , min(vCTL.g, fDiff));
		fDiff *= vCTL.b;

		// float fSSAO = g_AOTexture.Sample(LinearSampler, In.vTexUV);

		Out.vShade = g_vLightDiffuse * saturate(fDiff);
		Out.vShade.a = 1.f;
		
		// vector		vLook = vWorldPos - g_vCamPosition;
		// float spec = GetSpecular(vNormal.xyz, vLook.xyz, g_vLightDir.xyz, 4.f);
		// spec = smoothstep(0.005f, 0.01f, spec);
		// Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * spec;
	}

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float fShaderFlag = g_DepthTexture.Sample(PointSampler, In.vTexUV).w;
	if (fShaderFlag == SHADER_TOON) discard;

	float		fViewZ = vDepthDesc.y * g_Far;

	/* 0 ~ 1 => -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;
 
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);
	float		fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;


	if (fShaderFlag == SHADER_DEFAULT)
	{
		Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal)))) * fAtt;
		Out.vShade.a = 1.f;

		vector		vReflect = reflect(normalize(vLightDir), normalize(vNormal));
		vector		vLook = vWorldPos - g_vCamPosition;

		Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f) * fAtt;

		Out.vSpecular.a = 0.f;
	}

	return Out;
}


float3 ApplyFog(float3 originalColor, float eyePosY, float3 eyeToPixel)
{
	float pixelDist = length( eyeToPixel );
	float3 eyeToPixelNorm = eyeToPixel / pixelDist;

	// Find the fog staring distance to pixel distance
	float fogDist = max(pixelDist - g_fStartDepth, 0.0);

	// Distance based fog intensity
	float fogHeightDensityAtViewer = exp( -g_fHeightFalloff * eyePosY );
	float fogDistInt = fogDist * fogHeightDensityAtViewer;

	// Height based fog intensity
	float eyeToPixelY = eyeToPixel.y * ( fogDist / pixelDist );
	float t = g_fHeightFalloff * eyeToPixelY;
	const float thresholdT = 0.01;
	float fogHeightInt = abs( t ) > thresholdT ?
		( 1.0 - exp( -t ) ) / t : 1.0;

	// Combine both factors to get the final factor
	float fogFinalFactor = exp( -g_fGlobalDensity * fogDistInt * fogHeightInt );

	// Find the sun highlight and use it to blend the fog color
	float sunHighlightFactor = saturate(dot(eyeToPixelNorm, normalize(g_vDirToSun)));
	sunHighlightFactor = pow(sunHighlightFactor, 8.0);
	float3 fogFinalColor = lerp(g_vFogColor, g_vHighlightColor, sunHighlightFactor);

	return lerp(fogFinalColor, originalColor, fogFinalFactor);
}


PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDepth = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	// vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	// vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	float fShaderFlag = g_DepthTexture.Sample(PointSampler, In.vTexUV).w;

	if (fShaderFlag == SHADER_TOON)
	{
		float4 vAMB = g_AMBTexture.Sample(LinearSampler, In.vTexUV);
		vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
		vDiffuse.rgb = pow(abs(vDiffuse.rgb), g_Gamma);
		vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
		vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

		Out.vColor = CalcHDRColor(vDiffuse, vDepth.b) * vShade + vAMB * 0.5f;
		if (0.0f == Out.vColor.a)
			discard;
	}
	else if (fShaderFlag == SHADER_DEFAULT)
	{
		vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
		vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
		Out.vColor = vShade * pow(2.f, vDepth.b);
		Out.vColor.a = saturate(vShade.a) + vSpecular;
		if (0.0f == Out.vColor.a)
			discard;
	}
	else if (fShaderFlag == SHADER_NONE_SHADE)
	{
		vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
		vDiffuse.rgb = pow(abs(vDiffuse.rgb), g_Gamma);
		Out.vColor = CalcHDRColor(vDiffuse, vDepth.b);
		if (0.0f == Out.vColor.a)
			discard;
	}
	else
	{
		discard;
	}

	float3 eyeToPixel;

	float fViewZ = vDepth.y * g_Far;
	float fShadowRate = 0.f;
	
	vector vPixelWorld;
	vPixelWorld.x = In.vTexUV.x * 2.f - 1.f;
	vPixelWorld.y = In.vTexUV.y * -2.f + 1.f;
	vPixelWorld.z = vDepth.x; /* 0 ~ 1 */
	vPixelWorld.w = 1.0f;
	vPixelWorld *= fViewZ; // pixcel clip space
	
	vPixelWorld = mul(vPixelWorld, g_ProjMatrixInv); // pixel view space
	vPixelWorld = mul(vPixelWorld, g_ViewMatrixInv); // pixel world space

	eyeToPixel = vPixelWorld - g_vCamPosition;

	if (fShaderFlag != SHADER_TOON)
	{
		// 그림자 연산
		vector		vLightViewSpace = mul(vPixelWorld, g_LightViewMatrix);;
		vector		vLightClipSpace = mul(vLightViewSpace, g_LightProjMatrix);
		float2		vNewUV;
		vNewUV.x = (vLightClipSpace.x / vLightClipSpace.w) * 0.5f + 0.5f;
		vNewUV.y = (vLightClipSpace.y / vLightClipSpace.w) * -0.5f + 0.5f;
		
		const float2 TexcelSize = float2( 1.0 / 8192.0,  1.0 / 8192.0);

		[unroll]
		for (int y = -1; y <= 1; ++y)
		{
			[unroll]
			for (int x = -1; x <= 1; ++x)
			{
				float2 offset = float2(x, y) * TexcelSize;
				vector		vShadowDepthInfo = g_ShadowDepthTexture.Sample(LinearSampler, vNewUV + offset);
				if (vLightViewSpace.z - 0.1f > vShadowDepthInfo.x * g_Far)
					fShadowRate += 1.f;
			}
		}
		

		fShadowRate /= 9.f;
		fShadowRate *= 0.5f;
		Out.vColor *= (1.f - fShadowRate);
	}
	

	if (g_bFog)
		Out.vColor.rgb = ApplyFog(Out.vColor.rgb, g_vCamPosition.y, eyeToPixel);

	return Out;
}


PS_OUT PS_OUTLINE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector vFlag = g_OutlineFlagTexture.Sample(LinearSampler, In.vTexUV);

	if (vFlag.a < 0.01f)
		discard;

	float coord[3] = { -vFlag.a, 0.f, vFlag.a };

	float3 vColor = (float3)0.f;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vColor += g_OutlineFlagTexture.Sample(LinearSampler, In.vTexUV + float2(coord[j] / g_iWinCX, coord[i] / g_iWinCY)).rgb;
		}
	}
	vColor /= 9.f;
	
	if (vColor.r != vFlag.r && vColor.g != vFlag.g && vColor.b != vFlag.b)
	{
		Out.vColor = vector(vFlag.rgb, 1.f);
	}
	else
	{
		discard;
	}


	return Out;
}

PS_OUT_LIGHT PS_MAIN_CAPSULE(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	float fShaderFlag = g_DepthTexture.Sample(PointSampler, In.vTexUV).w;
	if (fShaderFlag == SHADER_TOON) 
		discard;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * g_Far;

	/* 0 ~ 1 => -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.0f;
 
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	// g_vLightPos : Capsule Center pos;
	// g_fLightRange : Capsule Radius

    
    // Calculate capsule axis vector
    float3 capsuleAxis = g_vCapsuleEnd.xyz - g_vCapsuleStart.xyz;

	float3 vectorToPoint = vWorldPos.xyz - g_vCapsuleStart.xyz;
	float t = dot(vectorToPoint, capsuleAxis) / dot(capsuleAxis, capsuleAxis);
	float3 closestPoint = g_vCapsuleStart.xyz + saturate(t) * capsuleAxis;
    
    // Calculate closest point on capsule axis to vertex
    // float3 closestPoint = g_vCapsuleStart.xyz + dot(vWorldPos.xyz - g_vCapsuleStart.xyz, capsuleAxis) / dot(capsuleAxis, capsuleAxis) * capsuleAxis;
	// float3 vectorToVertex = vWorldPos.xyz - closestPoint;
	float		fDistance = length(vWorldPos.xyz - closestPoint);
	vector		vLightDir = float4(vWorldPos.xyz - closestPoint, 0.f);
	float		fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;


	if (fShaderFlag == SHADER_DEFAULT)
	{
		Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal)))) * fAtt;
		Out.vShade.a = 1.f;

		vector		vReflect = reflect(normalize(vLightDir), normalize(vNormal));
		vector		vLook = vWorldPos - g_vCamPosition;

		Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f) * fAtt;

		Out.vSpecular.a = 0.f;
	}

	
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

	// 4
	pass Outline
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_OUTLINE();
	}

	// 5
	pass Light_Capsule
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CAPSULE();
	}
}
