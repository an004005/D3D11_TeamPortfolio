matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;

/* 빛정보 */
vector			g_vLightDir;
vector			g_vLightPos;
float			g_fRange;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;


/* 재질정보 */
texture2D		g_DiffuseTexture[3];
vector			g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMtrlSpecular = vector(0.2f, 0.2f, 0.2f, 1.f);

/* 지형 셰이딩 */
uint			g_bEditMode;
texture2D		g_BrushTexture;
vector			g_vBrushPos;
float			g_fBrushRange = 5.f;
texture2D		g_FilterTexture;

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



/* 샘플링 해오는 함수 */
/* dx9 : tex2D(DefaultSampler, In.vTexUV);*/
/* dx11 : g_Texture.Sample(DefaultSampler, In.vTexUV); */

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;	
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	/*SV_TARGET0 : 모든 정보가 결정된 픽셀이다. AND 0번째 렌더타겟에 그리기위한 색상이다. */
	float4		vColor : SV_TARGET0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexUV * 30.f);
	vector		vDestDiffuse1 = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexUV * 30.f);
	vector		vDestDiffuse2 = g_DiffuseTexture[2].Sample(LinearSampler, In.vTexUV * 30.f);
	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	

	/*vector		*/
	vector		vBrush = (vector)0.f;
	if (g_bEditMode)
	{
		if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange && 
			g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
		{
			float2		vUV;

			vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
			vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

			vBrush = g_BrushTexture.Sample(LinearSampler, vUV);
		}
	}


	vector		vMtrlDiffuse = vSourDiffuse * vFilter.a + vDestDiffuse1 * (1.f - vFilter.a);
	vMtrlDiffuse = vMtrlDiffuse * vFilter.r + vDestDiffuse2 * (1.f - vFilter.r)  + vBrush;

	vector		vDiffuse = (g_vLightDiffuse * vMtrlDiffuse);	

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f,
		normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	/* 두 벡터 : 빛의 반사벡터, 정점을 바라보는 시선벡터 */
	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f,
		normalize(vLook))), 30.f);

	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ fSpecular * (g_vLightSpecular * g_vMtrlSpecular);

	return Out;
}

technique11 DefaultTechnique
{
	pass Terrain
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
