#include "stdafx.h"
#include "..\public\GameManager.h"
#include "JsonStorage.h"
#include "GameInstance.h"

#include "Canvas_Acquisition.h"
#include "Canvas_LeftTalk.h"
#include "Canvas_Quest.h"
#include "DistanceUI.h"

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

	// 오른쪽에 몬스터 처치 및 아이템 획득시에 뜨는 UI
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Acquisition.json");
	m_pCanvas_Acquisition = dynamic_cast<CCanvas_Acquisition*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Acquisition", &json));
	assert(m_pCanvas_Acquisition != nullptr && "Failed to Clone : Canvas_Acquisition");

	// 왼쪽에 조잘조잘때 뜨는 UI
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_LeftTalk.json");
	m_pCanvas_LeftTalk = dynamic_cast<CCanvas_LeftTalk*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_LeftTalk", &json));
	assert(m_pCanvas_LeftTalk != nullptr && "Failed to Clone : Canvas_LeftTalk");

	return S_OK;
}

void CGameManager::Tick(_double TimeDelta)
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_0) && LEVEL_NOW == LEVEL_UI)
	{
		m_pCanvas_Acquisition->Set_EnemyUI(EEnemyName::EM0400, 5); 
		m_pCanvas_LeftTalk->Add_Talk(0);
		//m_pCanvas_LeftTalk->Add_Talk(1);
		//m_pCanvas_LeftTalk->Add_Talk(2);
		m_bQuest = true;
	}

	//if (CGameInstance::GetInstance()->KeyDown(DIK_9) && LEVEL_NOW == LEVEL_UI)
	//{
	//	m_bSuccessQuest = true; // 0번 먼저 누르고 9번 누르기

	//}

	Quest_Tick();
}

void CGameManager::ConsumeEnemyDamageReport(ENEMY_DAMAGE_REPORT tReport)
{
	if (tReport.bDead)
	{
		m_pCanvas_Acquisition->Set_EnemyUI(tReport.eName, tReport.eStat.iLevel);
	}

	m_EneymyReports.push_back(tReport);
}

void CGameManager::ConsumePlayerDamageReport(PLAYER_DAMAGE_REPORT tReport)
{

}

void CGameManager::FullItem(const wstring szItemName)
{
	m_pCanvas_Acquisition->Set_FullItem(szItemName);
}

void CGameManager::Quest_Tick()
{
	// Test 용 각 레벨에서 할 예정
	if (true == m_bQuest)
	{
		m_bQuest = false;

		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Quest.json");
		json["QuestIndex"] = 0;
		m_pCanvas_Quest = dynamic_cast<CCanvas_Quest*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Quest", &json));
		assert(m_pCanvas_Quest != nullptr && "Failed to Clone : CCanvas_Quest");
		//m_pCanvas_Quest->Set_Quest(0);

		if (LEVEL_UI == LEVEL_NOW) return;

		json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/DistanceUI.json");
		json["ArrivalPoint"] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pDistanceUI = dynamic_cast<CDistanceUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"DistanceUI", &json));
		assert(m_pDistanceUI != nullptr && "Failed to Clone : DistanceUI");
		
	}

	if (true == m_bSuccessQuest)
	{
		m_bSuccessQuest = false;
		m_pCanvas_Quest->Set_SuccessQuest();
	}
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

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCanvas_Acquisition))
		m_pCanvas_Acquisition->SetDelete();
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCanvas_LeftTalk))
		m_pCanvas_LeftTalk->SetDelete();

}
