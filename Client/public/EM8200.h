#pragma once
#include "Client_Defines.h"
#include "Enemy.h"
#include "Controller.h"

BEGIN(Client)
class CEM8200 :   public CEnemy
{
private:
	CEM8200(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM8200(const CEM8200& rhs);
	virtual ~CEM8200() = default;

public:
	virtual HRESULT		Initialize(void* pArg) override;

	virtual void		SetUpComponents(void* pArg) override;
	virtual void		SetUpSound() override;
	virtual void		SetUpAnimationEvent() override;
	virtual void		SetUpFSM() override;

	virtual void		BeginTick() override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual void		AfterPhysX() override;

	virtual HRESULT		Render() override;
	virtual void		Imgui_RenderProperty() override;
	virtual void		SetUpUI() override;

public:
	_bool				IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3				GetMoveAxis() const { return m_vMoveAxis; }
	_float				GetTurnRemain() const { return m_fTurnRemain; }
	_bool				IsPlayingSocket() const;
	_bool				IsRun() const { return m_bRun; }

protected:
	// void	Strew_Overlap(); // FlowerShower 공격
	// void	Spin_SweepCapsule();
	// void	Kick_SweepSphere();
	// void	Dodge_VelocityCalc();
	// void	HeavyAttackPushStart();


private:
	void Play_LightHitAnim();
	void Play_MidHitAnim();

private:
	class CEM8200_Controller* m_pController = nullptr;
	class CEM8200_AnimInstance* m_pASM = nullptr;

	// CParticleGroup* m_pFallRoseParticle = nullptr;
	// CParticleGroup* m_pShootFlwParticle = nullptr;

private:
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
	_bool m_bHitAir = false;

	// Jump, dodge
	_bool m_bJumpAttack = false;
	_float3 m_vOnJumpMoveVelocity;

	_bool m_bDodge = false;
	_float4	m_vDodgeDir;
	// ~ Jump, dodge

	// spin atk
	_bool m_bSpinAtk = false;

	// Run 상태를 위한 _bool
	_bool m_bRun = false;

	CController::EHandleInput m_eInput;

	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;

public:
	static CEM8200*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END