#ifndef __SHADER_DEFINES_HLSL__
#define __SHADER_DEFINES_HLSL__
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

sampler LinearSampler_Clamp = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = CLAMP;
	AddressV = CLAMP;
};


RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_NonCulling
{
	FillMode = Solid;
	CullMode = none;
	FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_CW
{
	CullMode = Front;
	FrontCounterClockwise = false;
};

// RasterizerState RS_NonCulling
// {
// 	FillMode = wireframe;
// 	CullMode = none;
// 	FrontCounterClockwise = false;
// };

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

	
//bool BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend[0] = SRC_ALPHA;
	DestBlend[0] = INV_SRC_ALPHA;
	BlendOp[0] = Add;
};

BlendState BS_One
{
	BlendEnable[0] = true;
	SrcBlend[0] = ONE;
	DestBlend[0] = ONE;
	BlendOp[0] = Add;
};

BlendState BS_AlphaOne
{
	BlendEnable[0] = true;
	SrcBlend[0] = SRC_ALPHA;
	DestBlend[0] = ONE;
	BlendOp[0] = Add;
};

#endif // __DEPENDENCY_HLSL__