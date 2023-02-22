#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
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

BEGIN(Client);
class CBaseAnimInstance;

class CPlayer : public CGameObject
{
protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick() {}
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	HRESULT SetUp_Components(void* pArg);

	CFSMComponent*		m_pFSM = nullptr;
	CBaseAnimInstance*	m_pASM = nullptr;

	CRenderer*			m_pRenderer = nullptr;
	CModel*				m_pModel = nullptr;

protected:
	_bool			m_bAir = false;
	_bool			m_PreAir = false;
	_bool			m_bMove = false;
	_bool			m_bWalk = false;

protected:
	wstring			m_ModelName;

public:
	_bool isAir() { return m_bAir; }
	_bool isMove() { return m_bMove; }
	_bool isWalk() { return m_bWalk; }

public:
	static CPlayer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END