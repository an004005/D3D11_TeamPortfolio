#include "stdafx.h"
#include "..\public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	for (auto itr = m_GameObjects.begin(); itr != m_GameObjects.end();)
	{
		const auto pGameObject = *itr;

		if (pGameObject->IsDeleted())
		{
			pGameObject->Release();
			itr = m_GameObjects.erase(itr);
		}
		else 
			++itr;
	}

	for (const auto pGameObject : m_GameObjects)
		pGameObject->Tick(TimeDelta);
}

void CLayer::Late_Tick(_double TimeDelta)
{
	for (const auto pGameObject : m_GameObjects)
		pGameObject->Late_Tick(TimeDelta);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
