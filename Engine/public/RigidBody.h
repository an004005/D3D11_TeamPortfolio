#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody : public CComponent
{
public:
	enum SHAPE { TYPE_BOX, TYPE_SPHERE, TYPE_CAPSULE, TYPE_END };
	static const array<const string, TYPE_END> s_EnumNames;

protected:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty();
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	void SetPxWorldMatrix(const _float4x4& WorldMatrix);
	_float4x4 GetPxWorldMatrix();
	void Activate(_bool bActive);

	// GameObject의 Tick에서 실행 필수
	void Update_Tick(class CTransform* pTransform);
	void Update_Tick(_fmatrix matrix);
	// GameObject의 AfterPhysX에서 실행 필수
	void Update_AfterPhysX(class CTransform* pTransform);

	// 날리기 & 돌리기
	void AddForce(_float3 vForce);
	void AddTorque(_float3 vTorque);

	void Set_Kinetic(_bool bKinematic);

	_bool IsKinematic() const { return m_bKinematic; }
	_bool IsTrigger() const { return m_bTrigger; }
	
	void SetOriginTransform(const _float4x4& OriginMatrix);

	// 콜라이더 타입 변경을 제외한 나머지 설정 변경 가능
	void UpdateChange();

	void CallOnTriggerIn(class CGameObject* TriggeredObj) { if (m_OnTriggerIn) m_OnTriggerIn(TriggeredObj); }
	void SetOnTriggerIn(std::function<void(class CGameObject*)> TriggerIn) { m_OnTriggerIn = TriggerIn; }
	void CallOnTriggerOut(class CGameObject* TriggeredObj) { if (m_OnTriggerOut) m_OnTriggerOut(TriggeredObj); }
	void SetOnTriggerOut(std::function<void(class CGameObject*)> TriggerOut) { m_OnTriggerOut = TriggerOut; }
	void CallOnTriggerPersist(class CGameObject* TriggeredObj) { if (m_OnTriggerPersist) m_OnTriggerPersist(TriggeredObj); }
	void SetOnTriggerPersist(std::function<void(class CGameObject*)> TriggerPersist) { m_OnTriggerPersist = TriggerPersist; }

public:
	_float4x4					Get_OriginMatrix();
	physx::PxBoxGeometry		Get_BoxGeometry();
	physx::PxSphereGeometry		Get_SphereGeometry();
	physx::PxCapsuleGeometry	Get_CapsuleGeometry();
	physx::PxTransform			Get_PxTransform();

protected:
	void ReleaseActor();
	void CreateActor();
	void SetUpActor();

protected:
	physx::PxRigidDynamic* m_pActor = nullptr;
	physx::PxShape* m_pShape = nullptr;

	_bool m_bKinematic = false;
	_bool m_bTrigger = false;
	_float m_fDensity = 10.f;
	ECOLLIDER_TYPE m_eColliderType = CT_STATIC;

	_float4x4 m_OriginTransformMatrix;
	SHAPE m_eShapeType = TYPE_BOX;

	std::function<void(class CGameObject*)> m_OnTriggerIn = nullptr;
	std::function<void(class CGameObject*)> m_OnTriggerOut = nullptr;
	std::function<void(class CGameObject*)> m_OnTriggerPersist = nullptr;


public:
	virtual void Free() override;
	static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
};

END