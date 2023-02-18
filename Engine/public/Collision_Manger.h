#pragma once
#include "Base.h"
#include "Collider.h"


BEGIN(Engine)
	class CCollider;
class CGameObject;

class CCollision_Manger : public CBase
{
	DECLARE_SINGLETON(CCollision_Manger)

private:
	CCollision_Manger();
	virtual ~CCollision_Manger() = default;

public:
	void Initialize(_float2 vLeftTop, _float2 vRightBot);

	void AddStaticCollider(CCollider* pStaticColl);
	void AddDynamicCollider(CCollider* pDynamicColl);


	// // 발 밑에 충돌체가 있는지 확인
	_bool CheckOnCollider(_float4 vPos, _float& fHeight, CGameObject* pOwner, _bool bOnlyStatic);
	// // 이 충돌체와 부딪혔는지 확인
	_bool CheckCollided(CCollider* pCollider, _bool bOnlyStatic);

	_bool SphereTest(const BoundingSphere& Sphere, _float3& v, CGameObject* pOwner, _bool bOnlyStatic);
	
	_bool GetRayIntersectSingle(_float3 vOrigin, _float3 vDir, CGameObject* pSelf, OUT RAY_INTERSECT_OUT& Out);
	// static과 충돌할때 까지 ray 충돌 진행, 충돌했다면 마지막 Object는 null로 채워진다.
	void GetRayIntersects_UntilStatic(_float3 vOrigin, _float3 vDir, CGameObject* pSelf, OUT vector<RAY_INTERSECT_OUT>& Outs);
	void GetSphereIntersects(const BoundingSphere& Sphere, CGameObject* pSelf, OUT list<CGameObject*>& IntersectedObjects);

	void ResetDynamicCollider();
	
private:
	void GetStrideRange(CCollider* pCollider, _uint& iCollFrom, _uint& iCollTo, _uint& iRowFrom, _uint& iRowTo);
	void GetStrideIdx(_float2 vPos, _uint& iColl, _uint& iRow);

	void ClosestPtPointAABB(_float3 p, const BoundingBox& AABB, _float3& q);
	_float SqDistPointAABB(_float3 p, const BoundingBox& AABB);
	_bool TestSphereAABB(const BoundingSphere& Sphere, const BoundingBox& AABB, _float3& vPenetrated);
	void ClosestPtPointOBB(_float3 p, const BoundingOrientedBox& OBB, _float3& q);
	_bool TestSphereOBB(const BoundingSphere& Sphere, const BoundingOrientedBox& OBB, _float3& vPenetrated);
	bool SegmentIntersectRectangle(_float a_rectangleMinX,
		_float a_rectangleMinY,
		_float a_rectangleMaxX,
		_float a_rectangleMaxY,
		_float a_p1x,
		_float a_p1y,
		_float a_p2x,
		_float a_p2y);

private:
	static constexpr _uint s_iStride = 20;

	_float2 m_vLeftTop;
	_float2 m_vRightBot;
	_float m_fWidth;
	_float m_fHeight;
	_float m_fCellWidth;
	_float m_fCellHeight;

	array<array<vector<CCollider*>, s_iStride>, s_iStride> m_StaticColliders;
	array<array<list<CCollider*>, s_iStride>, s_iStride> m_PreDynamicColliders;

	array<array<list<CCollider*>, s_iStride>, s_iStride> m_DynamicColliders;


public:
	virtual void Free() override;
};

END