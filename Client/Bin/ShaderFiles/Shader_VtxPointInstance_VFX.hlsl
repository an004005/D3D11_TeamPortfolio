#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
	row_major float4x4	Matrix : WORLD;	
};

struct VS_OUT
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
};

struct VS_OUT_MATRIX
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
	row_major float4x4	Matrix : WORLD;	
};

struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
};

struct GS_IN_MATRIX
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
	row_major float4x4	Matrix : WORLD;	
};

struct GS_OUT
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

VS_OUT_MATRIX VS_MAIN_MATRIX(VS_IN In)
{
	VS_OUT_MATRIX		Out = (VS_OUT_MATRIX)0;

	vector	vPosition = mul(float4(In.vPosition, 1.f), In.Matrix);
	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;	
	Out.vPSize = In.vPSize;
	Out.Matrix = In.Matrix;

	return Out;
}

// g_float_0 : y 스케일
[maxvertexcount(6)]
void GS_MAIN_LINE(point GS_IN_MATRIX In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	float3 vRight = matrix_look(In[0].Matrix) * 0.5f;
	
	float3 tmp = g_vCamPosition.xyz - In[0].vPosition;//vLookAtCam
	float3 vUp = normalize(cross(vRight, tmp)) * g_float_0;
	
	float3 vCenter = In[0].vPosition - vRight;
	
	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);
	
	float3		vPosition;
	
	vPosition = vCenter - vRight + vUp;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 0.f);
	
	vPosition = vCenter + vRight + vUp;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(1.f, 0.f);
	
	vPosition = vCenter + vRight - vUp;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 1.f);
	
	vPosition = vCenter - vRight - vUp;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2(0.f, 1.f);
	
	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();
	
	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();

}

// g_tex_0 : 디퓨즈
PS_OUT PS_MAIN_GUN_TRACER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_tex_0.Sample(PointSampler, In.vTexUV.x) * 2.f;
	Out.vColor.a = Out.vColor.r;
	return Out;
}


/***********************
 * BillBoard_FlipBook
 *********************/
VS_OUT VS_MAIN_TIME(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix World = In.Matrix;
	Out.vPSize.x = World[3][3];
	World[3][3] = 1.f;

	vector		vPosition = mul(float4(In.vPosition, 1.f), World);
	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;	

	return Out;
}

// In[0].vPSize.x : 현재 시간
// g_float_1 : 한 프레임 지속 시간
// g_int_0 : 플릭북 가로 개수
// g_int_1 : 플립북 세로 개수
// g_vec2_0 : 텍스쳐 xy 사이즈
[maxvertexcount(6)]
void GS_MAIN_FLIPBOOK( point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];
	
	float3		vLook = g_vCamPosition.xyz - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_vec2_0.x;
	float3		vUp = normalize(cross(vLook, vRight)) * g_vec2_0.y;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = Get_FlipBookUV(float2(0.f, 0.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = Get_FlipBookUV(float2(1.f, 0.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = Get_FlipBookUV(float2(1.f, 1.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = Get_FlipBookUV(float2(0.f, 1.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();

}

// g_tex_0 : 디퓨즈
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 grey = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor = float4(1.f, 197.f / 255.f, 129.f/255.f, 1.f) * 2.f; // Yellow
	Out.vColor.a = grey.r;

	if (Out.vColor.a < 0.01f)
		discard;

	return Out;
}


struct GS_OUT_HIT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fTime : TEXCOORD1;
};

struct PS_IN_HIT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fTime : TEXCOORD1;
};

/******************
 * HIT 이펙트
 *****************/
// In[0].vPSize.x : 현재 시간
[maxvertexcount(6)]
void GS_MAIN_FLIPBOOK_HIT( point GS_IN In[1], inout TriangleStream<GS_OUT_HIT> Vertices)
{
	GS_OUT_HIT		Out[4];
	
	float3		vLook = normalize(g_vCamPosition.xyz - In[0].vPosition) * 0.5f;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_vec2_0.x;
	float3		vUp = normalize(cross(vLook, vRight)) * g_vec2_0.y;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp + vLook;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].fTime = In[0].vPSize.x;

	vPosition = In[0].vPosition - vRight + vUp + vLook;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].fTime = In[0].vPSize.x;

	vPosition = In[0].vPosition - vRight - vUp + vLook;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].fTime = In[0].vPSize.x;

	vPosition = In[0].vPosition + vRight - vUp + vLook;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].fTime = In[0].vPSize.x;

	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();

}

// g_float_0 : 파티클 생성 후 시간
// g_float_1 : 한 프레임 지속 시간
// g_int_0 : 플릭북 가로 개수
// g_int_1 : 플립북 세로 개수
// g_vec2_0 : 텍스쳐 xy 사이즈
PS_OUT PS_MAIN_HIT2(PS_IN_HIT In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor = (float4)1.f;

	float4 grey = g_tex_0.Sample(LinearSampler, Get_FlipBookUV(In.vTexUV, In.fTime, g_float_0, g_int_0, g_int_1));
	if (In.fTime > g_float_0 * 3.f)
		vColor = float4(136.f / 255.f, 8.f/ 255.f, 8.f /255.f, 1.f);
	else
		vColor = saturate(g_tex_1.Sample(LinearSampler, float2(grey.r, 0.5f))) * 2.f;

	Out.vColor = grey.r * vColor;
	Out.vColor.a = grey.a;
	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_HIT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 grey = g_tex_0.Sample(LinearSampler, In.vTexUV);
	Out.vColor = saturate(g_tex_1.Sample(LinearSampler, float2(grey.r, 0.5f))) * 1.5f;
	Out.vColor.a = grey.a;
	return Out;
}

/******************
 * 헤드샷 이펙트 이펙트
 *****************/
// In[0].vPSize.x : 현재 시간
// g_float_1 : 한 프레임 지속 시간
// g_int_0 : 플릭북 가로 개수
// g_int_1 : 플립북 세로 개수
// g_vec2_0 : 텍스쳐 xy 사이즈
[maxvertexcount(6)]
void GS_MAIN_FLIPBOOK_HEADSHOT( point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];
	
	float3		vLook = normalize(g_vCamPosition.xyz - In[0].vPosition) * 0.1f;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_vec2_0.x;
	float3		vUp = normalize(cross(vLook, vRight)) * g_vec2_0.y;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp * 2.f - vLook;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = Get_FlipBookUV(float2(0.f, 0.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition - vRight + vUp * 2.f - vLook;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = Get_FlipBookUV(float2(1.f, 0.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition - vRight - vLook;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = Get_FlipBookUV(float2(1.f, 1.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition + vRight - vLook;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = Get_FlipBookUV(float2(0.f, 1.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();
}

[maxvertexcount(6)]
void GS_MAIN_FLIPBOOK_HEADSHOT2( point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];
	
	float3		vLook = g_vCamPosition.xyz - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_vec2_0.x;
	float3		vUp = normalize(cross(vLook, vRight)) * g_vec2_0.y;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float3		vPosition;

	vPosition = In[0].vPosition + vRight + vUp * 2.f;
	Out[0].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[0].vTexUV = Get_FlipBookUV(float2(0.f, 0.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition - vRight + vUp * 2.f;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[1].vTexUV = Get_FlipBookUV(float2(1.f, 0.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition - vRight;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[2].vTexUV = Get_FlipBookUV(float2(1.f, 1.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	vPosition = In[0].vPosition + vRight;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matVP);
	Out[3].vTexUV = Get_FlipBookUV(float2(0.f, 1.f), In[0].vPSize.x, g_float_0, g_int_0, g_int_1);

	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();
}


technique11 DefaultTechnique
{
	//0
	pass LineTracer
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_MATRIX();
		GeometryShader = compile gs_5_0 GS_MAIN_LINE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GUN_TRACER();
	}

	//1
	// g_float_0 : xy 스케일
	// g_tex_0 : 디퓨즈
	pass BillBoard_FlipBook
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TIME();
		GeometryShader = compile gs_5_0 GS_MAIN_FLIPBOOK();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 2
	pass BillBoard_Hit
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TIME();
		GeometryShader = compile gs_5_0 GS_MAIN_FLIPBOOK_HIT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIT2();
	}

	// 3
	pass BillBoard_HeadShot
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TIME();
		GeometryShader = compile gs_5_0 GS_MAIN_FLIPBOOK_HEADSHOT();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIT();
	}

	// 4
	pass BillBoard_HeadShot2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_TIME();
		GeometryShader = compile gs_5_0 GS_MAIN_FLIPBOOK_HEADSHOT2();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIT();
	}


}
