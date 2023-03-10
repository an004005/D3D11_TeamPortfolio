Texture2D<float4> HDRTex   : register( t0 );
StructuredBuffer<float> AvgLum : register( t1 );
Texture2D<float4> BloomTex : register( t2 );

SamplerState PointSampler : register( s0 );
SamplerState LinearSampler : register( s1 );

static const float2 arrBasePos[4] = {
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

static const float2 arrUV[4] = {
	float2(0.0, 0.0),
	float2(1.0, 0.0),
	float2(0.0, 1.0),
	float2(1.0, 1.0),
};

//-----------------------------------------------------------------------------------------
// Vertex shader
//-----------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position : SV_Position; // vertex position 
	float2 UV		: TEXCOORD0;
};

VS_OUTPUT FullScreenQuadVS( uint VertexID : SV_VertexID )
{
    VS_OUTPUT Output;

    Output.Position = float4( arrBasePos[VertexID].xy, 0.0, 1.0);
    Output.UV = arrUV[VertexID].xy;
    
    return Output;    
}

//-----------------------------------------------------------------------------------------
// Pixel shader
//-----------------------------------------------------------------------------------------

cbuffer FinalPassConstants : register( b0 )
{
	// Tone mapping
	float MiddleGrey  : packoffset( c0 );
	float LumWhiteSqr : packoffset( c0.y );
	float BloomScale  : packoffset( c0.z );
	float Gamma  : packoffset( c0.w );
	float FlimSlope  : packoffset( c1 );
}

// static const float3 LUM_FACTOR = float3(0.299, 0.587, 0.114);
static const float3 LUM_FACTOR = float3(0.2126, 0.7152, 0.0722);

static const float3x3 ACESInputMat =
{
	{ 0.59719, 0.35458, 0.04823 },
	{ 0.07600, 0.90834, 0.01566 },
	{ 0.02840, 0.13383, 0.83777 }
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
	{ 1.60475, -0.53108, -0.07367 },
	{ -0.10208,  1.10813, -0.00605 },
	{ -0.00327, -0.07276,  1.07602 }
};

float3 RRTAndODTFit(float3 v)
{
	float3 a = v * (v + 0.0245786f) - 0.000090537f;
	float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
	return a / b;
}

float3 ACESFitted(float3 color)
{
	color = mul(ACESInputMat, color);

	// Apply RRT and ODT
	color = RRTAndODTFit(color);

	color = mul(ACESOutputMat, color);

	// Clamp to [0, 1]
	color = saturate(color);

	return color;
}

// Reinhard Tonemapper
float3 tonemap_reinhard(float3 color)
{
   color *= 16;
   color = color/(1+color);
   float3 ret = pow(color, 1.f); // gamma
   return ret;
}

// Uncharted 2 Tonemapper
float3 tonemap_uncharted2(float3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float3 tonemap_uc2(float3 color)
{
	// default 11.2
    float W = 11.2;

    color *= 16;  // Hardcoded Exposure Adjustment

    float exposure_bias = 2.0f;
    float3 curr = tonemap_uncharted2(exposure_bias*color);

    float3 white_scale = 1.0f/tonemap_uncharted2(W);
    float3 ccolor = curr*white_scale;

    float3 ret = pow(abs(ccolor), 1.f); 

    return ret;
}

// Filmic tonemapper
float3 tonemap_filmic(float3 color)
{
    color = max(0, color - 0.004f);
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f)+ 0.06f);

    // result has 1/2.2 baked in
    return pow(color, 1.f);
}

float3 GammaCorrection(float3 color)
{
	return pow(color, 1.f / Gamma);
}

float3 ToneMapping(float3 HDRColor)
{
	// Find the luminance scale for the current pixel
	float LScale = dot(HDRColor, LUM_FACTOR);
	LScale *= MiddleGrey / AvgLum[0];
	LScale = (LScale + LScale * LScale / LumWhiteSqr) / (1.0 + LScale);
	return HDRColor * LScale; // Apply the luminance scale to the pixels color
}

float4 FinalPassPS( VS_OUTPUT In ) : SV_TARGET
{
	// Get the color sample
	float3 color = HDRTex.Sample(LinearSampler, In.UV.xy ).xyz;
	
	// Add the bloom contribution
	color += BloomScale * BloomTex.Sample( LinearSampler, In.UV.xy ).xyz;

	// Tone mapping

	 //color = tonemap_reinhard(color);
	// color = tonemap_uc2(color);
	// color = tonemap_filmic(color);
	//color = ACESFitted(color);
	color = ToneMapping(color);
	color = GammaCorrection(color);

	// color = GammaCorrection(color);

	// Output the LDR value
	return float4(color, 1.0);
}
