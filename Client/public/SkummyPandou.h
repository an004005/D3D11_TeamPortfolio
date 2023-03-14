#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CFSMComponent;
class CGameInstance;
class CAnimation;
class CRigidBody;
END

BEGIN(Client)

class CSkummyPandou : public CMonster
{
private:
	CSkummyPandou(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkummyPandou(const CSkummyPandou& rhs);
	virtual ~CSkummyPandou() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	virtual void AfterPhysX();

	_matrix		 AttachCollider(CRigidBody* pRigidBody);

	void		 RushSweep(_bool bCol);

	void		HitDir(_double TimeDelta);

private:
	class CSkPd_Controller*	m_pController = nullptr;
	class CSkPd_AnimInstance* m_pASM = nullptr;

	CRigidBody*				m_pTrigger = nullptr;	
	
private:
	CScarletCharacter* m_pTarget = nullptr;
	
	// Move
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	// Rush Speed
	_bool m_bRush = false;

	// Attack
	_bool m_bAtkSwitch = false;
	_bool m_bAtkToggle = false;
	_bool m_bOneHit = false;
	list<CScarletCharacter*> m_CollisionList;
	// Damage
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

	// TakeDamage
	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;

	// 家南 局聪皋捞记 焊包

	CAnimation* m_pAtk_RushStart = nullptr;
	CAnimation* m_pAtk_RushLoop = nullptr;
	CAnimation* m_pAtk_RushEnd = nullptr;
	
	CAnimation* m_pThreat = nullptr;

	CAnimation* m_pDamage_L_F = nullptr;
	CAnimation* m_pDamage_L_B = nullptr;

	CAnimation* m_pDamage_M_F = nullptr;
	CAnimation* m_pDamage_M_B = nullptr;
	CAnimation* m_pDamage_M_L = nullptr;
	CAnimation* m_pDamage_M_R = nullptr;

	// Air Damage
	CAnimation*	m_pBlowStart = nullptr;
	CAnimation*	m_pBlowLand = nullptr;
	CAnimation*	m_pGetUp = nullptr;
	CAnimation*	m_pRiseStart = nullptr;

	CAnimation* m_pDeadAnim = nullptr;

public:
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;

public:
	static CSkummyPandou* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END