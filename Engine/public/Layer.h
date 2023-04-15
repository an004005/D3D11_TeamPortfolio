#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);
	void AfterPhysX();
	
	list<class CGameObject*>& GetGameObjects() { return m_GameObjects; }
	void SetUseTimeRatio(_bool bUse) { m_bUseTimeRatio = bUse; }
	void SetLayerTimeRatio(_float fLayerTimeRatio) { m_fLayerTimeRatio = fLayerTimeRatio; }

	void SetActive(_bool bActive) { m_bActive = bActive;}
	_bool IsActive() const { return m_bActive; }

	CGameObject* Pop_InLayer(CGameObject* pGameObject);

private:
	list<class CGameObject*>			m_GameObjects;
	typedef list<class CGameObject*>	GAMEOBJECTS;
	_bool m_bUseTimeRatio = true;
	_float m_fLayerTimeRatio = 1.f;

	_bool m_bActive = true;

public:
	static CLayer* Create();
	virtual void Free() override;

};

END