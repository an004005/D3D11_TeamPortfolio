#pragma once

#include "Component.h"

/* 충돌이 필요한 객체들에게 충돌체를 추가해주기위해. */
/* 객체가 충돌체를 가지고 있게 만들어주겠다. */
/* 디버깅용으로 그리는 형태의 기능을 만들어주겠다. */

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
public:
	enum TYPE { TYPE_OBB, TYPE_SPHERE, TYPE_CAPSULE, TYPE_END };
	static const array<const string, TYPE_END> s_EnumNames;

public:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg);
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void Imgui_RenderProperty() override;

public:
	void ChangeType(CCollider::TYPE eType);
	void Update_Origin(_fmatrix TransformMatrix);
	void Update(_fmatrix TransformMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	// origin: world 적용안한 최초의 상태
	TYPE						m_eType = TYPE_END;
	_bool						m_bColl = false;
	_float4x4					m_OriginTransformMatrix;

	BoundingOrientedBox*		m_pOBB_Original = nullptr;
	BoundingOrientedBox*		m_pOBB = nullptr;
	BoundingSphere*				m_pSphere_Original = nullptr;
	BoundingSphere*				m_pSphere = nullptr;
	struct CBoundingCapsule*		m_pCapsule = nullptr;
	struct CBoundingCapsule*		m_pCapsule_Origin = nullptr;

	physx::PxShape* m_pShape = nullptr;
	physx::PxRigidDynamic* m_pActor = nullptr;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*				m_pBatch = nullptr;
	BasicEffect*										m_pEffect = nullptr;
	ID3D11InputLayout*									m_pInputLayout = nullptr;	
	_float4												m_vColor;
#endif // _DEBUG

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END