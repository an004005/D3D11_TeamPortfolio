#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Client)

// flower_Hands :: 소즈 포즈
class CEM0210 : public CEnemy
{
private:
	CEM0210(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0210(const CEM0210& rhs);
	virtual ~CEM0210() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void SetUpComponents(void* pArg) override;
	virtual void SetUpSound() override;
	virtual void SetUpAnimationEvent() override;
	virtual void SetUpFSM() override;
	virtual void SetUpUI() override;

	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual _bool Exclude() override;

public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;
	_bool IsRun() const { return m_bRun; }

private:
	void Dodge_VelocityCalc();
	void Play_LightHitAnim();
	void Play_MidHitAnim();


private:
	//충돌 관련 함수 정의
	void Swing_SweepSphere();
	void Somersault_SweepSphere();
	void	HeavyAttackPushStart();

private:
	class CEM0210_Controller*		m_pController = nullptr;
	class CEM0210_AnimInstance*		m_pASM = nullptr;

//	CEffectGroup*				m_pSwingEffect = nullptr;
private:
	//원시 데이터
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
	_bool m_bHitAir = false;

	_bool		m_bHitMove = false;

	// Attack
	_bool		m_bAttack = false;
	
	// Run
	_bool m_bRun = false;

	//dodge
	_float4 m_vDodgeDir;
	_float3 m_vDodgeVelocity;
	_bool m_bDodge = false;

	//Jump
	_float3 m_vOnJumpMoveVelocity;

	//투명
	_bool		m_IsInvisible = false;

	//투명 풀리고 나서 처음 맞았을 때.
	_bool		m_IsFirstHit = false;
	_bool m_bDown = false;
	_bool	m_eSASType = false;
	_double		m_dRenderChangeDelay = 0.0;

	CController::EHandleInput	m_eInput = CController::EHandleInput::HANDLE_END;

	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;
public:
	static CEM0210* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

