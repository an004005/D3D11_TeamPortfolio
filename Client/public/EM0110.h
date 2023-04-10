#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

// 드릴말

class CEM0110 : public CEnemy
{
private:
	CEM0110(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0110(const CEM0110& rhs);
	virtual ~CEM0110() = default;

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
	virtual void HitEffect(DAMAGE_PARAM& tDamageParams);
	virtual void CheckHP(DAMAGE_PARAM& tDamageParams);

public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;
	_bool IsRun() const { return m_bRun; }
	void AfterLocal180Turn();
	void Adjust_MoveAxis(_double TimeDelta);

private:
	void Play_HeavyHitAnim();
	void	HeavyAttackPushStart();
	void HitWeakProcess(_double TimeDelta);

private:
	//충돌 관련 함수 정의
	void Rush_Overlap();
	void Rush_SweepSphere();
	void Kick_SweepSphere();
	void AOE_Overlap();

private:
	class CEM0110_Controller*		m_pController = nullptr;
	class CEM0110_AnimInstance*		m_pASM = nullptr;

	CParticleGroup*				m_pBugParticle = nullptr;
	CEffectGroup*				m_pRushEffect = nullptr;

private:
	//원시 데이터
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	//rush
	_float3 m_vRushDirection;
	_bool	m_bRush = false;

	// Attack
	_bool		m_bAttack = false;
	_double	m_dLoopTick = 0.0;

	//Weak
	_int			m_iArmorHp = 0;
	_bool		m_bDestroyArmor = false;
	_bool		m_bArmorProcess = false;
	_bool		m_bWeakProcess = false;

	// Run
	_bool m_bRun = false;

	CMaterial* m_pArmor = nullptr;
	CMaterial* m_pWeak = nullptr;

	CController::EHandleInput	m_eInput = CController::EHandleInput::HANDLE_END;

	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;
	//_float4x4 pivot;
public:
	static CEM0110* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

