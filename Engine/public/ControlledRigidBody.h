#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CControlledRigidBody : public CComponent
{
protected:
	CControlledRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CControlledRigidBody(const CControlledRigidBody& rhs);
	virtual ~CControlledRigidBody() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty();
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	// �ڷ���Ʈ��
	void SetPosition(const _float4& vPos);
	_float4 GetPosition();

	// �̵���, �⺻ �߷¾��� ������ �� �Լ��� �߷� ����� ����մϴ�.
	void Move(_float4 vVelocity, _float fTimeDelta, _float minDist = 0.001f);

protected:
	void CreateController();
	void ReleaseController();

protected:
	physx::PxController* m_pController = nullptr;
	physx::PxCapsuleControllerDesc m_tDesc;
	physx::PxControllerFilters m_Filters;

	physx::PxFilterData m_MoveFilterData;
	ECOLLIDER_TYPE m_eColliderType = CT_PLAYER;


public:
	virtual void Free() override;
	static CControlledRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
};



END