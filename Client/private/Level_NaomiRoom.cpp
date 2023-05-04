#include "stdafx.h"
#include "..\public\Level_NaomiRoom.h"
#include "GameInstance.h"
#include "GameManager_Tutorial.h"
#include "Imgui_Batch.h"
#include "GameManager.h"

// PJW Add
#include "Map_KineticBatchPreset.h"
#include "PhysX_Manager.h"
#include "GameUtils.h"
#include "JsonStorage.h"

#include "EM1200.h"
#include "BronJon.h"
#include "PlayerInfoManager.h"
#include "Item_Manager.h"

CLevel_NaomiRoom::CLevel_NaomiRoom(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_NaomiRoom::Initialize()
{
	 //m_bPlayerSpawn = false;


	CPlayerInfoManager::GetInstance()->Load("../Bin/Resources/SaveData/PlayerInfo.json");
	CItem_Manager::GetInstance()->Load("../Bin/Resources/SaveData/Item.json");

	m_strLevelName = L"NaomiRoom";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/NaomiRoom_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_NaomiRoom.json";
	m_strColorGradingPath = "../Bin/Resources/VFX/PostVFX/ColorGrading_Naomi.json";

	m_BGM.CloneSound(m_MainSound); // ±âº» bgm

	//Boss
	m_BGM.CloneSound("em1200BGM");

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Naomi_SpeicialContainer.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Player_AI_Pos.json");	
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Trigger_Naomi_Invisible.json");

	// Red String Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Red_Test1.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/RedString_1.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/RedString_2.json");

	// AchieveItem Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Alarm/NaomiRoom.json");

	// UI
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Achieve_Batch.json");
	
	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Kinetic_Normal.json");
	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_NaomiRoom::Tick(_double TimeDelta)
{
	// if (m_BGMOnce.IsNotDo())
	// 	m_BGM.PlaySound(m_MainSound);
	//
	//
	// if (FindGameObjectInLayer<CEM1200>(L"Layer_Monster"))
	// {
	// 	if (m_bBossBGM == false)
	// 	{
	// 		m_BGM.StopAllLoop();
	// 		m_bBossBGM = true;
	// 		m_BGM.PlaySound("em1200BGM");
	// 	}
	// }
	// else
	// {
	// 	if (m_bBossBGM == true)
	// 	{
	// 		m_BGM.StopAllLoop();
	// 		m_bBossBGM = false;
	// 		m_BGM.PlaySound(m_MainSound);
	// 	}
	// }


	CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);
	CLevel_StageDefault::Tick(TimeDelta);
}

CLevel_NaomiRoom * CLevel_NaomiRoom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_NaomiRoom*		pInstance = new CLevel_NaomiRoom(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_NaomiRoom");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_NaomiRoom::Free()
{
	__super::Free();
}
