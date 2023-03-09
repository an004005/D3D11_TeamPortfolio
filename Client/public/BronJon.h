#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster.h"

BEGIN(Engine)
class CGameInstance;
class CAnimationInstance;
class CAnimation;
class CFSMComponent;
class CRigidBody;
END

BEGIN(Client)
class CBrJ_AnimInstance;

class CBronJon : public CMonster
{
public:
	enum HITAXISTYPE { HAS_FL, HAS_BL, HAS_LL, HAS_RL, HAS_FM, HAS_BM, HAS_LM, HAS_RM, HAS_END };

private:
	CBronJon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBronJon(const CBronJon& rhs);
	virtual ~CBronJon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;

	virtual void AfterPhysX() override;

	void		 Collision();

	_matrix AttachCollider();

	_matrix WeaponRigidBodyMatrix();

private:
	CFSMComponent*			m_pFSM = nullptr;		
	CFSMComponent*			m_pSocketFSM = nullptr;

	CBrJ_AnimInstance*		m_pASM = nullptr;

	CRigidBody*				m_pTrigger = nullptr;
	CRigidBody*				m_pJawRBody = nullptr;

private:
	HRESULT				Setup_AnimSocket();
	list<CAnimation*>	m_GroundDmgSocket;
	list<CAnimation*>	m_DeadAnimSocket;

	HRESULT				Setup_WeakAnimState();

	// Socket Animation 관리용
private:
	list<CAnimation*>	m_HitLightFoward;
	list<CAnimation*>	m_HitLightBack;

	list<CAnimation*>	m_HitMiddleFoward;
	list<CAnimation*>	m_HitMiddleBack;

	list<CAnimation*>	m_HitMiddleLeft;
	list<CAnimation*>	m_HitMiddleRight;

	_bool		m_bStruck = false;

	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;

	HITAXISTYPE	m_Haxistype = HAS_END;

private:
	HRESULT SetUp_Components(void* pArg);

public:
	_bool IsIdle() const { return m_bIdle; }

	// Move(4-Direct)
	_bool IsMoveF() const { return m_bMoveF; }
	_bool IsMoveB() const { return m_bMoveB; }
	_bool IsMoveL_Start() const { return m_bMoveL_Start; }
	_bool IsMoveL_End() const { return m_bMoveL_End; }

	_bool IsMoveR_Start() const { return m_bMoveR_Start; }
	_bool IsMoveR_End() const { return m_bMoveR_End; }

	_bool IsDodgeB() const { return m_bDodgeB; }
	_bool IsDodgeL() const { return m_bDodgeL; }
	_bool IsDodgeR() const { return m_bDodgeR; }

	_bool IsLaserAtkStart() const { return m_bLaserAtkStart; }
	_bool IsLaserAtkIng() const { return m_bLaserAtkIng; }
	_bool IsLaserAtkEnd() const { return m_bLaserAtkEnd; }

	_bool IsBiteAtk() const { return m_bBiteAtk; }
	_bool IsThreat() const { return m_bThreat; }

	_bool IsDamage() const { return m_bDamage; }
	_bool IsDead() const { return m_bDead; }

	// ASM 통제용
	_bool IsStatic() const { return m_bStatic; }

private:
	_bool			m_bIdle = false;

	// Move (4-Direct)
	_bool			m_bMoveF = false;
	_bool			m_bMoveB = false;

	_bool			m_bMoveL_Start = false;
	_bool			m_bMoveL_End = false;
	
	_bool			m_bMoveR_Start = false;
	_bool			m_bMoveR_End = false;

//	_bool			m_bTargetCheck = false;
	_bool			m_bLaserAtkStart = false;
	_bool			m_bLaserAtkIng = false;
	_bool			m_bLaserAtkEnd = false;

	_bool			m_bBiteAtk = false;
	_bool			m_bThreat = false;

	// Socket
	_bool			m_bDamage = false;
	_bool			m_bDead = false;

	// Dodge 3-Direct
	_bool			m_bDodgeB = false;
	_bool			m_bDodgeL = false;
	_bool			m_bDodgeR = false;

private:	//FSM
	_bool			m_bInitialize = false;
	_bool			m_bStatic = false;

	_bool			m_bCreateLaser = false;
	_bool			m_bCreateBite = false;

private:
	wstring			m_ModelName;
	_float			m_fTimeAcc = 0.f;		// Idle 상태를 유지하기 위한 시간
	_float			m_fMovingTime = 0.f;	// 시간이 조건에 찰때까지 움직여라

	_float3			m_fMyPos = { 0.f, 0.f, 0.f };
	_float3			m_fStorePos = { 0.f, 0.f, 0.f }; // 타겟의 위치를 한번만 저장한다던지, 계속 받아둬도 됨
	_float4			m_fCorrect;

	_vector			m_vMyPos;
	_vector			m_vStorePos;
	_vector			m_vDest;

	_uint			m_iNearPattern = 0;
	_uint			m_iFarPattern = 0;

	_bool			m_bNear = false;
	_bool			m_bFar = false;

private:
	
	CGameObject*	m_pPlayer = nullptr;

	public:
		static CBronJon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
		virtual CGameObject* Clone(void* pArg = nullptr) override;
		virtual void Free() override;
};

END