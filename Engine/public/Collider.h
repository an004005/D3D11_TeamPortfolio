#pragma once

#include "Component.h"

/* 충돌이 필요한 객체들에게 충돌체를 추가해주기위해. */
/* 객체가 충돌체를 가지고 있게 만들어주겠다. */
/* 디버깅용으로 그리는 형태의 기능을 만들어주겠다. */

BEGIN(Engine)

typedef struct tagRayIntersectOut
{
	_float fDistance;
	class CGameObject* pObject;
} RAY_INTERSECT_OUT;

class ENGINE_DLL CCollider : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_CAPSULE, TYPE_END };
	static const array<const string, TYPE_END> s_EnumNames;

public:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg);
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

public:
	void ChangeType(CCollider::TYPE eType);
	void Update_Origin(_fmatrix TransformMatrix);
	void Update(_fmatrix TransformMatrix);

	// Y축 평면에 사영한 AABB 형태로 변환
	void Get2D_AABB_YPlane(_float2& vLeftTop, _float2& vRightBot);
	TYPE GetType() const { return m_eType; }
	BoundingBox* GetAABB() { return m_pAABB; }
	BoundingOrientedBox* GetOBB() { return m_pOBB; }
	BoundingSphere* GetSphere() { return m_pSphere; }

	void SetOwner(class CGameObject* pOwner) { m_pOwner = pOwner; }
	class CGameObject* GetOwner() const { return m_pOwner; }
public:
	_bool Collision(class CCollider* pTargetCollider);
	_bool Collision(_fvector vOrigin, _fvector vDir, _float& fDistance);


#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	// origin: world 적용안한 최초의 상태
	TYPE						m_eType = TYPE_END;
	_bool						m_bColl = false;
	_float4x4					m_OriginTransformMatrix;
	class CGameObject*			m_pOwner = nullptr;

	BoundingBox*				m_pAABB_Original = nullptr;
	BoundingBox*				m_pAABB = nullptr;
	BoundingOrientedBox*		m_pOBB_Original = nullptr;
	BoundingOrientedBox*		m_pOBB = nullptr;
	BoundingSphere*				m_pSphere_Original = nullptr;
	BoundingSphere*				m_pSphere = nullptr;
	struct CBoundingCapsule*		m_pCapsule = nullptr;
	struct CBoundingCapsule*		m_pCapsule_Origin = nullptr;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*				m_pBatch = nullptr;
	BasicEffect*										m_pEffect = nullptr;
	ID3D11InputLayout*									m_pInputLayout = nullptr;	
	_float4												m_vColor;
#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);
	// _float3 Compute_Min();
	// _float3 Compute_Max();
	// OBBDESC Compute_OBBDesc();


	

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END