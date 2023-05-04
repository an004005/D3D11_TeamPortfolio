#include "stdafx.h"
#include "Level_DownTown.h"

#include "GameManager.h"
#include "Imgui_Batch.h"

#include "PlayerInfoManager.h"
#include "Item_Manager.h"

/*********************
 * CLevel_DownTown
 **********************/
CLevel_DownTown::CLevel_DownTown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_DownTown::Initialize()
{
	 // m_bPlayerSpawn = false;

	m_strLevelName = L"DownTown1";

	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/DownTown_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_DownTown.json";

	m_BGM.CloneSound(m_MainSound); // 기본 bgm

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/DownTown1.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Npc/DownTown1_Npc.json");

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/DownTown1/PlayerSpawnPosition.json");

	//AI
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/DownTown1/AI.json");

	CGameInstance::GetInstance()->LoadFogJson("../Bin/Resources/Batch/Tutorial_fog.json");
	return S_OK;
}

void CLevel_DownTown::Tick(_double TimeDelta)
{
	// if (m_BGMOnce.IsNotDo())
	// 	m_BGM.PlaySound(m_MainSound);

	CLevel_StageDefault::Tick(TimeDelta);
}

CLevel_DownTown* CLevel_DownTown::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_DownTown*		pInstance = new CLevel_DownTown(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_DownTown");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_DownTown::Free()
{
	CLevel_StageDefault::Free();
}



/*********************
 * CLevel_DownTown_Second
 **********************/
CLevel_DownTown_Second::CLevel_DownTown_Second(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_DownTown_Second::Initialize()
{
	 // m_bPlayerSpawn = false;

	m_strLevelName = L"DownTown2";

	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/DownTown_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_DownTown.json";

	m_BGM.CloneSound(m_MainSound); // 기본 bgm



	if (FAILED(__super::Initialize()))
		return E_FAIL;
	//if (FAILED(Ready_Layer_AI(LAYER_AI)))
	//	return E_FAIL;

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/DownTown2.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Npc/DownTown1_Npc.json");

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/DownTown2/PlayerSpawnPosition.json");

	//AI
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/DownTown2/AI.json");
	CGameInstance::GetInstance()->LoadFogJson("../Bin/Resources/Batch/Tutorial_fog.json");
	return S_OK;
}

void CLevel_DownTown_Second::Tick(_double TimeDelta)
{
	// if (m_BGMOnce.IsNotDo())
	// 	m_BGM.PlaySound(m_MainSound);

	CLevel_StageDefault::Tick(TimeDelta);
}

CLevel_DownTown_Second* CLevel_DownTown_Second::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_DownTown_Second*		pInstance = new CLevel_DownTown_Second(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_DownTown_Second");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_DownTown_Second::Free()
{
	CLevel_StageDefault::Free();
}


/*********************
 * CLevel_DownTown_Third
 **********************/
CLevel_DownTown_Third::CLevel_DownTown_Third(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_DownTown_Third::Initialize()
{
	 // m_bPlayerSpawn = false;

	/*CPlayerInfoManager::GetInstance()->Load("../Bin/Resources/SaveData/PlayerInfo.json");
	CItem_Manager::GetInstance()->Load("../Bin/Resources/SaveData/Item.json");*/

	m_strLevelName = L"DownTown3";

	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/DownTown_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_DownTown.json";

	m_BGM.CloneSound(m_MainSound); // 기본 bgm

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/DownTown3.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Npc/DownTown1_Npc.json");
	
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/DownTown3/PlayerSpawnPosition.json");

	//AI
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/DownTown3/AI.json");
	CGameInstance::GetInstance()->LoadFogJson("../Bin/Resources/Batch/Tutorial_fog.json");
	return S_OK;
}

void CLevel_DownTown_Third::Tick(_double TimeDelta)
{
	// if (m_BGMOnce.IsNotDo())
	// 	m_BGM.PlaySound(m_MainSound);

	CLevel_StageDefault::Tick(TimeDelta);
}

CLevel_DownTown_Third* CLevel_DownTown_Third::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_DownTown_Third*		pInstance = new CLevel_DownTown_Third(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_DownTown_Third");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_DownTown_Third::Free()
{
	CLevel_StageDefault::Free();
}