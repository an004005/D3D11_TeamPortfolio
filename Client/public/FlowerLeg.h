#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
class CRigidBody;
class CGameInstance;
END

BEGIN(Client)
class CFL_AnimInstance;

class CFlowerLeg : public CMonster
{
public:
	enum KEYSTATE { KEY_W, KEY_S, KEY_A, KEY_D, KEY_END };
	enum MOUSESTATE { MS_LB, MS_RB, MS_END };

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

private:
	void Key_Input(_double TimeDelta);
	void JumpToGround(_double TimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;

	CFL_AnimInstance*		m_pASM = nullptr;
	CRigidBody*				m_pTrigger = nullptr;

private:
	HRESULT				Setup_AnimSocket();

private:
	_vector			m_vCurrentLook = { 0.f, 0.f, 0.f, 0.f };
	// Jump
	_float3			m_fMyPos = { 0.f, 0.f, 0.f };
	
	_float			m_fJumpSpeed = 13.3f;
	_float			m_fJumpTime = 0.f;
	_float			m_fGravity = 7.7f;
	// ~Jump

	// Jump Speed Tool
	_float		m_fWeight = 0.24f;
	_float		m_fOverLap = 0.28f;


	// 스테이트 상태
	_bool m_bIdle = false;
	_bool m_bWalk = false;
	_bool m_bAir = false;
	_bool m_bLanding = false;
	
	// Attack L + R
	_bool m_bAttackL = false;	
	_bool m_bAttackR = false;

	// ~Attack L + R

	// imgui Test
	_uint m_iTestHp = 1000;

public:
	_bool IsIdle() const { return m_bIdle; }
	_bool IsWalk() const { return m_bWalk; }
	_bool IsAir() const { return m_bAir; }
	_bool IsLanding() const { return m_bLanding; }
	
	_bool IsAttackL() const { return m_bAttackL; }
	_bool IsAttackR() const { return m_bAttackR; }

public:
	KEYSTATE	const	Get_eDikKey() { return m_eDikKey; }
	MOUSESTATE  const	Get_eDimMouse() { return m_eDimMouse; }

private:
	KEYSTATE		m_eDikKey = KEY_END;
	MOUSESTATE		m_eDimMouse = MS_END;

private:
	wstring					m_ModelName;

private:
	HRESULT SetUp_Components(void* pArg);

public:
	static CFlowerLeg* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END