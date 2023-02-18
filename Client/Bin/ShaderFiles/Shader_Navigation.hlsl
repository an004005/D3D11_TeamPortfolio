
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vColor = (vector)1.f;
float			g_fHeight = 0.f;

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
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	
	vector	vPosition = float4(In.vPosition, 1.f);

	vPosition.y += g_fHeight;

	Out.vPosition = mul(vPosition, matWVP);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;	
};

struct PS_OUT
{

	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_vColor;
	
	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
