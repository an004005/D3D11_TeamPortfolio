#pragma once

BEGIN(Engine)

// https://wickedengine.net/2020/04/26/capsule-collision-detection/
struct ENGINE_DLL CBoundingCapsule
{
	_float3 m_vTip;
	_float3 m_vBase;
	_float m_fRadius = 0.f;

	CBoundingCapsule(_float3 vTip, _float3 vBase, _float fRadius)
		: m_vTip(vTip), m_vBase(vBase), m_fRadius(fRadius){}

	CBoundingCapsule(const CBoundingCapsule& other)
		: m_vTip(other.m_vTip),
		  m_vBase(other.m_vBase),
		  m_fRadius(other.m_fRadius)
	{
	}

	void Transform(CBoundingCapsule& Out, _fmatrix WorldMatrix) const;

	bool Intersects(const CBoundingCapsule& Capsule) const;
	bool Intersects(const BoundingSphere& Sphere) const;
	bool Intersects(_float3 vOrigin, _float3 vDir, _float& fDistanceOut) const;

private:
	static _vector ClosestPointOnLineSegment(_fvector vA, _fvector vB, _fvector vPoint)
	{
		const _vector AB = vB - vA;
		const _float t = XMVectorGetX(XMVector3Dot(vPoint - vA, vB)) / XMVectorGetX(XMVector3Dot(AB, AB));
		return vA + min(max(t, 0), 1) * AB;
	}

	// https://github.com/dizzzzy/cpp-capsuleIntersection/blob/master/Segment.cpp
    float dist_Segment_to_Segment(_float3 s1, _float3 s2, _float3 k1, _float3 k2) const;
	bool DoesIntersectCapsule(_float3 eyePos, _float3 myDir, _float3 capsuleA, _float3 capsuleB, float radius) const;
};

END