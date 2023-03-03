#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster.h"

BEGIN(Engine)
class CGameInstance;
class CShader;
class CAnimationInstance;
class CAnimation;
class CFSMComponent;
class CModel;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)
class CBdLm_AnimInstance;

class CBuddyLumi : public CMonster
{
public:
	enum HITAXISTYPE {HAS_FL, HAS_BL, HAS_LL, HAS_RL, HAS_FM, HAS_BM, HAS_LM, HAS_RM, HAS_END };

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
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	virtual void AfterPhysX() override;

	_matrix AttachCollider();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;
	CFSMComponent*			m_pSocketFSM = nullptr;

	CBdLm_AnimInstance*		m_pASM = nullptr;
	CRigidBody*				m_pTrigger = nullptr;

	CRigidBody*				m_pWeaponCollider = nullptr;

private:	// 기존 버전 (Test)
	HRESULT				Setup_AnimSocket();
	list<CAnimation*>	m_GroundDmgSocket;
	list<CAnimation*>	m_AirDmgSocket;
	list<CAnimation*>	m_DeadAnimSocket;

	HRESULT				Setup_WeakAnimState();

	// Socket Animation 관리용
private:	// 피격을 좀 더 쪼갤 것 (list<CAnimation*> 멤버 변수로 자세히 상황을 쪼개서 쓰자.
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
	// EventCaller 관리용
public:
	void		Event_AtkCall(_bool bSwitch) { m_bAtkCall = bSwitch; }

	void		Set_Struck(_bool bStruck) { m_bStruck = bStruck; }
	void		Set_Air(_bool bAir) { m_bAir = bAir; }

private:	// EventCaller
	_bool		m_bAtkCall = false;

private:
	HRESULT		SetUp_Components(void* pArg);
	HRESULT		SetUp_Event();

public:
	_bool IsIdle() const { return m_bIdle; }
	_bool IsAir() const { return m_bAir; }
	_bool IsWalk() const { return m_bWalk; }
	_bool IsDodgeB() const { return m_bDodgeB; }
	_bool IsDodgeL() const { return m_bDodgeL; }
	_bool IsDodgeR() const { return m_bDodgeR; }
	_bool IsRun() const { return m_bRun; }
	_bool IsAttack() const { return m_bAttack; }
	_bool IsThreat() const { return m_bThreat; }
	_bool IsDamage() const { return m_bDamage; }
	_bool IsDead() const { return m_bDead; }

// ASM 통제용
	_bool IsStatic() const { return m_bStatic; }

private:
	_bool			m_bIdle = false;
	_bool			m_bAir = false;
	_bool			m_bPreAir = false;
	_bool			m_bWalk = false;	
	_bool			m_bRun = false;
	_bool			m_bAttack = false;
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

private:
	wstring			m_ModelName;

	_float			m_fTimeAcc = 0.f;

	// 위치값 저장 및 실시간 갱신
	_float3			m_fMyPos = { 0.f, 0.f, 0.f };
	_float3			m_fStorePos = { 0.f, 0.f, 0.f };

	_vector			m_vMyPos;
	_vector			m_vStorePos;
	// ~위치값 저장 및 실시간 갱신

	_uint			m_iAfterRunPt = 0;
	_uint			m_iAfterAtkPt = 0;
	_uint			m_iWalkPosition = 0;

private:
	CGameObject*	m_pPlayer = nullptr;

public:
	static CBuddyLumi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END