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

CLevel_NaomiRoom::CLevel_NaomiRoom(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_NaomiRoom::Initialize()
{
	 //m_bPlayerSpawn = false;

	m_strLevelName = L"NaomiRoom";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/NaomiRoom_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_NaomiRoom.json";

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
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Achieve_Batch.json");

	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Kinetic_Normal.json");
	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_NaomiRoom::Tick(_double TimeDelta)
{
	CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);
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
