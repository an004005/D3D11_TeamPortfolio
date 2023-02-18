#include "stdafx.h"
#include "Collision_Manger.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CCollision_Manger)

CCollision_Manger::CCollision_Manger()
{
}

void CCollision_Manger::Initialize(_float2 vLeftTop, _float2 vRightBot)
{
	m_vLeftTop = vLeftTop;
	m_vRightBot = vRightBot;
	m_fWidth = m_vRightBot.x - m_vLeftTop.x;
	m_fHeight = m_vLeftTop.y - m_vRightBot.y;
	m_fCellWidth = m_fWidth / s_iStride;
	m_fCellHeight = m_fHeight / s_iStride;
}

void CCollision_Manger::AddStaticCollider(CCollider* pStaticColl)
{
	_uint iCollFrom = 0;
	_uint iCollTo = 0;
	_uint iRowFrom = 0;
	_uint iRowTo = 0;
	GetStrideRange(pStaticColl, iCollFrom, iCollTo, iRowFrom, iRowTo);

	for (_uint i = iRowFrom; i <= iRowTo; ++i)
	{
		for (_uint j = iCollFrom; j <= iCollTo; ++j)
		{
			Safe_AddRef(pStaticColl);
			m_StaticColliders[i][j].push_back(pStaticColl);
		}
	}
}

void CCollision_Manger::AddDynamicCollider(CCollider* pDynamicColl)
{
	_uint iCollFrom = 0;
	_uint iCollTo = 0;
	_uint iRowFrom = 0;
	_uint iRowTo = 0;
	GetStrideRange(pDynamicColl, iCollFrom, iCollTo, iRowFrom, iRowTo);

	if (pDynamicColl->GetOwner() == nullptr)
		return;

	for (_uint i = iRowFrom; i <= iRowTo; ++i)
	{
		for (_uint j = iCollFrom; j <= iCollTo; ++j)
		{
			Safe_AddRef(pDynamicColl);
			m_DynamicColliders[i][j].push_back(pDynamicColl);
		}
	}
}

void CCollision_Manger::ResetDynamicCollider()
{
	for (size_t i = 0; i < s_iStride; ++i)
	{
		for (size_t j = 0; j < s_iStride; ++j)
		{
			for (auto pCollider : m_PreDynamicColliders[i][j])
				Safe_Release(pCollider);
			m_PreDynamicColliders[i][j].clear();

			for (auto pCollider : m_DynamicColliders[i][j])
			{
				if (pCollider->GetOwner())
					m_PreDynamicColliders[i][j].push_back(pCollider);
				else
					Safe_Release(pCollider);
			}
			m_DynamicColliders[i][j].clear();
		}
	}
}

_bool CCollision_Manger::CheckOnCollider(_float4 vPos, _float& fHeight, CGameObject* pOwner, _bool bOnlyStatic)
{
	_uint iColl;
	_uint iRow;
	GetStrideIdx({vPos.x, vPos.z}, iColl, iRow);

	vPos.y += 2.f;
	_vector vOrigin = vPos;
	_vector vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	_float fDistance;
	for (auto pStatic : m_StaticColliders[iRow][iColl])
	{
		if (pStatic->Collision(vOrigin, vDir, fDistance))
		{
			fHeight = XMVectorGetY(vOrigin + vDir * fDistance);
			return true;
		}
	}

	if (bOnlyStatic == false)
	{
		for (auto pDynamic : m_PreDynamicColliders[iRow][iColl])
		{
			if (pDynamic->Collision(vOrigin, vDir, fDistance))
			{
				if (pDynamic->GetOwner() == pOwner)
					continue;

				fHeight = XMVectorGetY(vOrigin + vDir * fDistance);
				return true;
			}
		}
	}

	return false;
}

_bool CCollision_Manger::CheckCollided(CCollider* pCollider, _bool bOnlyStatic)
{
	_uint iCollFrom = 0;
	_uint iCollTo = 0;
	_uint iRowFrom = 0;
	_uint iRowTo = 0;
	GetStrideRange(pCollider, iCollFrom, iCollTo, iRowFrom, iRowTo);

	for (_uint i = iRowFrom; i <= iRowTo; ++i)
	{
		for (_uint j = iCollFrom; j <= iCollTo; ++j)
		{
			for (auto pStatic : m_StaticColliders[i][j])
			{
				if (pCollider->Collision(pStatic))
					return true;
			}

			if (bOnlyStatic == false)
			{
				for (auto pDynamic : m_PreDynamicColliders[i][j])
				{
					if (pCollider->Collision(pDynamic) && pCollider != pDynamic)
						return true;
				}
			}
		}
	}

	return false;
}

_bool CCollision_Manger::SphereTest(const BoundingSphere& Sphere, _float3& v, CGameObject* pOwner, _bool bOnlyStatic)
{
	_float2 vLeftTop{Sphere.Center.x - Sphere.Radius, Sphere.Center.z + Sphere.Radius};
	_float2 vRightBot{Sphere.Center.x + Sphere.Radius, Sphere.Center.z - Sphere.Radius};

	_uint iCollFrom;
	_uint iCollTo;
	_uint iRowFrom;
	_uint iRowTo;

	GetStrideIdx(vLeftTop, iCollFrom, iRowFrom);
	GetStrideIdx(vRightBot, iCollTo, iRowTo);

	v = _float3::Zero;
	_bool bIntersected = false;

	for (_uint i = iRowFrom; i <= iRowTo; ++i)
	{
		for (_uint j = iCollFrom; j <= iCollTo; ++j)
		{
			for (auto pStatic : m_StaticColliders[i][j])
			{
				if (pStatic->GetType() == CCollider::TYPE_AABB)
				{
					if (TestSphereAABB(Sphere, *pStatic->GetAABB(), v))
					{
						bIntersected = true;
					}
				}
				else if (pStatic->GetType() == CCollider::TYPE_OBB)
				{
					if (TestSphereOBB(Sphere, *pStatic->GetOBB(), v))
					{
						bIntersected = true;
					}
				}
			}

			if (bOnlyStatic == false)
			{
				for (auto pDynamic : m_PreDynamicColliders[i][j])
				{
					if (pDynamic->GetOwner() == pOwner)
						continue;

					if (pDynamic->GetType() == CCollider::TYPE_AABB)
					{
						if (TestSphereAABB(Sphere, *pDynamic->GetAABB(), v))
						{
							bIntersected = true;
						}
					}
					else if (pDynamic->GetType() == CCollider::TYPE_OBB)
					{
						if (TestSphereOBB(Sphere, *pDynamic->GetOBB(), v))
						{
							bIntersected = true;
						}
					}
				}
			}
		}
	}

	return bIntersected;
}

_bool CCollision_Manger::GetRayIntersectSingle(_float3 vOrigin, _float3 vDir, CGameObject* pSelf, RAY_INTERSECT_OUT& Out)
{
	_uint iCollTmp;
	_uint iRowTmp;
	GetStrideIdx({ vOrigin.x, vOrigin.z }, iCollTmp, iRowTmp);

	_int iColl = (_int)iCollTmp;
	_int iRow = (_int)iRowTmp;

	_float2 v2Origin{ vOrigin.x, vOrigin.z };
	_float2 v2Dir{ vDir.x, vDir.z };
	v2Dir.Normalize();

	set<CGameObject*> objSet;
	_bool bVisited[s_iStride][s_iStride]{};
	bVisited[iRow][iColl] = true;

	_bool bStatic = false;
	_float fStaticDistance = 0.f;

	bVisited[iRow][iColl] = true;
	const _vector _vOrigin = vOrigin;
	const _vector _vDir = XMVector3Normalize(vDir);

	_float fDistance;

	list<RAY_INTERSECT_OUT> tmp;

	_bool bIntersected = false;
	while (true)
	{
		for (const auto pStatic : m_StaticColliders[iRow][iColl])
		{
			if (pStatic->Collision(vOrigin, vDir, fDistance))
			{
				bIntersected = true;
				tmp.push_back(RAY_INTERSECT_OUT{ fDistance, nullptr });
			}
		}
		 
		for (const auto pDynamic : m_PreDynamicColliders[iRow][iColl])
		{
			if (pDynamic->GetOwner() == pSelf || pDynamic->GetOwner() == nullptr)
				continue;
			if (pDynamic->Collision(vOrigin, vDir, fDistance))
			{
				bIntersected = true;
				const auto Succeed = objSet.insert(pDynamic->GetOwner());
				if (Succeed.second)
				{
					tmp.push_back(RAY_INTERSECT_OUT{fDistance, pDynamic->GetOwner()});
				}
			}
		}

		if (bIntersected)
		{
			RAY_INTERSECT_OUT min{ 99999.f, nullptr };
			for (auto intersected : tmp)
			{
				if (intersected.fDistance < min.fDistance)
				{
					min.fDistance = intersected.fDistance;
					min.pObject = intersected.pObject;
					Out = min;
				}
			}
			return true;
		}

		{
			_bool bFindNextCell = false;

			static constexpr _int dColl[8]{1, -1, 0, 0, 1, 1, -1, -1};
			static constexpr _int dRow[8]{0, 0, 1, -1, 1, -1, -1, 1};
			_float2 v2Dest = v2Origin + v2Dir * 256.f;

			for (int i = 0; i < 8; ++i)
			{
				_int iNextColl = iColl + dColl[i];
				_int iNextRow = iRow + dRow[i];

				if (iNextColl < 0 || iNextRow < 0 || iNextColl >= (_int)s_iStride || iNextRow >= (_int)s_iStride)
					continue;
				if (bVisited[iNextRow][iNextColl])
					continue;

				_float2 vLeftTopCell{m_vLeftTop.x + m_fCellWidth * (_float)iNextColl, m_vLeftTop.y - m_fCellHeight * (_float)iNextRow};
				_float2 vRightBotCell{ vLeftTopCell.x + m_fCellWidth, vLeftTopCell.y - m_fCellHeight };

				if (SegmentIntersectRectangle(vLeftTopCell.x, vRightBotCell.y, vRightBotCell.x, vLeftTopCell.y, v2Origin.x, v2Origin.y, v2Dest.x, v2Dest.y))
				{
					iColl = iNextColl;
					iRow = iNextRow;
					bVisited[iRow][iColl] = true;
					bFindNextCell = true;
					break;
				}
			}

			if (bFindNextCell == false)
				break;
		}
	}

	return false;
}

void CCollision_Manger::GetRayIntersects_UntilStatic(_float3 vOrigin, _float3 vDir, CGameObject* pSelf, vector<RAY_INTERSECT_OUT>& Outs)
{
	_uint iCollTmp;
	_uint iRowTmp;
	GetStrideIdx({ vOrigin.x, vOrigin.z }, iCollTmp, iRowTmp);

	_int iColl = (_int)iCollTmp;
	_int iRow = (_int)iRowTmp;

	_float2 v2Origin{ vOrigin.x, vOrigin.z };
	_float2 v2Dir{ vDir.x, vDir.z };
	v2Dir.Normalize();

	Outs.clear();
	Outs.reserve(10);
	set<CGameObject*> objSet;
	_bool bVisited[s_iStride][s_iStride]{};
	bVisited[iRow][iColl] = true;

	_bool bStatic = false;
	_float fStaticDistance = 0.f;

	bVisited[iRow][iColl] = true;
	const _vector _vOrigin = vOrigin;
	const _vector _vDir = XMVector3Normalize(vDir);

	_float fDistance;

	while (true)
	{
		for (const auto pStatic : m_StaticColliders[iRow][iColl])
		{
			if (pStatic->Collision(vOrigin, vDir, fDistance))
			{
				bStatic = true;
				fStaticDistance = fDistance;
				Outs.push_back(RAY_INTERSECT_OUT{ fDistance, nullptr });
			}
		}
		 
		for (const auto pDynamic : m_PreDynamicColliders[iRow][iColl])
		{
			if (pDynamic->GetOwner() == pSelf || pDynamic->GetOwner() == nullptr)
				continue;
			if (pDynamic->Collision(vOrigin, vDir, fDistance))
			{
				if (bStatic && fStaticDistance < fDistance)
					continue;

				const auto Succeed = objSet.insert(pDynamic->GetOwner());
				if (Succeed.second)
				{
					Outs.push_back(RAY_INTERSECT_OUT{fDistance, pDynamic->GetOwner()});
				}
			}
		}

		if (bStatic)
			break;

		{
			_bool bFindNextCell = false;

			static constexpr _int dColl[8]{1, -1, 0, 0, 1, 1, -1, -1};
			static constexpr _int dRow[8]{0, 0, 1, -1, 1, -1, -1, 1};
			_float2 v2Dest = v2Origin + v2Dir * 256.f;

			for (int i = 0; i < 8; ++i)
			{
				_int iNextColl = iColl + dColl[i];
				_int iNextRow = iRow + dRow[i];

				if (iNextColl < 0 || iNextRow < 0 || iNextColl >= (_int)s_iStride || iNextRow >= (_int)s_iStride)
					continue;
				if (bVisited[iNextRow][iNextColl])
					continue;

				_float2 vLeftTopCell{m_vLeftTop.x + m_fCellWidth * (_float)iNextColl, m_vLeftTop.y - m_fCellHeight * (_float)iNextRow};
				_float2 vRightBotCell{ vLeftTopCell.x + m_fCellWidth, vLeftTopCell.y - m_fCellHeight };

				if (SegmentIntersectRectangle(vLeftTopCell.x, vRightBotCell.y, vRightBotCell.x, vLeftTopCell.y, v2Origin.x, v2Origin.y, v2Dest.x, v2Dest.y))
				{
					iColl = iNextColl;
					iRow = iNextRow;
					bVisited[iRow][iColl] = true;
					bFindNextCell = true;
					break;
				}
			}

			if (bFindNextCell == false)
				break;
		}
	}


	// 거리가 커지는 순으로 정렬
	std::sort(Outs.begin(), Outs.end(), 
		[](const RAY_INTERSECT_OUT& left, const RAY_INTERSECT_OUT& right)
	{
		return left.fDistance < right.fDistance; // 오름차순
	});
}

void CCollision_Manger::GetSphereIntersects(const BoundingSphere& Sphere, CGameObject* pSelf,
                                            list<CGameObject*>& IntersectedObjects)
{
	_float2 vLeftTop{Sphere.Center.x - Sphere.Radius, Sphere.Center.z + Sphere.Radius};
	_float2 vRightBot{Sphere.Center.x + Sphere.Radius, Sphere.Center.z - Sphere.Radius};

	_uint iCollFrom;
	_uint iCollTo;
	_uint iRowFrom;
	_uint iRowTo;

	GetStrideIdx(vLeftTop, iCollFrom, iRowFrom);
	GetStrideIdx(vRightBot, iCollTo, iRowTo);

	_float3 v;
	set<CGameObject*> ObjSet;

	for (_uint i = iRowFrom; i <= iRowTo; ++i)
	{
		for (_uint j = iCollFrom; j <= iCollTo; ++j)
		{
			for (const auto pDynamic : m_PreDynamicColliders[i][j])
			{
				if (pDynamic->GetOwner() == pSelf || pDynamic->GetOwner() == nullptr)
					continue;

				if (pDynamic->GetType() == CCollider::TYPE_AABB)
				{
					if (TestSphereAABB(Sphere, *pDynamic->GetAABB(), v))
					{
						ObjSet.insert(pDynamic->GetOwner());
					}
				}
				else if (pDynamic->GetType() == CCollider::TYPE_OBB)
				{
					if (TestSphereOBB(Sphere, *pDynamic->GetOBB(), v))
					{
						ObjSet.insert(pDynamic->GetOwner());
					}
				}
			}
		}
	}

	IntersectedObjects.assign(ObjSet.begin(), ObjSet.end());
}

void CCollision_Manger::GetStrideRange(CCollider* pCollider, _uint& iCollFrom, _uint& iCollTo, _uint& iRowFrom,
                                       _uint& iRowTo)
{
	_float2 vLeftTop;
	_float2 vRightBot;
	pCollider->Get2D_AABB_YPlane(vLeftTop, vRightBot);

	GetStrideIdx(vLeftTop, iCollFrom, iRowFrom);
	GetStrideIdx(vRightBot, iCollTo, iRowTo);
	//
	// // collider는 판 위에 있어야한다.
	// Assert(vLeftTop.x >= m_vLeftTop.x);
	// Assert(vLeftTop.y <= m_vLeftTop.y);
	// Assert(vRightBot.x <= m_vRightBot.x);
	// Assert(vRightBot.y >= m_vRightBot.y);
	//
	// iCollFrom = static_cast<_uint>(fabs(vLeftTop.x - m_vLeftTop.x) / m_fCellWidth);
	// iCollTo = static_cast<_uint>(fabs(vRightBot.x - m_vLeftTop.x) / m_fCellWidth);
	// iRowFrom = static_cast<_uint>(fabs(m_vLeftTop.y - vLeftTop.y) / m_fCellHeight);
	// iRowTo = static_cast<_uint>(fabs(m_vLeftTop.y - vRightBot.y) / m_fCellHeight);
}

void CCollision_Manger::GetStrideIdx(_float2 vPos, _uint& iColl, _uint& iRow)
{
	// Assert(vPos.x >= m_vLeftTop.x);
	// Assert(vPos.y <= m_vLeftTop.y);
	// Assert(vPos.x <= m_vRightBot.x);
	// Assert(vPos.y >= m_vRightBot.y);

	if (vPos.x < m_vLeftTop.x)
	{
		vPos.x = m_vLeftTop.x;
	}
	else if (vPos.x > m_vRightBot.x)
	{
		vPos.x = m_vRightBot.x;
	}

	if (vPos.y < m_vRightBot.y)
	{
		vPos.y = m_vRightBot.y;
	}
	else if (vPos.y > m_vLeftTop.y)
	{
		vPos.y = m_vLeftTop.y;
	}


	iColl = static_cast<_uint>(fabs(vPos.x - m_vLeftTop.x) / m_fCellWidth);
	iRow = static_cast<_uint>(fabs(vPos.y - m_vLeftTop.y) / m_fCellHeight);
}

void CCollision_Manger::ClosestPtPointAABB(_float3 p, const BoundingBox& AABB, _float3& q)
{
	 // min :  AABB.Center.x - AABB.Extents.x
	// max : AABB.Center.x + AABB.Extents.x

	for (int i = 0; i < 3; i++)
	{
		float v =  ((_float*)&p)[i];
		if (v < ((_float*)&AABB.Center)[i] - ((_float*)&AABB.Extents)[i]) 
			v = ((_float*)&AABB.Center)[i] - ((_float*)&AABB.Extents)[i]; 
		if (v > ((_float*)&AABB.Center)[i] + ((_float*)&AABB.Extents)[i])
			v = ((_float*)&AABB.Center)[i] + ((_float*)&AABB.Extents)[i]; 
		((_float*)&q)[i] = v;
	}
}

_float CCollision_Manger::SqDistPointAABB(_float3 p, const BoundingBox& AABB)
{
	float sqDist = 0.0f;

	for (int i = 0; i < 3; i++) 
	{
	// For each axis count any excess distance outside box extents
		float v = ((_float*)&p)[i];
		if (v < ((_float*)&AABB.Center)[i] - ((_float*)&AABB.Extents)[i])
			sqDist += (((_float*)&AABB.Center)[i] - ((_float*)&AABB.Extents)[i] - v) * (((_float*)&AABB.Center)[i] - ((_float*)&AABB.Extents)[i] - v);
		if (v > ((_float*)&AABB.Center)[i] + ((_float*)&AABB.Extents)[i]) 
			sqDist += (v - ((_float*)&AABB.Center)[i] + ((_float*)&AABB.Extents)[i]) * (v - ((_float*)&AABB.Center)[i] + ((_float*)&AABB.Extents)[i]);
	}
	return sqDist;
}

_bool CCollision_Manger::TestSphereAABB(const BoundingSphere& Sphere, const BoundingBox& AABB, _float3& vPenetrated)
{
	_float3 p;
	ClosestPtPointAABB(Sphere.Center, AABB, p);

	_vector v = p - Sphere.Center;
	float distance_Squared = XMVectorGetX(XMVector3Dot(v, v));

	if (distance_Squared <= Sphere.Radius * Sphere.Radius)
	{
	    if (false == XMVector3NearEqual(v, _float4::Zero, XMVectorSet(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)))
	    {
			v = XMVector3Normalize(v);
	    }
		_vector _vPenetrated = vPenetrated;
		_vPenetrated += v * (Sphere.Radius - XMVectorGetX(XMVector3Length(p - Sphere.Center)));
		vPenetrated = _vPenetrated;
	    return true;
	}

	// No intersection
	return false;
}

void CCollision_Manger::ClosestPtPointOBB(_float3 p, const BoundingOrientedBox& OBB, _float3& q)
{
	_vector d = p - OBB.Center;
	// Start result at center of box; make steps from there
	q = OBB.Center;

	_matrix ToObbRot = _float4x4::CreateFromQuaternion(OBB.Orientation);

	_vector ObbAxis[3]{
		XMVector3TransformNormal(_float3::UnitX, ToObbRot),
		XMVector3TransformNormal(_float3::UnitY, ToObbRot),
		XMVector3TransformNormal(_float3::UnitZ, ToObbRot)
	};
	
	// For each OBB axis...
	for (int i = 0; i < 3; i++) 
	{
		// ...project d onto that axis to get the distance
		// along the axis of d from the box center
		float dist = XMVectorGetX(XMVector3Dot(d, ObbAxis[i]));
		// If distance farther than the box extents, clamp to the box
		if (dist > ((_float*)&OBB.Extents)[i]) 
			dist = ((_float*)&OBB.Extents)[i];
		if (dist < -((_float*)&OBB.Extents)[i])
			dist = -((_float*)&OBB.Extents)[i];
		// Step that distance along the axis to get world coordinate
		q += dist * ObbAxis[i];
	}
}

_bool CCollision_Manger::TestSphereOBB(const BoundingSphere& Sphere, const BoundingOrientedBox& OBB,
	_float3& vPenetrated)
{
	// Find point (p) on OBB closest to Sphere centre
	_float3 p;
	ClosestPtPointOBB(Sphere.Center, OBB, p);

	// Sphere and OBB intersect if the (squared) distance from sphere centre to point (p)
	// is less than the (squared) sphere radius
	_vector v = XMLoadFloat3(&p)- XMLoadFloat3(&Sphere.Center);

	// Vector3.Dot(v, v) gives the square distance to point p
	float distance_Squared = XMVectorGetX(XMVector3Dot(v, v));

	if (distance_Squared <= Sphere.Radius * Sphere.Radius)
	{
	    if (false == XMVector3NearEqual(v, _float4::Zero, XMVectorSet(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)))
	    {
			v = XMVector3Normalize(v);
	    }

		_vector _vPenetrated = vPenetrated;
		_vPenetrated += v * (Sphere.Radius - XMVectorGetX(XMVector3Length(p - Sphere.Center)));
		vPenetrated = _vPenetrated;
	    return true;
	}

	// No intersection
	return false;
}

// https://stackoverflow.com/questions/99353/how-to-test-if-a-line-segment-intersects-an-axis-aligned-rectange-in-2d
bool CCollision_Manger::SegmentIntersectRectangle(_float a_rectangleMinX, _float a_rectangleMinY,
	_float a_rectangleMaxX, _float a_rectangleMaxY, _float a_p1x, _float a_p1y, _float a_p2x, _float a_p2y)
{
    // Find min and max X for the segment

    _float minX = a_p1x;
    _float maxX = a_p2x;

    if(a_p1x > a_p2x)
    {
      minX = a_p2x;
      maxX = a_p1x;
    }

    // Find the intersection of the segment's and rectangle's x-projections

    if(maxX > a_rectangleMaxX)
    {
      maxX = a_rectangleMaxX;
    }

    if(minX < a_rectangleMinX)
    {
      minX = a_rectangleMinX;
    }

    if(minX > maxX) // If their projections do not intersect return false
    {
      return false;
    }

    // Find corresponding min and max Y for min and max X we found before

    _float minY = a_p1y;
    _float maxY = a_p2y;

    _float dx = a_p2x - a_p1x;

    if(fabs(dx) > 0.0000001f)
    {
      _float a = (a_p2y - a_p1y) / dx;
      _float b = a_p1y - a * a_p1x;
      minY = a * minX + b;
      maxY = a * maxX + b;
    }

    if(minY > maxY)
    {
      _float tmp = maxY;
      maxY = minY;
      minY = tmp;
    }

    // Find the intersection of the segment's and rectangle's y-projections

    if(maxY > a_rectangleMaxY)
    {
      maxY = a_rectangleMaxY;
    }

    if(minY < a_rectangleMinY)
    {
      minY = a_rectangleMinY;
    }

    if(minY > maxY) // If Y-projections do not intersect return false
    {
      return false;
    }

    return true;
}

void CCollision_Manger::Free()
{
	for (auto& Row : m_StaticColliders)
	{
		for (auto& Cell : Row)
		{
			for (auto pColl : Cell)
			{
				Safe_Release(pColl);
			}
			Cell.clear();
		}
	}

	for (auto& Row : m_PreDynamicColliders)
	{
		for (auto& Cell : Row)
		{
			for (auto pColl : Cell)
			{
				Safe_Release(pColl);
			}
			Cell.clear();
		}
	}

	for (auto& Row : m_DynamicColliders)
	{
		for (auto& Cell : Row)
		{
			for (auto pColl : Cell)
			{
				Safe_Release(pColl);
			}
			Cell.clear();
		}
	}
}
