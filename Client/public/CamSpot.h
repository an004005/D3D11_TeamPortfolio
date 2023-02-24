#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CCamera_Player;
class CController;

class CCamSpot : public CGameObject
{
private:
	CCamSpot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamSpot(const CCamSpot& rhs);
	virtual ~CCamSpot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);

private:
	void	MouseMove(_double TimeDelta);
	_float	m_fSensitivity = 0.2f;
	_float	m_fCamHeight = 0.f;

public:
	CTransform*	GetTransform() { return this->m_pTransformCom; }
	
private:
	CGameObject* m_pTargetObject = nullptr;
	CGameObject* m_pPlayerCamera = nullptr;

private:
	HRESULT	Setup_Components(void);
	CController* m_pController = nullptr;

public:
	static CCamSpot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END