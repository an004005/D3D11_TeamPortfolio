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
	float White  : packoffset( c1 );
}

static const float3 LUM_FACTOR = float3(0.299, 0.587, 0.114);

// Reinhard Tonemapper
float4 tonemap_reinhard(float3 color)
{
   color *= 16;
   color = color/(1+color);
   float3 ret = pow(color, Gamma); // gamma
   return float4(ret,1);
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

    float3 ret = pow(abs(ccolor), Gamma); // gamma

    return ret;
}

// Filmic tonemapper
float3 tonemap_filmic(float3 color)
{
    color = max(0, color - 0.004f);
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f)+ 0.06f);

    // result has 1/2.2 baked in
    return pow(color, Gamma);
}

float3 ToneMapping(float3 HDRColor)
{
	// Find the luminance scale for the current pixel
	float LScale = dot(HDRColor, LUM_FACTOR);
	LScale *= MiddleGrey / AvgLum[0];
	LScale = (LScale + LScale * LScale / LumWhiteSqr) / (1.0 + LScale);
	float3 LDRColor = HDRColor * LScale; // Apply the luminance scale to the pixels color
	return pow(LDRColor, 1.f / Gamma); // gamma correction
}

float4 FinalPassPS( VS_OUTPUT In ) : SV_TARGET
{
	// Get the color sample
	float3 color = HDRTex.Sample( PointSampler, In.UV.xy ).xyz;
	
	// Add the bloom contribution
	color += BloomScale * BloomTex.Sample( LinearSampler, In.UV.xy ).xyz;

	// Tone mapping
	color = ToneMapping(color);
	// color = tonemap_reinhard(color);
	// color = tonemap_filmic(color);
	// color = tonemap_uc2(color);

	// Output the LDR value
	return float4(color, 1.0);
}
