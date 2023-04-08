#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

float g_Radius;
float4 g_vPlayerPos;

struct VS_IN
{
	float3		vPosition : POSITION;

	row_major float4x4	Matrix : WORLD;
	float3		vStartTangent : TEXCOORD0;
	float3		vEndTangent : TEXCOORD1;
};

struct VS_OUT
{
	float3		vPosition : POSITION;
	row_major float4x4	Matrix : WORLD;
	float3		vStartTangent : TEXCOORD0;
	float3		vEndTangent : TEXCOORD1;
};

struct GS_IN
{
	float3		vPosition : POSITION;
	row_major float4x4	Matrix : WORLD;
	float3		vStartTangent : TEXCOORD0;
	float3		vEndTangent : TEXCOORD1;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vNormal : NORMAL;
	float4		vWorldPos : TEXCOORD2;
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vNormal : NORMAL;
	float4		vWorldPos : TEXCOORD2;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
	float4		vFlag : SV_TARGET1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPosition = In.vPosition;
	Out.Matrix = In.Matrix;
	Out.vStartTangent = normalize(In.vStartTangent);
	Out.vEndTangent = normalize(In.vEndTangent);

	return Out;
}

#define SEGMENTCNT 12

// 6 * iRectCnt(12) = 72
[maxvertexcount(SEGMENTCNT * 6)]
void GS_MAIN(line GS_IN In[2], inout TriangleStream<GS_OUT> Vertices)
{
	// iRectCnt(12) * 2 + 2 = 26
	GS_OUT		Out[SEGMENTCNT * 2 + 2];

	matrix Pivot = In[0].Matrix;
	// matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	const uint iRectCnt = SEGMENTCNT;

	matrix PivotWorld = mul(Pivot, g_WorldMatrix);
	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	static const float3 vDirection = float3(0.f, 1.f, 0.f); // up

	float3 vStartTangent = normalize(In[0].vStartTangent); // look
	float3 vStartRight = normalize(cross(vDirection, vStartTangent));
	float3 vStartUp = normalize(cross(vStartTangent, vStartRight)); 
	float3x3 StartRotation = float3x3(vStartRight, vStartUp, vStartTangent);

	float3 vEndTangent = normalize(In[0].vEndTangent);
	float3 vEndRight = normalize(cross(vDirection, vEndTangent)); // up
	float3 vEndUp = normalize(cross(vEndTangent, vEndRight)); // right
	float3x3 EndRotation = float3x3(vEndRight, vEndUp, vEndTangent);

	float fCurDegree = 0.f;
	float fSegmentPerDeg = 360.f / float(iRectCnt);
	float fScale = length(matrix_look(Pivot));
	float fRadius = g_Radius / fScale;
	float fU = 0.f;

	[unroll]
	for (uint i = 0; i <= iRectCnt; ++i)
	{
		uint iEndIdx = i * 2;
		uint iStartIdx = iEndIdx + 1;

		const float theta = (fCurDegree - 90.f) * PI / 180.f;

		Out[iEndIdx].vPosition = float4(cos(theta) * fRadius, -sin(theta) * fRadius, 0.f, 1.f);
		Out[iEndIdx].vPosition.xyz = mul(Out[iEndIdx].vPosition.xyz, EndRotation);
		Out[iEndIdx].vNormal = float4(Out[iEndIdx].vPosition.xyz, 0.f);
		Out[iEndIdx].vPosition.z += 1.f;

		Out[iEndIdx].vWorldPos = mul(Out[iEndIdx].vPosition, PivotWorld);
		Out[iEndIdx].vNormal = normalize(mul(Out[iEndIdx].vNormal, PivotWorld));

		Out[iEndIdx].vPosition = mul(Out[iEndIdx].vWorldPos, matVP);
		Out[iEndIdx].vTexUV = float2(fU, 0.f);

		//////////////////////////
		
		Out[iStartIdx].vPosition = float4(cos(theta) * fRadius, -sin(theta) * fRadius, 0.f, 1.f);
		Out[iStartIdx].vPosition.xyz = mul(Out[iStartIdx].vPosition.xyz, StartRotation);
		Out[iStartIdx].vNormal = float4(Out[iStartIdx].vPosition.xyz, 0.f);

		Out[iStartIdx].vWorldPos = mul(Out[iStartIdx].vPosition, PivotWorld);
		Out[iStartIdx].vNormal = normalize(mul(Out[iStartIdx].vNormal, PivotWorld));

		Out[iStartIdx].vPosition = mul(Out[iStartIdx].vWorldPos, matVP);
		Out[iStartIdx].vTexUV = float2(fU, 1.f);

		fCurDegree += fSegmentPerDeg;
		fU += 1.f / (float)iRectCnt;
	}

	[unroll]
	for (uint j = 0; j < iRectCnt; ++j)
	{
		uint iLeftTop = j * 2;
		uint iLeftBot = iLeftTop + 1;
		uint iRightTop = (j + 1) * 2;
		uint iRightBot = iRightTop + 1;

		Vertices.Append(Out[iLeftTop]);
		Vertices.Append(Out[iRightTop]);
		Vertices.Append(Out[iRightBot]);
		Vertices.RestartStrip();

		Vertices.Append(Out[iLeftTop]);
		Vertices.Append(Out[iRightBot]);
		Vertices.Append(Out[iLeftBot]);
		Vertices.RestartStrip();
	}
}


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vViewDir = g_vCamPosition - In.vWorldPos;
	float fFresnel = FresnelEffect(In.vNormal.xyz, vViewDir.xyz, 0.1f);

	float4 vColor = lerp(float4(1.f, 0.f, 0.f, 0.6f), (float4)1.f, saturate(1.f - fFresnel) * 0.5f);
	Out.vColor = CalcHDRColor(vColor, fFresnel * 2.5f);
	Out.vFlag = float4(0.f, SHADER_POST_OBJECTS, 0.f, 0.f);

	return Out;
}

PS_OUT PS_BRAIN_FIELD_FLOOR(PS_IN In)
{
	PS_OUT			Out = PS_MAIN(In);
	// g_vPlayerPos

	float fLength = length(g_vPlayerPos.xz - In.vWorldPos.xz);
	if (fLength <= 7.f)
	{
		discard;
	}
	else if (fLength <= 15.f)
	{
		Out.vColor.a *= Remap(fLength, float2(7.f, 15.f), float2(0.f, 1.f));
	}
	

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
	pass BrainFieldFloor
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BRAIN_FIELD_FLOOR();
	}
}
