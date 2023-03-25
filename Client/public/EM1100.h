#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CFSMComponent;
class CGameInstance;
class CAnimation;
class CRigidBody;
END

BEGIN(Client)

// 초(고속)파리 :: 방도 팡뒤(Bangdo Fandu)

class CEM1100 : public CEnemy
{
private:
	CEM1100(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM1100(const CEM1100& rhs);
	virtual ~CEM1100() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void SetUpComponents(void* pArg) override;
	virtual void SetUpSound() override;
	virtual void SetUpAnimationEvent() override;
	virtual void SetUpFSM() override;

	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_bool IsRun() const { return m_bRun; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_bool IsPlayingSocket() const;

//	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();

	_bool IsTargetFront();

private:
	//충돌 관련 함수 정의

private:
	class CEM1100_Controller*		m_pController = nullptr;
	class CEM1100_AnimInstance*		m_pASM = nullptr;

	//충돌
	CRigidBody*				m_pRange = nullptr;
	CRigidBody*				m_pBody = nullptr;

private:
	_float3						 m_vMoveAxis;
	_bool						m_bHitAir = false;
	_bool						m_bRun = false;
	//Rush
	_bool						m_bRush = false;
	_bool						m_bChangeDir = false;

	CController::EHandleInput	m_eInput;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;
public:
	static CEM1100* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

