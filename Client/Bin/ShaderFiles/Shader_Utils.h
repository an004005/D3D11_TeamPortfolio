#ifndef __SHADER_UTILS_HLSL__
#define __SHADER_UTILS_HLSL__

#define IDENTITY_MATRIX float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)



static const float PI = 3.14159265f;
static const float EPSILON = 1e-6f;

// using in blend or light acc to separate shader step
#define SHADER_DEFAULT 1 << 0
#define SHADER_TOON 1 << 1

#define SHADER_DISTORTION 1 << 2
#define SHADER_SCIFI 1 << 3
#define SHADER_SCIFI_PLAYER_ATTACK 1 << 4

float2 Get_FlipBookUV(float2 vRectUV, float fCurTime, float fFrameTime, int iRowCnt, int iColCnt)
{
	float fTotalTime = fFrameTime * iRowCnt * iColCnt;
	float fTime = fmod(fCurTime, fTotalTime);
	uint iFrame = uint(fTime / fFrameTime); // idx = y * X + x;

	float fRowSize = 1.f / iRowCnt;
	float fColSize = 1.f / iColCnt;

	float2 vOutUV;
	vOutUV.x = vRectUV.x / iRowCnt + (iFrame % iRowCnt) * fRowSize;
	vOutUV.y = vRectUV.y / iColCnt + (iFrame / iRowCnt) * fColSize;

	return vOutUV;
}

float2 Get_ColorGradientUV(float fGrayColor)
{
	return float2(fGrayColor, fGrayColor);
}

// Tiling : wrap 샘플러 사용시의 텍스쳐 반복 여부 결정값
// offset : uv 이동 값
float2 TilingAndOffset(float2 UV, float2 Tiling, float2 Offset)
{
    return UV * Tiling + Offset;
}

float4 ConstantBiasScale(float4 v4)
{
	return (v4 - 0.5f) * 2.f;
}
float2 ConstantBiasScale(float2 v2)
{
	return (v2 - 0.5f) * 2.f;
}
float ConstantBiasScale(float f)
{
	return (f - 0.5f) * 2.f;
}

float4 FlowMapping(Texture2D flowTexture, Texture2D targetTexture, sampler sample, float2 vUV, float time, float strength, float2 flowTiling, float2 tiling)
{
	float2 flowUV = flowTexture.Sample(sample, TilingAndOffset(vUV, flowTiling, float2(0.f, 0.f))).xy;
	flowUV = ConstantBiasScale(flowUV) * -1.f;

	float2 flowUV1 = flowUV * frac(time) * strength + TilingAndOffset(vUV, tiling, float2(0.f, 0.f));
	float4 flowMap1 = targetTexture.Sample(sample, flowUV1);

	float2 flowUV2 = flowUV * frac(time + 0.5f) * strength + TilingAndOffset(vUV, tiling, float2(0.f, 0.f));
	float4 flowMap2 = targetTexture.Sample(sample, flowUV2);

	float4 flowMap = lerp(flowMap1, flowMap2, abs(ConstantBiasScale(frac(time))));

	return flowMap;
}

// InMinMax : x(min), y(max)
// OutMinMax : x(min), y(max)
float4 Remap(float4 In, float2 InMinMax, float2 OutMinMax)
{
    return OutMinMax.x + (In - InMinMax.x) * (OutMinMax.y - OutMinMax.x) / (InMinMax.y - InMinMax.x);
}
float2 Remap(float2 In, float2 InMinMax, float2 OutMinMax)
{
    return OutMinMax.x + (In - InMinMax.x) * (OutMinMax.y - OutMinMax.x) / (InMinMax.y - InMinMax.x);
}
float Remap(float In, float2 InMinMax, float2 OutMinMax)
{
    return OutMinMax.x + (In - InMinMax.x) * (OutMinMax.y - OutMinMax.x) / (InMinMax.y - InMinMax.x);
}

float4 CalcHDRColor(float4 vColor, float fIntensity)
{
	float4 vSatColor = saturate(vColor);
	float4 vHDRColor = vSatColor * pow(2.f, fIntensity);
	vHDRColor.a = vSatColor.a;

	return vHDRColor;
}

float FresnelEffect(float3 Normal, float3 ViewDir, float Power)
{
    return pow((1.0 - saturate(dot(normalize(Normal), normalize(ViewDir)))), Power);
}

float3 matrix_right(float4x4 m)
{
	return float3(m[0][0], m[0][1], m[0][2]);
}
float3 matrix_up(float4x4 m)
{
	return float3(m[1][0], m[1][1], m[1][2]);
}
float3 matrix_look(float4x4 m)
{
	return float3(m[2][0], m[2][1], m[2][2]);
}
float3 matrix_postion(float4x4 m)
{
	return float3(m[3][0], m[3][1], m[3][2]);
}

float PackPostProcessFlag(float Flags, int PostProcessFlag)
{
	return asfloat(asint(Flags) | PostProcessFlag);
}

bool CheckPostProcessFlag(float Flags, int PostProcessFlag)
{
	return asint(Flags) & PostProcessFlag;
}


float4x4 inverse(float4x4 m) {
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
float4 matrix_to_quaternion(float4x4 m)
{
    float tr = m[0][0] + m[1][1] + m[2][2];
    float4 q = float4(0, 0, 0, 0);

    if (tr > 0)
    {
        float s = sqrt(tr + 1.0) * 2; // S=4*qw 
        q.w = 0.25 * s;
        q.x = (m[2][1] - m[1][2]) / s;
        q.y = (m[0][2] - m[2][0]) / s;
        q.z = (m[1][0] - m[0][1]) / s;
    }
    else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
    {
        float s = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2; // S=4*qx 
        q.w = (m[2][1] - m[1][2]) / s;
        q.x = 0.25 * s;
        q.y = (m[0][1] + m[1][0]) / s;
        q.z = (m[0][2] + m[2][0]) / s;
    }
    else if (m[1][1] > m[2][2])
    {
        float s = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2; // S=4*qy
        q.w = (m[0][2] - m[2][0]) / s;
        q.x = (m[0][1] + m[1][0]) / s;
        q.y = 0.25 * s;
        q.z = (m[1][2] + m[2][1]) / s;
    }
    else
    {
        float s = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2; // S=4*qz
        q.w = (m[1][0] - m[0][1]) / s;
        q.x = (m[0][2] + m[2][0]) / s;
        q.y = (m[1][2] + m[2][1]) / s;
        q.z = 0.25 * s;
    }

    return q;
}

float4x4 m_scale(float4x4 m, float3 v)
{
    float x = v.x, y = v.y, z = v.z;

    m[0][0] *= x; m[1][0] *= y; m[2][0] *= z;
    m[0][1] *= x; m[1][1] *= y; m[2][1] *= z;
    m[0][2] *= x; m[1][2] *= y; m[2][2] *= z;
    m[0][3] *= x; m[1][3] *= y; m[2][3] *= z;

    return m;
}

float4x4 quaternion_to_matrix(float4 quat)
{
    float4x4 m = float4x4(float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0));

    float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    m[0][0] = 1.0 - (yy + zz);
    m[0][1] = xy - wz;
    m[0][2] = xz + wy;

    m[1][0] = xy + wz;
    m[1][1] = 1.0 - (xx + zz);
    m[1][2] = yz - wx;

    m[2][0] = xz - wy;
    m[2][1] = yz + wx;
    m[2][2] = 1.0 - (xx + yy);

    m[3][3] = 1.0;

    return m;
}

float4x4 m_translate(float4x4 m, float3 v)
{
    float x = v.x, y = v.y, z = v.z;
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;
    return m;
}

float4x4 compose(float3 position, float4 quat, float3 scale)
{
    float4x4 m = quaternion_to_matrix(quat);
    m = m_scale(m, scale);
    m = m_translate(m, position);
    return m;
}

void decompose(in float4x4 m, out float3 position, out float4 rotation, out float3 scale)
{
    float sx = length(float3(m[0][0], m[0][1], m[0][2]));
    float sy = length(float3(m[1][0], m[1][1], m[1][2]));
    float sz = length(float3(m[2][0], m[2][1], m[2][2]));

    // if determine is negative, we need to invert one scale
    float det = determinant(m);
    if (det < 0) {
        sx = -sx;
    }

    position.x = m[3][0];
    position.y = m[3][1];
    position.z = m[3][2];

    // scale the rotation part

    float invSX = 1.0 / sx;
    float invSY = 1.0 / sy;
    float invSZ = 1.0 / sz;

    m[0][0] *= invSX;
    m[0][1] *= invSX;
    m[0][2] *= invSX;

    m[1][0] *= invSY;
    m[1][1] *= invSY;
    m[1][2] *= invSY;

    m[2][0] *= invSZ;
    m[2][1] *= invSZ;
    m[2][2] *= invSZ;

    rotation = matrix_to_quaternion(m);

    scale.x = sx;
    scale.y = sy;
    scale.z = sz;
}

float4x4 axis_matrix(float3 right, float3 up, float3 forward)
{
    float3 xaxis = right;
    float3 yaxis = up;
    float3 zaxis = forward;
    return float4x4(
		xaxis.x, yaxis.x, zaxis.x, 0,
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0,
		0, 0, 0, 1
	);
}

// http://stackoverflow.com/questions/349050/calculating-a-lookat-matrix
float4x4 look_at_matrix(float3 forward, float3 up)
{
    float3 xaxis = normalize(cross(forward, up));
    float3 yaxis = up;
    float3 zaxis = forward;
    return axis_matrix(xaxis, yaxis, zaxis);
}

float4x4 look_at_matrix(float3 at, float3 eye, float3 up)
{
    float3 zaxis = normalize(at - eye);
    float3 xaxis = normalize(cross(up, zaxis));
    float3 yaxis = cross(zaxis, xaxis);
    return axis_matrix(xaxis, yaxis, zaxis);
}

float Rand(float2 co)
{
	return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}



// float r1 = Rand(float2(x, accTime));
// float r2 = Rand(float2(x * accTime, accTime));
// float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));





float4x4 extract_rotation_matrix(float4x4 m)
{
    float sx = length(float3(m[0][0], m[0][1], m[0][2]));
    float sy = length(float3(m[1][0], m[1][1], m[1][2]));
    float sz = length(float3(m[2][0], m[2][1], m[2][2]));

    // if determine is negative, we need to invert one scale
    float det = determinant(m);
    if (det < 0) {
        sx = -sx;
    }

    float invSX = 1.0 / sx;
    float invSY = 1.0 / sy;
    float invSZ = 1.0 / sz;

    m[0][0] *= invSX;
    m[0][1] *= invSX;
    m[0][2] *= invSX;
    m[0][3] = 0;

    m[1][0] *= invSY;
    m[1][1] *= invSY;
    m[1][2] *= invSY;
    m[1][3] = 0;

    m[2][0] *= invSZ;
    m[2][1] *= invSZ;
    m[2][2] *= invSZ;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

    return m;
}












#endif
