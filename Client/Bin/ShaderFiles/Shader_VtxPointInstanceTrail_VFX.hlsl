#include "Shader_Defines.h"
#include "Shader_Utils.h"
#include "Shader_Params.h"

matrix			g_TrailData[300];
float g_fLife;
float g_fWidth;
Texture2D g_AlphaTex;
float4 g_Color;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
	row_major float4x4	Matrix : WORLD;	
	uint		instanceID : SV_InstanceID;
};

struct VS_OUT
{
	row_major float4x4	Matrix : WORLD;	
	float		life : TEXCOORD0;
	uint		instanceID : SV_InstanceID;
};

struct GS_IN
{
	row_major float4x4	Matrix : WORLD;	
	float		life : TEXCOORD0;
	uint		instanceID : SV_InstanceID;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		life : TEXCOORD1;
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		life : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix World = In.Matrix;
	Out.life = World[3][3];

	World[0][3] = 0.f;
	World[1][3] = 0.f;
	World[2][3] = 0.f;
	World[3][3] = 1.f;

	Out.Matrix = World;
	Out.instanceID = In.instanceID;

	return Out;
}

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	float CurWidth = g_fWidth * In[0].life / g_fLife * 0.5f;
	float4x4 WorldMatrix = In[0].Matrix;

	if (In[0].instanceID == 0)
	{
		float3		vPosition;
		float3 vCenter = matrix_postion(WorldMatrix);
		float3 vUp = matrix_up(WorldMatrix) * CurWidth;

		vPosition = vCenter + vUp;
		Out[0].vPosition = vector(vPosition, 1.f);
		Out[0].life = In[0].life;
		Out[0].vTexUV = float2(0.f, 0.f);
		
		vPosition = vCenter + vUp;
		Out[1].vPosition = vector(vPosition, 1.f);
		Out[1].life = In[0].life;
		Out[1].vTexUV = float2(1.f, 0.f);
		
		vPosition = vCenter - vUp;
		Out[2].vPosition = vector(vPosition, 1.f);
		Out[2].life = In[0].life;
		Out[2].vTexUV = float2(1.f, 1.f);
		
		vPosition = vCenter - vUp;
		Out[3].vPosition = vector(vPosition, 1.f);
		Out[3].life = In[0].life;
		Out[3].vTexUV = float2(0.f, 1.f);

		matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

		Out[0].vPosition = mul(Out[0].vPosition, matVP);
		Out[1].vPosition = mul(Out[1].vPosition, matVP);
		Out[2].vPosition = mul(Out[2].vPosition, matVP);
		Out[3].vPosition = mul(Out[3].vPosition, matVP);
	}
	else
	{
		matrix PreWorldMatrix = g_TrailData[In[0].instanceID - 1];
		float PreWidth = g_fWidth * PreWorldMatrix[3][3] / g_fLife * 0.5f;

		float3 vPreCenter = matrix_postion(PreWorldMatrix);
		float3 vPreUp = matrix_up(PreWorldMatrix) * PreWidth;

		float3 vCenter = matrix_postion(WorldMatrix);
		float3 vUp = matrix_up(WorldMatrix) * CurWidth;
		
		float3		vPosition;
		
		vPosition = vPreCenter + vPreUp;
		Out[0].vPosition = vector(vPosition, 1.f);
		Out[0].life = In[0].life;
		Out[0].vTexUV = float2(0.f, 0.f);
		
		vPosition = vCenter + vUp;
		Out[1].vPosition = vector(vPosition, 1.f);
		Out[1].life = In[0].life;
		Out[1].vTexUV = float2(1.f, 0.f);
		
		vPosition = vCenter - vUp;
		Out[2].vPosition = vector(vPosition, 1.f);
		Out[2].life = In[0].life;
		Out[2].vTexUV = float2(1.f, 1.f);

		vPosition = vPreCenter - vPreUp;
		Out[3].vPosition = vector(vPosition, 1.f);
		Out[3].life = In[0].life;
		Out[3].vTexUV = float2(0.f, 1.f);

		matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

		Out[0].vPosition = mul(Out[0].vPosition, matVP);
		Out[1].vPosition = mul(Out[1].vPosition, matVP);
		Out[2].vPosition = mul(Out[2].vPosition, matVP);
		Out[3].vPosition = mul(Out[3].vPosition, matVP);
	}


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

	float4 alpha = g_AlphaTex.Sample(LinearSampler, In.vTexUV);

	Out.vColor = CalcHDRColor((alpha.r + 1.f) * g_Color, 1.5f);
	Out.vColor.a = alpha.r;

	return Out;
}

PS_OUT PS_MAIN_ALPHA_DECEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 alpha = g_AlphaTex.Sample(LinearSampler, In.vTexUV);

	Out.vColor = CalcHDRColor((alpha.r + 1.f) * g_Color, 1.5f);
	Out.vColor.a = alpha.r * (In.life / g_fLife);

	return Out;
}


[maxvertexcount(6)]
void GS_FIREWALL(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
	GS_OUT		Out[4];

	float CurWidth = g_fWidth;
	float4x4 WorldMatrix = In[0].Matrix;
	float3 vU = matrix_right(WorldMatrix);

	if (In[0].instanceID == 0)
	{
		float3		vPosition;
		float3 vCenter = matrix_postion(WorldMatrix);
		float3 vUp = matrix_look(WorldMatrix) * CurWidth;

		vPosition = vCenter + vUp;
		Out[0].vPosition = vector(vPosition, 1.f);
		Out[0].life = In[0].life;
		Out[0].vTexUV = float2(0.f, 0.f);
		
		vPosition = vCenter + vUp;
		Out[1].vPosition = vector(vPosition, 1.f);
		Out[1].life = In[0].life;
		Out[1].vTexUV = float2(1.f, 0.f);
		
		vPosition = vCenter;
		Out[2].vPosition = vector(vPosition, 1.f);
		Out[2].life = In[0].life;
		Out[2].vTexUV = float2(1.f, 1.f);
		
		vPosition = vCenter;
		Out[3].vPosition = vector(vPosition, 1.f);
		Out[3].life = In[0].life;
		Out[3].vTexUV = float2(0.f, 1.f);
		matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

		Out[0].vPosition = mul(Out[0].vPosition, matVP);
		Out[1].vPosition = mul(Out[1].vPosition, matVP);
		Out[2].vPosition = mul(Out[2].vPosition, matVP);
		Out[3].vPosition = mul(Out[3].vPosition, matVP);
	}
	else
	{
		matrix PreWorldMatrix = g_TrailData[In[0].instanceID - 1];
		float PreWidth = CurWidth;

		float3 vPreCenter = matrix_postion(PreWorldMatrix);
		float3 vPreUp = matrix_look(PreWorldMatrix) * PreWidth;
		float3 vPreU = matrix_right(PreWorldMatrix);

		float3 vCenter = matrix_postion(WorldMatrix);
		float3 vUp = matrix_look(WorldMatrix) * CurWidth;
		float3 vU = matrix_right(WorldMatrix);
		
		float3		vPosition;
		
		vPosition = vPreCenter + vPreUp;
		Out[0].vPosition = vector(vPosition, 1.f);
		Out[0].life = In[0].life;
		Out[0].vTexUV = float2(vPreU.x, 0.f);
		
		vPosition = vCenter + vUp;
		Out[1].vPosition = vector(vPosition, 1.f);
		Out[1].life = In[0].life;
		Out[1].vTexUV = float2(vU.x, 0.f);
		
		vPosition = vCenter;
		Out[2].vPosition = vector(vPosition, 1.f);
		Out[2].life = In[0].life;
		Out[2].vTexUV = float2(vU.x, 1.f);

		vPosition = vPreCenter;
		Out[3].vPosition = vector(vPosition, 1.f);
		Out[3].life = In[0].life;
		Out[3].vTexUV = float2(vPreU.x, 1.f);
		matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

		Out[0].vPosition = mul(Out[0].vPosition, matVP);
		Out[1].vPosition = mul(Out[1].vPosition, matVP);
		Out[2].vPosition = mul(Out[2].vPosition, matVP);
		Out[3].vPosition = mul(Out[3].vPosition, matVP);
	}


	Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();
	
	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();
}

PS_OUT PS_FIREWALL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 vTileOffsetUV = TilingAndOffset(In.vTexUV, float2(1.f, 1.f), float2(0.f, g_float_0 * 0.6f));

	float erosionDissolve = g_tex_6.Sample(LinearSampler, In.vTexUV).a;
	float4 dissolve = g_tex_0.Sample(LinearSampler, vTileOffsetUV);

	if (erosionDissolve * dissolve.r < g_float_2)
		discard;

	float4 dissolve2 = g_tex_0.Sample(LinearSampler, TilingAndOffset(vTileOffsetUV, float2(2.f, 2.f), float2(0.f, 0.f)));
	if (In.vTexUV.y * dissolve2.r <= 0.1f)
		discard;

	float4 orange = float4(1.f, 68.f / 255.f, 51.f/255.f, 1.f);// orange

	float4 smokeMask = g_tex_2.Sample(LinearSampler, vTileOffsetUV);
	float smokeLightMask = g_tex_4.Sample(LinearSampler, vTileOffsetUV).r;
	Out.vColor = saturate(orange * (saturate(smokeMask.r * 1.5f)  * In.vTexUV.y + smokeLightMask) * 2.f);

	if (In.vTexUV.y * dissolve.r <= 0.3f || dissolve.a <= 0.1f)
		return Out;

	float4 color =  float4(1.f, 197.f / 255.f, 129.f/255.f, 1.f); // Yellow
	float4 flameTearAway = g_tex_3.Sample(LinearSampler, vTileOffsetUV);

	float4 bottomWallBright = g_tex_5.Sample(LinearSampler, In.vTexUV);
	Out.vColor = flameTearAway.r * color + Out.vColor + (bottomWallBright * color);

	return Out;
}

PS_OUT PS_FIREWALL_BOT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 alpha = g_AlphaTex.Sample(LinearSampler, In.vTexUV);

	Out.vColor = CalcHDRColor((alpha.r + 1.f) * g_Color, 1.f);
	Out.vColor.a = alpha.r;

	return Out;
}

technique11 DefaultTechnique
{
	//0
	pass Default
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//1
	pass alphaDecend
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_DECEND();
	}

	//2
	pass FireWall
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_FIREWALL();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FIREWALL();
	}

	//3
	pass FireWallBot
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FIREWALL_BOT();
	}

	

}
