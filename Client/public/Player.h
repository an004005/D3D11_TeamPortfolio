#pragma once
#include "ImguiObject.h"
#include "GameObject.h"

BEGIN(Engine)
class CAnimation;
class CFSMComponent;
END

BEGIN(Client);

class CPlayer : public CGameObject
{
private:
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

private:
	CFSMComponent*	m_pFSM = nullptr;

};

END