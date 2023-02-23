#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
class CShader;
class CAnimationInstance;
class CAnimation;
class CFSMComponent;
class CModel;
class CRenderer;
END

BEGIN(Client)
class CBdLm_AnimInstance;

class CBuddyLumi : public CGameObject
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

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;

	CBdLm_AnimInstance*		m_pASM = nullptr;

private:
	HRESULT				Setup_AnimSocket();
	list<CAnimation*>	m_GroundDmgSocket;
	list<CAnimation*>	m_AirDmgSocket;
	list<CAnimation*>	m_DeadAnimSocket;
private:
	HRESULT SetUp_Components(void* pArg);

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

private:
	wstring			m_ModelName;

	_float			m_fTimeAcc = 0.f;
	_float3			m_fMyPos = { 0.f, 0.f, 0.f };

	_uint			m_iAfterAtkPt = 0;

private:
	_bool			m_bRtoA = false; // Run이 끝나고 Attack로 가기 위한 bool 변수


public:
	static CBuddyLumi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END