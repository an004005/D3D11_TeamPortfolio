#include "stdafx.h"
#include "..\public\GameManager.h"
#include "JsonStorage.h"
#include "GameInstance.h"	

#include "Canvas_Acquisition.h"
#include "Canvas_LeftTalk.h"
#include "Canvas_Quest.h"
#include "DistanceUI.h"
#include "Canvas_Alarm.h"

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
	if (nullptr != s_GameManager)
	{
		dwRefCnt = s_GameManager->Release();
		if (0 == dwRefCnt)
			s_GameManager = nullptr;
	}
	return dwRefCnt;
}

HRESULT CGameManager::Initialize()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 오른쪽에 몬스터 처치 및 아이템 획득시에 뜨는 UI
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Acquisition.json");
	m_pCanvas_Acquisition = dynamic_cast<CCanvas_Acquisition*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Acquisition", &json));
	assert(m_pCanvas_Acquisition != nullptr && "Failed to Clone : Canvas_Acquisition");

	// 왼쪽에 조잘조잘때 뜨는 UI
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_LeftTalk.json");
	m_pCanvas_LeftTalk = dynamic_cast<CCanvas_LeftTalk*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_LeftTalk", &json));
	assert(m_pCanvas_LeftTalk != nullptr && "Failed to Clone : Canvas_LeftTalk");

	if (LEVEL_NOW == LEVEL_DOWNTOWN_1)
	{
		// 맵 이름
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
		CCanvas_Alarm* pCanvas_Alarm = dynamic_cast<CCanvas_Alarm*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_Alarm", &json));
		pCanvas_Alarm->Set_MapName(0.0f);
	}
	else if (LEVEL_NOW == LEVEL_CONSTRUCTIONSITE_3F)
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Quest.json");
		CCanvas_Quest* pCanvas_Quest = dynamic_cast<CCanvas_Quest*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Quest", &json));
		assert(pCanvas_Quest != nullptr && "Failed to Clone : CCanvas_Quest");
		pCanvas_Quest->Add_Quest(4, false);

		// 맵 이름
		json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
		CCanvas_Alarm* pCanvas_Alarm = dynamic_cast<CCanvas_Alarm*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_Alarm", &json));
		pCanvas_Alarm->Set_MapName(3.0f);
	}
	else if (LEVEL_NOW == LEVEL_CONSTRUCTIONSITE_2F)
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Quest.json");
		CCanvas_Quest* pCanvas_Quest = dynamic_cast<CCanvas_Quest*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Quest", &json));
		assert(pCanvas_Quest != nullptr && "Failed to Clone : CCanvas_Quest");
		pCanvas_Quest->Add_Quest(2, false);
	}
	else if (LEVEL_NOW == LEVEL_SUBWAY)
	{
		// 맵 이름
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
		CCanvas_Alarm* pCanvas_Alarm = dynamic_cast<CCanvas_Alarm*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_Alarm", &json));
		pCanvas_Alarm->Set_MapName(1.0f);
	}
	else if (LEVEL_NOW == LEVEL_HOSPITAL_1F)
	{
		// 맵 이름
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
		CCanvas_Alarm* pCanvas_Alarm = dynamic_cast<CCanvas_Alarm*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_Alarm", &json));
		pCanvas_Alarm->Set_MapName(2.0f);
	}

	return S_OK;
}

void CGameManager::Tick(_double TimeDelta)
{


}

void CGameManager::ConsumeEnemyDamageReport(ENEMY_DAMAGE_REPORT tReport)
{
	if (tReport.bDead)
	{
		m_pCanvas_Acquisition->Set_EnemyUI(tReport.eName, tReport.eStat.iLevel);

		if (false == m_bEM1200Dead)
		{
			if (tReport.eName == EEnemyName::EM1100)
			{
				// 병원에서 물보스를 죽였을 때
				m_bEM1200Dead = true;

				CGameManager::GetInstance()->Set_LeftTalk(32);
				CGameManager::GetInstance()->Set_LeftTalk(33);
				CGameManager::GetInstance()->Set_LeftTalk(34);
				CGameManager::GetInstance()->Set_LeftTalk(35);
			}
		}

		if (false == m_bEM0110Dead)
		{
			if (tReport.eName == EEnemyName::EM0110)
			{
				// 말잡고 나서
				m_bEM0110Dead = true;

				CGameManager::GetInstance()->Set_LeftTalk(107);
				CGameManager::GetInstance()->Set_LeftTalk(108);
			}
		}
	}

	m_EneymyReports.push_back(tReport);

	// 몬스터 상태이상
	if (EDeBuffType::DEBUFF_FIRE == tReport.eBeDeBuff) Set_LeftTalk(98);
	if (EDeBuffType::DEBUFF_OIL == tReport.eBeDeBuff) Set_LeftTalk(99);
	if (EDeBuffType::DEBUFF_THUNDER == tReport.eBeDeBuff) Set_LeftTalk(100);
	if (EDeBuffType::DEBUFF_WATER == tReport.eBeDeBuff) Set_LeftTalk(101);




}

void CGameManager::ConsumePlayerDamageReport(PLAYER_DAMAGE_REPORT tReport)
{
	// 플레이어 상태이상
	if (EDeBuffType::DEBUFF_FIRE == tReport.eBeDeBuff) Set_LeftTalk(94);
	if (EDeBuffType::DEBUFF_OIL == tReport.eBeDeBuff) Set_LeftTalk(95);
	if (EDeBuffType::DEBUFF_THUNDER == tReport.eBeDeBuff) Set_LeftTalk(96);
	if (EDeBuffType::DEBUFF_WATER == tReport.eBeDeBuff) Set_LeftTalk(97);




}

void CGameManager::Set_FullItem(const wstring szItemName)
{
	m_pCanvas_Acquisition->Set_FullItem(szItemName);
}

void CGameManager::Set_AddlItem(const wstring szItemName)
{
	m_pCanvas_Acquisition->Set_AddItem(szItemName);
}

void CGameManager::Set_LeftTalk(const _int iIndex, const _int iQuest)
{
	m_pCanvas_LeftTalk->Add_Talk(iIndex, iQuest);
}

void CGameManager::Set_SuccessQuest(const _uint iCoin)
{
	m_pCanvas_Acquisition->Set_SuccessQuest(iCoin);
}

//void CGameManager::Quest_Tick()
//{
//	// Test 용 각 레벨에서 할 예정
//	if (true == m_bQuest)
//	{
//		m_bQuest = false;
//
//		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Quest.json");
//		json["QuestIndex"] = 0;
//		m_pCanvas_Quest = dynamic_cast<CCanvas_Quest*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Quest", &json));
//		assert(m_pCanvas_Quest != nullptr && "Failed to Clone : CCanvas_Quest");
//
//		if (LEVEL_UI == LEVEL_NOW) return;
//
//		json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/DistanceUI.json");
//		json["ArrivalPoint"] = { 0.0f, 0.0f, 0.0f, 0.0f };
//		m_pDistanceUI = dynamic_cast<CDistanceUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"DistanceUI", &json));
//		assert(m_pDistanceUI != nullptr && "Failed to Clone : DistanceUI");
//		
//	}
//
//	if (true == m_bSuccessQuest)
//	{
//		m_bSuccessQuest = false;
//		m_pCanvas_Quest->Set_SuccessQuest();
//	}
//}

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
