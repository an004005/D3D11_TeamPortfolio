#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
class CAnimation;
class CRigidBody;
class CGameInstance;
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

	virtual void AfterPhysX() override;
	
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	// 내용물 빼고 삭제
	void		 Collision();

	_matrix AttachCollider(CRigidBody* pRigidBody);

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom = nullptr;
		
	class CBdLm_Controller*		m_pController = nullptr;
	class CBdLm_AnimInstance*	m_pASM = nullptr;

	CRigidBody*					m_pTrigger = nullptr;
	CRigidBody*					m_pWeaponCollider = nullptr;

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

	// 소켓 애니메이션 보관

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
	_bool m_bOneHit = false;

	// Damaged
	_bool		m_bStruck = false;
	_bool		m_bAirStruck = false;

	_bool		m_bMaintain = false;

	_uint		m_iAirDamage = 0;
	_uint		m_iPreAirDamageCnt = 0;

	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;

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