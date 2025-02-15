#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "ParticleGroup.h"
#include "EffectGroup.h"

BEGIN(Engine)
class CFSMComponent;
class CGameInstance;
class CAnimation;
class CRigidBody;
END

BEGIN(Client)


class CFlowerLeg_Invisible : public CMonster
{
private:
	CFlowerLeg_Invisible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlowerLeg_Invisible(const CFlowerLeg_Invisible& rhs);
	virtual ~CFlowerLeg_Invisible() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	
	virtual void AfterPhysX() override;

	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	//virtual void SetActive() override;

	void	Strew_Overlap(); // Atk_Strew 충돌체
	void	Spin_SweepCapsule(_bool bCol);
	void	Kick_SweepSphere();

	_matrix AttachCollider(CRigidBody* pRigidBody);

	void HitDir(_double TimeDelta);

	CRigidBody* Get_TailCol() { return m_pTailCol; }

	// De buff
	virtual void DeBuff_End() override;
	virtual void DeBuff_Fire() override;
	virtual void DeBuff_Oil() override;
	// ~De buff

private:
	class CFLInvisible_Controller*	m_pController = nullptr;
	class CFLInvisible_AnimInstance*		m_pASM = nullptr;
	
	CRigidBody*				m_pTrigger = nullptr;
	CRigidBody*				m_pTailCol = nullptr;

	CRigidBody*				m_pLeftLeg = nullptr;
	CRigidBody*				m_pRightLeg = nullptr;
	
	CParticleGroup*			m_pFallRoseParticle = nullptr;
	CParticleGroup*			m_pShootFlwParticle = nullptr;


	// Refine
private:
	CScarletCharacter* m_pTarget = nullptr;

	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	_bool m_bDown = false; // 공중 피격 후 낙하하여 Down 된 상태. 보스와 달리 단순 Down 뿐
	
	// Jump
	_bool m_bJumpAttack = false;
	_float3 m_vOnJumpMoveVelocity;	// 실시간 이동 좌표
	_float m_fJumpMoveTime = 0.f;	// JumpStart동안 이동하는 시간
	
	// Dodge
	_float3 m_vOnDodgeMoveVelocity;
	_float m_fDodgeMoveTime = 0.f;

	_float3 m_vOnDodgeMoveVelocityRev;
	_float m_fDodgeMoveTimeRev = 0.f;

	_float3 m_vOnDodgeMoveVelocityBack;
	_float m_fDodgeMoveTimeBack = 0.f;

	// 소켓 애니메이션 보관
	CAnimation* m_pAtk_Spin = nullptr;		// 회전 공격
	CAnimation* m_pAtk_Strew = nullptr;		// 전방에 꽃 흩뿌리는 공격

	CAnimation* m_pJumpStart = nullptr;
	CAnimation* m_pJumpLanding = nullptr;

	CAnimation* m_pDodgeL_Start = nullptr;
	CAnimation* m_pDodgeL_Stop = nullptr;

	CAnimation* m_pDodgeR_Start = nullptr;
	CAnimation* m_pDodgeR_Stop = nullptr;

	CAnimation* m_pDodgeB_Start = nullptr;
	CAnimation* m_pDodgeB_Stop = nullptr;

	CAnimation* m_pThreat = nullptr;

	CAnimation* m_pDamage_L_F = nullptr;
	CAnimation* m_pDamage_L_B = nullptr;

	CAnimation* m_pDamage_M_F = nullptr;
	CAnimation* m_pDamage_M_B = nullptr;
	CAnimation* m_pDamage_M_L = nullptr;
	CAnimation* m_pDamage_M_R = nullptr;

	CAnimation* m_pBlowStart = nullptr; 
	CAnimation* m_pBlowLand = nullptr;
	CAnimation* m_pGetUp = nullptr;
	CAnimation* m_pRiseStart = nullptr;
	
	CAnimation* m_pDeadAnim = nullptr;

	// Run 상태를 위한 _bool
	_bool m_bRun = false;

	// Attack
	_bool m_bAtkSwitch = false;
	_bool m_bOneHit = false;
	list<CScarletCharacter*> m_CollisionList;

	// Damaged
	_bool		m_bStruck = false;
	_bool		m_bAirStruck = false;

	_bool		m_bAirMaintain = false;

//	_bool		m_bInvisible = false;
	_bool		m_bUntouchable = false;

	// HitDir
	_bool		m_bHitMove = false;
	
	_vector		m_vPreDir;
	_vector		m_vCurDir;
	_vector		m_vFinDir;
	_bool		m_bOneTick = false;
	// ~HitDir

	// TakeDamage 
	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;
	// ~TakeDamage 

public:
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;		
	_bool IsRun() const { return m_bRun; }

	void NotAirStruck(_bool bState) {  m_bAirStruck = bState; }
									
	// ~Refine

public:
	static CFlowerLeg_Invisible* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END