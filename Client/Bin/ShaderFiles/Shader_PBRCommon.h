#ifndef __SHADER_PBR_HLSL__
#define __SHADER_PBR_HLSL__
#include "Shader_Utils.h"


// Shlick's approximation of Fresnel
// https://en.wikipedia.org/wiki/Schlick%27s_approximation
float3 Fresnel_Shlick(float3 f0, float3 f90, float x)
{
    return f0 + (f90 - f0) * pow(1.f - x, 5.f);
}

// Burley B. "Physically Based Shading at Disney"
// SIGGRAPH 2012 Course: Practical Physically Based Shading in Film and Game Production, 2012.
float Diffuse_Burley(float NdotL, float NdotV, float LdotH, float roughness)
{
    float fd90 = 0.5f + 2.f * roughness * LdotH * LdotH;
    return Fresnel_Shlick(1, fd90, NdotL).x * Fresnel_Shlick(1, fd90, NdotV).x;
}

// GGX specular D (normal distribution)
// https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
float Specular_D_GGX(float alpha, float NdotH)
{
    const float alpha2 = alpha * alpha;
    const float lower = (NdotH * NdotH * (alpha2 - 1)) + 1;
    return alpha2 / max(EPSILON, PI * lower * lower);
}

// Schlick-Smith specular G (visibility) with Hable's LdotH optimization
// http://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf
// http://graphicrants.blogspot.se/2013/08/specular-brdf-reference.html
float G_Shlick_Smith_Hable(float alpha, float LdotH)
{
    return rcp(lerp(LdotH * LdotH, 1, alpha * alpha * 0.25f));
}

// A microfacet based BRDF.
//
// alpha:           This is roughness * roughness as in the "Disney" PBR model by Burley et al.
//
// specularColor:   The F0 reflectance value - 0.04 for non-metals, or RGB for metals. This follows model 
//                  used by Unreal Engine 4.
//
// NdotV, NdotL, LdotH, NdotH: vector relationships between,
//      N - surface normal
//      V - eye normal
//      L - light normal
//      H - half vector between L & V.
float3 Specular_BRDF(float alpha, float3 specularColor, float NdotV, float NdotL, float LdotH, float NdotH)
{
    // Specular D (microfacet normal distribution) component
    float specular_D = Specular_D_GGX(alpha, NdotH);

    // Specular Fresnel
    float3 specular_F = Fresnel_Shlick(specularColor, 1, LdotH);

    // Specular G (visibility) component
    float specular_G = G_Shlick_Smith_Hable(alpha, LdotH);

    return specular_D * specular_F * specular_G;
}

// Diffuse irradiance
float3 Diffuse_IBL(float3 N)
{
    return g_IrradianceTexture.Sample(CubeSampler, N);
}
//
// // Approximate specular image based lighting by sampling radiance map at lower mips 
// // according to roughness, then modulating by Fresnel term. 
float3 Specular_IBL(float3 N, float3 V, float lodBias)
{
    // float mip = lodBias * NumRadianceMipLevels;
    // float3 dir = reflect(-V, N);
    // return g_RadianceTexture.SampleLevel(CubeSampler, dir, mip);

    float mip = lodBias * 5;
    float3 dir = reflect(-V, N);
    return g_RadianceTexture.SampleLevel(CubeSampler, dir, mip) * 0.3f;
}


// Apply Disney-style physically based rendering to a surface with:
//
// V, N:             Eye and surface normals
//
// numLights:        Number of directional lights.
//
// lightColor[]:     Color and intensity of directional light.
//
// lightDirection[]: Light direction.
float3 LightSurface(
    float3 V, float3 N, float3 lightColor, float3 lightDirection,
    float3 albedo, float roughness, float metallic, float ambientOcclusion)
{
    // Specular coefficiant - fixed reflectance value for non-metals
    static const float kSpecularCoefficient = 0.04;

    const float NdotV = saturate(dot(N, V));

    // Burley roughness bias
    const float alpha = roughness * roughness;

    // Blend base colors
    const float3 c_diff = lerp(albedo, float3(0, 0, 0), metallic)       * ambientOcclusion;
    const float3 c_spec = lerp(kSpecularCoefficient, albedo, metallic)  * ambientOcclusion;

    // Output color
    float3 acc_color = 0;

    // Accumulate light values
    // light vector (to light)
    const float3 L = normalize(-lightDirection);

    // Half vector
    const float3 H = normalize(L + V);

    // products
    const float NdotL = saturate(dot(N, L));
    const float LdotH = saturate(dot(L, H));
    const float NdotH = saturate(dot(N, H));

    // Diffuse & specular factors
    float diffuse_factor = Diffuse_Burley(NdotL, NdotV, LdotH, roughness);
    float3 specular = Specular_BRDF(alpha, c_spec, NdotV, NdotL, LdotH, NdotH);

    // Directional light
    acc_color += NdotL * lightColor * (((c_diff * diffuse_factor) + specular));

    // // Add diffuse irradiance
    float3 diffuse_env = Diffuse_IBL(N);
    acc_color += c_diff * diffuse_env;

    // acc_color += c_diff * 0.4f;


    // // Add specular radiance 
    float3 specular_env = Specular_IBL(N, V, roughness);
    acc_color += c_spec * specular_env;

    return acc_color;
}


#endif
