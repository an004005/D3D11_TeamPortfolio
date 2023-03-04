#include "stdafx.h"
#include "..\Public\MathUtils.h"
#include "Engine_Defines.h"
#include <cmath>
#include <random>

_bool CMathUtils::FloatCmp(_float f1, _float f2, _float fEpsilon)
{
	return fabs(f1 - f2) <= fEpsilon;
}

_float4 CMathUtils::Quat2EulerDegree(Quaternion q)
{
	_float4 vEuler;
	_float sqw = q.w * q.w;
	_float sqx = q.x * q.x;
	_float sqy = q.y * q.y;
	_float sqz = q.z * q.z;

	vEuler.x = XMConvertToDegrees(asinf(2.f * (q.w * q.x - q.y * q.z)));
	vEuler.y = XMConvertToDegrees(atan2f(2.f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw)));
	vEuler.z = XMConvertToDegrees(atan2f(2.f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw)));

	return vEuler;
}

_float4 CMathUtils::Quat2Euler(Quaternion q)
{
	_float4 vEuler;
	_float sqw = q.w * q.w;
	_float sqx = q.x * q.x;
	_float sqy = q.y * q.y;
	_float sqz = q.z * q.z;

	vEuler.x = asinf(2.f * (q.w * q.x - q.y * q.z));
	vEuler.y = atan2f(2.f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw));
	vEuler.z = atan2f(2.f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw));

	return vEuler;
}

_float CMathUtils::Lerp(_float f1, _float f2, _float fS)
{
	return f1 * (1.f - fS) + f2 * fS;
}

_float CMathUtils::SmoothStep(_float fS)
{
    fS = (fS > 1.0f) ? 1.0f : ((fS < 0.0f) ? 0.0f : fS);  // Clamp value to 0 to 1
    fS = fS * fS*(3.f - 2.f*fS);
	return fS;
}

_float CMathUtils::Saturate(_float fS)
{
	if (fS < 0.f) return 0.f;
	if (fS > 1.f) return 1.f;
	return fS;
}

float CMathUtils::DistPointSegment(_float3 a, _float3 b, _float3 c)
{
	_vector vA = a;
	_vector vB = b;
	_vector vC = c;

	_vector ab = vB - vA;
	_vector ac = vC - vA;
	_vector bc = vC - vB;

	_float e = XMVectorGetX(XMVector3Dot(ac, ab));
	if (e <= 0.f)
		return XMVectorGetX(XMVector3Length(ac));

	_float f = XMVectorGetX(XMVector3Dot(ab, ab));
	if (e >= f)
		return XMVectorGetX(XMVector3Length(bc));

	return sqrtf(XMVectorGetX(XMVector3Dot(ac, ac)) - e * e / f);

	// float e = Dot(ac, ab);
	// // Handle cases where c projects outside ab
	// if (e <= 0.0f) return Dot(ac, ac);
	// float f = Dot(ab, ab);
	// if (e >= f) return Dot(bc, bc);
	// // Handle cases where c projects onto ab
	// return Dot(ac, ac) – e * e / f;
}

void CMathUtils::BillBoard(_float4x4& WorldMatrix, _float4 vCamPos, _float3 vCamUp, _float3* vCamLook)
{
	_matrix _WorldMatrix = WorldMatrix;

	XMVECTOR O = _WorldMatrix.r[3];
	XMVECTOR C = vCamPos;
    XMVECTOR Z = XMVectorSubtract(O, C);

    XMVECTOR N = XMVector3LengthSq(Z);
    if (XMVector3Less(N, g_XMEpsilon))
    {
        if (vCamLook)
        {
            XMVECTOR F = *vCamLook;
            Z = XMVectorNegate(F);
        }
        else
            Z = g_XMNegIdentityR2;
    }
    else
    {
        Z = XMVector3Normalize(Z);
    }

    XMVECTOR up = vCamUp;
    XMVECTOR X = XMVector3Cross(up, Z);
    X = XMVector3Normalize(X);

    XMVECTOR Y = XMVector3Cross(Z, X);

	_float3 vScale = _float3(XMVectorGetX(XMVector3Length(_WorldMatrix.r[0])), 
			XMVectorGetX(XMVector3Length(_WorldMatrix.r[1])), 
			XMVectorGetX(XMVector3Length(_WorldMatrix.r[2])));

	_WorldMatrix.r[0] = X * vScale.x;
	_WorldMatrix.r[1] = Y * vScale.y;
	_WorldMatrix.r[2] = Z * vScale.z;

	WorldMatrix = _WorldMatrix;
}

void CMathUtils::LookAxisLockBillBoard(_float4x4& WorldMatrix, _float4 vCamPos)
{
	_matrix _WorldMatrix = WorldMatrix;
	_vector vLook = _WorldMatrix.r[2];
	_float3 vScale = _float3(XMVectorGetX(XMVector3Length(_WorldMatrix.r[0])), 
			XMVectorGetX(XMVector3Length(_WorldMatrix.r[1])), 
			XMVectorGetX(XMVector3Length(_WorldMatrix.r[2])));

	_vector vPos = _WorldMatrix.r[3];

	_vector vLookAtCam = XMLoadFloat4(&vCamPos) - vPos; // vLookAtCam
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLookAtCam, vLook));
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	_WorldMatrix.r[0] = vRight * vScale.x;
	_WorldMatrix.r[1] = vUp * vScale.y;

	WorldMatrix = _WorldMatrix;
}

void CMathUtils::UpAxisLockBillBoard(_float4x4& WorldMatrix, _float4 vCamPos)
{
	_matrix _WorldMatrix = WorldMatrix;

	_vector vUp = _WorldMatrix.r[1];
	_float3 vScale = _float3(XMVectorGetX(XMVector3Length(_WorldMatrix.r[0])), 
			XMVectorGetX(XMVector3Length(_WorldMatrix.r[1])), 
			XMVectorGetX(XMVector3Length(_WorldMatrix.r[2])));

	_vector vPos = _WorldMatrix.r[3];

	_vector vLookAtCam = XMLoadFloat4(&vCamPos) - vPos;
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLookAtCam));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	_WorldMatrix.r[0] = vRight * vScale.x;
	_WorldMatrix.r[2] = vLook * vScale.z;

	WorldMatrix = _WorldMatrix;
}

void CMathUtils::RightAxisLockBillBoard(_float4x4& WorldMatrix, _float4 vCamPos)
{
	_matrix _WorldMatrix = WorldMatrix;

	_vector vRight = _WorldMatrix.r[0];
	_float3 vScale = _float3(XMVectorGetX(XMVector3Length(_WorldMatrix.r[0])), 
			XMVectorGetX(XMVector3Length(_WorldMatrix.r[1])), 
			XMVectorGetX(XMVector3Length(_WorldMatrix.r[2])));

	_vector vPos = _WorldMatrix.r[3];

	_vector vLookAtCam = XMLoadFloat4(&vCamPos) - vPos;
	_vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLookAtCam));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	_WorldMatrix.r[1] = vUp * vScale.y;
	_WorldMatrix.r[2] = vLook * vScale.z;

	WorldMatrix = _WorldMatrix;
}

_float CMathUtils::RandomFloat(_float fMin, _float fMax)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(fMin, fMax);
	return dis(gen);
}

_float3 CMathUtils::RandomUnitVectorInCone_AxisY(_float fConDegree)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Convert the cone degree to radians
	_float fConRadian = XMConvertToRadians(fConDegree);

    // Generate a random angle around the y-axis
	_float fRadius = sin(fConRadian) / cos(fConRadian);
    _float fAngleRadian = dis(gen) * 2.0f * XM_PI;
    _float x = cos(fAngleRadian) * dis(gen) * fRadius;
	_float y = 1.f;
    _float z = sin(fAngleRadian) * dis(gen) * fRadius;

	return XMVector3Normalize(XMVectorSet(x, y, z, 0.f));
}

_float4 CMathUtils::RandomPosVectorInCircle_FromZero(_float fRadius)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    _float fAngleRadian = dis(gen) * 2.0f * XM_PI;
	_float x = cos(fAngleRadian) * dis(gen) * fRadius;
    _float z = sin(fAngleRadian) * dis(gen) * fRadius;

	return _float4{x, 0.f, z, 1.f};
}

