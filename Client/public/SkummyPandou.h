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
class CSkPd_AnimInstance;

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

	virtual void AfterPhysX();

	void StateCheck(_double TimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;

	CSkPd_AnimInstance*		m_pASM = nullptr;
	CRigidBody*				m_pTrigger = nullptr;	
	CRigidBody*				m_pSearch = nullptr;

private:
	HRESULT				Setup_AnimSocket();
	list<CAnimation*>	m_AirDmgSocket;
	list<CAnimation*>	m_DeadAnimSocket;

private:
	HRESULT SetUp_Components(void* pArg);

public:
	_bool IsIdle() const { return m_bIdle; }
	
	// Move(4-Direct)
	_bool IsMoveF() const { return m_bMoveF; }
	_bool IsMoveB() const { return m_bMoveB; }
	_bool IsMoveL() const { return m_bMoveL; }
	_bool IsMoveR() const { return m_bMoveR; }

	_bool IsRandomMove() const { return m_bRandomMove; }
	_bool CheckCurrentRatio() const { return m_bCurrentRatioSave; }

	_bool IsAttackStart() const { return m_bAttackStart; }
	_bool IsAttacking() const { return m_bAttacking; }
	_bool IsAttackEnd() const { return m_bAttackEnd; }

	_bool IsThreat() const { return m_bThreat; }
	
	_bool IsDamage() const { return m_bDamage; }
	_bool IsDead() const { return m_bDead; }

	// ASM 통제용
	_bool IsStatic() const { return m_bStatic; }

	_float	GetPlayRatio() { return m_fPlayRatio; }

private:
	_bool			m_bIdle = false;

	// Move
	// Move
	_bool			m_bMoveF = false;
	_bool			m_bMoveB = false;
	_bool			m_bMoveL = false;
	_bool			m_bMoveR = false;

	_bool			m_bRandomMove = false;
	_bool			m_bCurrentRatioSave = false;	// RandomMove를 끊어주기 위한 _bool

	_bool			m_bAttackStart = false;
	_bool			m_bAttacking = false;
	_bool			m_bAttackEnd = false;

	_bool			m_bThreat = false;

	// Socket
	_bool			m_bDamage = false;
	_bool			m_bDead = false;

	// ASM Control
	_bool			m_bStatic = false;
	_bool			m_bInitialize = false;

	// Out of Sight Check
	_bool			m_bArea = false;
	// Search Range
	_bool			m_bSearchEye = false;

private:
	wstring			m_ModelName;
	_float			m_fTimeAcc = 0.f;		// Idle 상태를 유지하기 위한 시간
	_float			m_fMovingTime = 0.f;	// 시간이 조건에 찰때까지 움직여라

	// 위치 저장용
	_float3			m_fMyPos = { 0.f, 0.f, 0.f };
	_float3			m_fStorePos = { 0.f, 0.f, 0.f };

	_vector			m_vMyPos;
	_vector			m_vStorePos;
	_vector			m_vDest;
	_bool			m_bDirCheck = false;
	// ~위치 저장용
	// 현재 플레이중인 Animation의 Ratio를 AnimState에 넘겨주기 위한 용도
	_float	m_fPlayRatio = 0.f;

	_uint	m_iAfterIdlePt = 0;

	DAMAGE_PARAM	m_StrDamage;

private:
	CGameObject*	m_pFlowerLeg = nullptr;
	CGameObject*	m_pStorePt = nullptr;

public:
	static CSkummyPandou* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END