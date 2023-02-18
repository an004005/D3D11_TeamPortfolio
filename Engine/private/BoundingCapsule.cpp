#include "stdafx.h"
#include "..\public\BoundingCapsule.h"

void CBoundingCapsule::Transform(CBoundingCapsule& Out, _fmatrix WorldMatrix) const
{
	Out.m_vTip = XMVector3TransformCoord(m_vTip, WorldMatrix);
	Out.m_vBase = XMVector3TransformCoord(m_vBase, WorldMatrix);

    XMVECTOR dX = XMVector3Dot( WorldMatrix.r[0], WorldMatrix.r[0] );
    XMVECTOR dZ = XMVector3Dot( WorldMatrix.r[2], WorldMatrix.r[2] );

    XMVECTOR d = XMVectorMax( dX, dZ );
    float Scale = sqrtf( XMVectorGetX(d) );
    Out.m_fRadius = m_fRadius * Scale;
}



bool CBoundingCapsule::Intersects(const CBoundingCapsule& OtherCapsule) const
{
	// Çö Ä¸½¶(a)
	const _vector a_Tip = XMLoadFloat3(&m_vTip);
	const _vector a_Base = XMLoadFloat3(&m_vBase);

	_vector a_Normal = XMVector3Normalize(a_Tip - a_Base);
	_vector a_LineEndOffset = a_Normal * m_fRadius;
	_vector a_A = a_Base + a_LineEndOffset;
	_vector a_B = a_Tip - a_LineEndOffset;

	// »ó´ë Ä¸½¶(B)
	const _vector b_Tip = XMLoadFloat3(&OtherCapsule.m_vTip);
	const _vector b_Base = XMLoadFloat3(&OtherCapsule.m_vBase);

	_vector b_Normal = XMVector3Normalize(b_Tip - b_Base);
	_vector b_LineEndOffset = b_Normal * OtherCapsule.m_fRadius;
	_vector b_A = b_Base + b_LineEndOffset;
	_vector b_B = b_Tip - b_LineEndOffset;

	// vector between line endpoints
	_vector v0 = b_A - a_A;
	_vector v1 = b_B - a_A;
	_vector v2 = b_A - a_B;
	_vector v3 = b_B - a_B;

	// squard distance
	_float d0 = XMVectorGetX(XMVector3Dot(v0, v0));
	_float d1 = XMVectorGetX(XMVector3Dot(v1, v1));
	_float d2 = XMVectorGetX(XMVector3Dot(v2, v2));
	_float d3 = XMVectorGetX(XMVector3Dot(v3, v3));

	// select best potential end point on capsule A:
	_vector bestA;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
	{
		bestA = a_B;
	}
	else
	{
		bestA = a_A;
	}

	// select point on capsule B line segment nearest to best potential endpoint on A capsule:
	_vector bestB = ClosestPointOnLineSegment(b_A, b_B, bestA);
	bestA = ClosestPointOnLineSegment(a_A, a_B, bestB);

	_float len = XMVectorGetX(XMVector3Length(bestA - bestB));
	_float penetration_depth = m_fRadius + OtherCapsule.m_fRadius - len;

	return penetration_depth > 0;
}

bool CBoundingCapsule::Intersects(const BoundingSphere& Sphere) const
{
	// Çö Ä¸½¶(a)
	const _vector a_Tip = XMLoadFloat3(&m_vTip);
	const _vector a_Base = XMLoadFloat3(&m_vBase);

	_vector a_Normal = XMVector3Normalize(a_Tip - a_Base);
	_vector a_LineEndOffset = a_Normal * m_fRadius;
	_vector a_A = a_Base + a_LineEndOffset;
	_vector a_B = a_Tip - a_LineEndOffset;

	// »ó´ë ±¸
	_vector b_A = XMLoadFloat3(&Sphere.Center);
	_vector b_B = XMLoadFloat3(&Sphere.Center);

	// vector between line endpoints
	_vector v0 = b_A - a_A;
	_vector v1 = b_B - a_A;
	_vector v2 = b_A - a_B;
	_vector v3 = b_B - a_B;

	// squard distance
	_float d0 = XMVectorGetX(XMVector3Dot(v0, v0));
	_float d1 = XMVectorGetX(XMVector3Dot(v1, v1));
	_float d2 = XMVectorGetX(XMVector3Dot(v2, v2));
	_float d3 = XMVectorGetX(XMVector3Dot(v3, v3));

	// select best potential end point on capsule A:
	_vector bestA;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
	{
		bestA = a_B;
	}
	else
	{
		bestA = a_A;
	}

	// select point on capsule B line segment nearest to best potential endpoint on A capsule:
	_vector bestB = ClosestPointOnLineSegment(b_A, b_B, bestA);
	bestA = ClosestPointOnLineSegment(a_A, a_B, bestB);

	_float len = XMVectorGetX(XMVector3Length(bestA - bestB));
	_float penetration_depth = m_fRadius + Sphere.Radius - len;

	return penetration_depth > 0;
}

bool CBoundingCapsule::Intersects(_float3 vOrigin, _float3 vDir, _float& fDistanceOut) const
{
	const _vector Tip = XMLoadFloat3(&m_vTip);
	const _vector Base = XMLoadFloat3(&m_vBase);

	_vector CapsuleNormal = XMVector3Normalize(Tip - Base);
	_vector LineEndOffset = CapsuleNormal * m_fRadius;
	_vector A = Base + LineEndOffset; // ¾Æ·¡ÂÊ ±¸
	_vector B = Tip - LineEndOffset; // À§ÂÊ ±¸

	const _bool bIntersect = DoesIntersectCapsule(vOrigin, vDir, A, B, m_fRadius);
	if (bIntersect)
	{
		fDistanceOut = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vOrigin) - ((A + B) / 2)));
	}

	return bIntersect;
}

float CBoundingCapsule::dist_Segment_to_Segment(_float3 s1, _float3 s2, _float3 k1, _float3 k2) const
{
	_vector   u = s2 - s1;
	_vector   v = k2 - k1;
	_vector   w = s1 - k1;
	float    a = XMVectorGetX(XMVector3Dot(u, u));
	float    b = XMVectorGetX(XMVector3Dot(u, v));
	float    c = XMVectorGetX(XMVector3Dot(v, v));
	float    d = XMVectorGetX(XMVector3Dot(u, w));
	float    e = XMVectorGetX(XMVector3Dot(v, w));
	float    D = a*c - b*b;  
	float    sc, sN, sD = D;       
	float    tc, tN, tD = D;  
 
	if (D < FLT_EPSILON) { 
		sN = 0.0;        
		sD = 1.0;        
		tN = e;
		tD = c;
	}
	else {                
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0) {        
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}
 
	if (tN < 0.0) {        
		tN = 0.0;
 
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {    
		tN = tD;
 
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
 
	sc = (abs(sN) < FLT_EPSILON ? 0.0 : sN / sD);
	tc = (abs(tN) < FLT_EPSILON ? 0.0 : tN / tD);
 
	_vector  dP = w + (u * sc) - (v * tc);
 
	return XMVectorGetX(XMVector3Length(dP));
}

bool CBoundingCapsule::DoesIntersectCapsule(_float3 eyePos, _float3 myDir, _float3 capsuleA, _float3 capsuleB,
	float radius) const
{
	_float3 end = eyePos + (myDir * 8192);
	auto dist = dist_Segment_to_Segment(eyePos, end, capsuleA, capsuleB);
        
	return dist < radius;
}
