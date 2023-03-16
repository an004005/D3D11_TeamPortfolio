#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "EffectGroup.h"

BEGIN(Engine)
class CFSMComponent;
class CGameInstance;
class CAnimation;
class CRigidBody;
END

BEGIN(Client)


class CBuddyLumi : public CMonster
{
private:
	CBuddyLumi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuddyLumi(const CBuddyLumi& rhs);
	virtual ~CBuddyLumi() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual void SetUp_UI() override;
	virtual void AfterPhysX() override;
	
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;

	void	Swing_SweepCapsule(_bool bCol);

	_matrix AttachCollider(CRigidBody* pRigidBody);

	void HitDir(_double TimeDelta);

//	virtual void SetActive() override;

private:		
	class CBdLm_Controller*		m_pController = nullptr;
	class CBdLm_AnimInstance*	m_pASM = nullptr;

//	CRigidBody*					m_pTrigger = nullptr;
	CRigidBody*					m_pWeaponCollider = nullptr;

	CEffectGroup*				m_pSwingEffect = nullptr;

private:
	CScarletCharacter* m_pTarget = nullptr;

	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
		
	// Dodge
	_float3 m_vOnDodgeMoveVelocityLeft;
	_float m_fDodgeMoveTimeLeft = 0.f;

	_float3 m_vOnDodgeMoveVelocityRight;
	_float m_fDodgeMoveTimeRight = 0.f;

	_float3 m_vOnDodgeMoveVelocityBack;
	_float m_fDodgeMoveTimeBack = 0.f;

	// 家南 局聪皋捞记 焊包

	CAnimation* m_pAtk_Swing = nullptr;

	CAnimation* m_pThreat = nullptr;

	CAnimation* m_pDodgeL = nullptr;
	CAnimation* m_pDodgeR = nullptr;
	CAnimation* m_pDodgeB = nullptr;
	
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

	// Run
	_bool m_bRun = false;

	// Attack
	_bool m_bAtkSwitch = false;
	_bool m_bOneHit = false;

	// Damaged
	_bool		m_bStruck = false;
	_bool		m_bAirStruck = false;

	_bool		m_bAirMaintain = false;

	// HitDir
	_bool		m_bHitMove = false;

	_vector		m_vPreDir;
	_vector		m_vCurDir;
	_vector		m_vFinDir;
	_bool		m_bOneTick = false;
	// ~HitDir

	string		m_strBoneName;

	// TakeDamage 
	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;
	// ~TakeDamage 

	// Swing Attack
	list<CScarletCharacter*> m_CollisionList;

public:
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;
	_bool IsRun() const { return m_bRun; }

public:
	static CBuddyLumi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END