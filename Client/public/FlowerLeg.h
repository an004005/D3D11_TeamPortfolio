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


class CFlowerLeg : public CMonster
{
private:
	CFlowerLeg(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlowerLeg(const CFlowerLeg& rhs);
	virtual ~CFlowerLeg() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	virtual void AfterPhysX();

	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;

	void	Strew_Overlap(); // Atk_Strew �浹ü
	void	Spin_SweepCapsule(_bool bCol);
	void	Kick_SweepSphere();

	_matrix AttachCollider(CRigidBody* pRigidBody);

	CRigidBody* Get_TailCol() { return m_pTailCol; }

private:
	class CFL_Controller*	m_pController = nullptr;
	class CFL_AnimInstance*		m_pASM = nullptr;
	
	CRigidBody*				m_pTrigger = nullptr;
	CRigidBody*				m_pTailCol = nullptr;

	CRigidBody*				m_pLeftLeg = nullptr;
	CRigidBody*				m_pRightLeg = nullptr;
	
	// Refine
private:
	CScarletCharacter* m_pTarget = nullptr;

	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	_bool m_bDown = false; // ���� �ǰ� �� �����Ͽ� Down �� ����. ������ �޸� �ܼ� Down ��
	
	// Jump
	_bool m_bJumpAttack = false;
	_float3 m_vOnJumpMoveVelocity;	// �ǽð� �̵� ��ǥ
	_float m_fJumpMoveTime = 0.f;	// JumpStart���� �̵��ϴ� �ð�
	
	// Dodge
	_float3 m_vOnDodgeMoveVelocity;
	_float m_fDodgeMoveTime = 0.f;

	_float3 m_vOnDodgeMoveVelocityRev;
	_float m_fDodgeMoveTimeRev = 0.f;

	_float3 m_vOnDodgeMoveVelocityBack;
	_float m_fDodgeMoveTimeBack = 0.f;

	// ���� �ִϸ��̼� ����
	CAnimation* m_pAtk_Spin = nullptr;		// ȸ�� ����
	CAnimation* m_pAtk_Strew = nullptr;		// ���濡 �� ��Ѹ��� ����

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

	// Run ���¸� ���� _bool
	_bool m_bRun = false;

	// Attack
	_bool m_bAtkSwitch = false;
	_bool m_bOneHit = false;
	list<CScarletCharacter*> m_CollisionList;

	// Damage
	_bool		m_bStruck = false;
	_bool		m_bAirStruck = false;

	_bool		m_bMaintain = false;

	_bool		m_bInvicible = false;

	_uint		m_iAirDamage = 0;
	_uint		m_iPreAirDamageCnt = 0;
	
	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;

public:
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;		
	_bool IsRun() const { return m_bRun; }

	void NotAirStruck(_bool bState) {  m_bAirStruck = bState; }
									
	// ~Refine

public:
	static CFlowerLeg* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END