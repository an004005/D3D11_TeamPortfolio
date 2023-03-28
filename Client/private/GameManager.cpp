
#include "stdafx.h"
#include "..\public\GameManager.h"
#include "JsonStorage.h"
#include "GameInstance.h"

#include "Canvas_Acquisition.h"
#include "Canvas_LeftTalk.h"

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
	// Safe_AddRef(s_GameManager);
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
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Acquisition.json");
	pCanvas_Acquisition = dynamic_cast<CCanvas_Acquisition*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Acquisition", &json));
	assert(pCanvas_Acquisition != nullptr && "Failed to Clone : Canvas_Acquisition");

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_LeftTalk.json");
	pCanvas_LeftTalk = dynamic_cast<CCanvas_LeftTalk*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_LeftTalk", &json));
	assert(pCanvas_LeftTalk != nullptr && "Failed to Clone : Canvas_LeftTalk");

	return S_OK;
}

void CGameManager::Tick(_double TimeDelta)
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	{
		pCanvas_Acquisition->Set_EnemyUI(EEnemyName::EM0400, 5);
		pCanvas_LeftTalk->Add_Talk(0);
		pCanvas_LeftTalk->Add_Talk(1);
		pCanvas_LeftTalk->Add_Talk(2);
	}
}

void CGameManager::ConsumeEnemyDamageReport(ENEMY_DAMAGE_REPORT tReport)
{
	if (tReport.bDead)
	{
		pCanvas_Acquisition->Set_EnemyUI(tReport.eName, tReport.eStat.iLevel);
	}

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
