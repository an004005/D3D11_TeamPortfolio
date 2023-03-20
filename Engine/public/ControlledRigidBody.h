#pragma once
#include "Component.h"

BEGIN(Engine)
	class CEngineControllerHitReport : public physx::PxUserControllerHitReport
{
public:
	virtual ~CEngineControllerHitReport() = default;
	virtual void onShapeHit(const physx::PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const physx::PxControllersHit& hit) override;
	virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) override{}

	void SetPushPower(_float fPushPower) { m_fPushPower = fPushPower; }
	_float GetPushPower() const { return m_fPushPower; }
	void SetHitCallback(const std::function<void(class CGameObject*, ECOLLIDER_TYPE)>& HitCallback, _uint iTargetTypes)
	{
		m_HitCallback = HitCallback;
		m_iTargetTypes = iTargetTypes;
	}

private:
	_float m_fPushPower = 100.f;
	std::function<void(class CGameObject*, ECOLLIDER_TYPE)> m_HitCallback = nullptr;
	_uint m_iTargetTypes = 0;
};

class ENGINE_DLL CControlledRigidBody : public CComponent
{
protected:
	CControlledRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CControlledRigidBody(const CControlledRigidBody& rhs);
	virtual ~CControlledRigidBody() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick() override;
	virtual void Imgui_RenderProperty();
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	// 텔레포트용
	void SetPosition(const _float4& vPos);
	void SetFootPosition(const _float4& vPos);

	_float4 GetPosition();
	_float4 GetFootPosition();

	// 이동용, 기본 중력없기 때문에 이 함수로 중력 만들어 줘야합니다.
	physx::PxControllerCollisionFlags Move(_float4 vVelocity, _float fTimeDelta, _float minDist = 0.001f);
	physx::PxControllerCollisionFlags MoveDisp(_float4 vPosDelta, _float fTimeDelta, _float minDist = 0.001f);

	// 이 캡슐과 어떤 액터가 닿아있으면 틱마다 실행됨(late틱과 after physx 사이에서)
	void SetContactCallback(
		const std::function<void(class CGameObject*, ECOLLIDER_TYPE)>& HitCallback, 
		_uint iTargetTypes = CTB_MONSTER | CTB_MONSTER_PART)
	{
		m_HitReport.SetHitCallback(HitCallback, iTargetTypes);
	}

	_bool IsOnPhysX();
	void SetActive(_bool bActive);

protected:
	void CreateController();
	void ReleaseController();
	void SetDefaultValue();

protected:
	CEngineControllerHitReport m_HitReport;
	physx::PxController* m_pController = nullptr;
	physx::PxCapsuleControllerDesc m_tDesc;
	_float m_fSlopeLimitDegree = 45.f;
	physx::PxControllerFilters m_Filters;

	physx::PxFilterData m_MoveFilterData;
	ECOLLIDER_TYPE m_eColliderType = CT_PLAYER;


public:
	virtual void Free() override;
	static CControlledRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
};

END