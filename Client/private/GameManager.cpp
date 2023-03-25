#include "stdafx.h"
#include "..\public\GameManager.h"

CGameManager* CGameManager::s_GameManager = nullptr;

CGameManager::CGameManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CGameManager::SetGameManager(CGameManager* pGameManager)

{
	Safe_Release(s_GameManager);
	s_GameManager = pGameManager;
	Safe_AddRef(s_GameManager);
}

_uint CGameManager::DestroyInstance()
{
	unsigned long	dwRefCnt = 0;		
	if(nullptr != s_GameManager)	
	{		
		dwRefCnt = s_GameManager->Release();
		if(0 == dwRefCnt)				
			s_GameManager = nullptr;		
	}									
	return dwRefCnt;	
}

HRESULT CGameManager::Initialize()
{
	return S_OK;
}

void CGameManager::Tick(_double TimeDelta)
{
}

void CGameManager::ConsumeEnemyDamageReport(ENEMY_DAMAGE_REPORT tReport)
{
}

void CGameManager::ConsumePlayerDamageReport(PLAYER_DAMAGE_REPORT tReport)
{
}

CGameManager* CGameManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameManager* pInstance = new CGameManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CGameManager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGameManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
