#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

// 브론존
class CEM0800 : public CEnemy
{
private:
	CEM0800(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0800(const CEM0800& rhs);
	virtual ~CEM0800() = default;

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
	virtual _bool IsWeak(CRigidBody* pHitPart);

public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_bool IsPlayingSocket() const;
	_bool	IsTurn() { return m_bTurn; }
	EBaseTurn GetBaseTurn() { return m_eTurn; }

	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();

	EBaseTurn FindTargetDirection();
	void HitWeakProcess(_double TimeDelta);

private:
	//충돌 관련 함수 정의
	void Bite_Overlap();
	void Submergence_Overlap();
	void Laser_SweepSphere();

private:
	class CEM0800_Controller*		m_pController = nullptr;
	class CEM0800_AnimInstance*		m_pASM = nullptr;

	CMaterial* m_pWeak = nullptr;
	CEffectGroup*				m_pWaterPool = nullptr;
	CParticleGroup*			m_pLeftArmBugParticle = nullptr;
	CParticleGroup*			 m_pRightArmBugParticle = nullptr;
private:
	_float3						m_vMoveAxis;
	EBaseTurn				m_eTurn = EBaseTurn::TURN_END;
	_bool						m_bTurn = false;


	// Attack
	_bool m_bLaser = false;
	_float	m_fLaserTime = 0.f;
	_bool m_bComeUp = false;
	
	CController::EHandleInput	m_eInput;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;

	_bool	m_bWeakProcess = false;
	_float4x4	pivot;
public:
	static CEM0800* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

