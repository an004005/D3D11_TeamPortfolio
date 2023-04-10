#include "stdafx.h"
#include "..\public\Level_FinalStage.h"
#include "GameInstance.h"
#include "GameManager_Tutorial.h"
#include "Imgui_Batch.h"

#include "RedString.h"
#include "GameUtils.h"
#include "JsonStorage.h"
#include "Imgui_AnimModifier.h"
#include "PhysX_Manager.h"
#include "Map_KineticBatchPreset.h"
#include "GameManager.h"

CLevel_FinalStage::CLevel_FinalStage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_FinalStage::Initialize()
{
	// m_bPlayerSpawn = false;

	m_strLevelName = L"FinalStage";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/FinalStage_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_FinalBossStage.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	// if (FAILED(Ready_Layer_AI(LAYER_AI)))
		// return E_FAIL;

	//CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/FinalStage/Kinetic_Normal.json");

	CGameInstance::GetInstance()->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));
	Json PreviewData;
	{
		PreviewData["Model"] = "Model_Player";
		PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND_TOON;
		CGameInstance::GetInstance()->Clone_GameObject(L"Preview", TEXT("ModelPreview"), &PreviewData);
	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/FinalStage/Kinetic_Normal.json");
	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);
		
	//CGameUtils::ListFiles("../Bin/Resources/Restrings/BranFieldStrings/", [](const string& filePath)
	//{
	//	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(filePath);
	//	CGameInstance::GetInstance()->Clone_GameObject(LAYER_MAP_DECO, L"Prototype_CombinedRedString", &json);
	//});

	}

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_FinalStage::Tick(_double TimeDelta)
{
	CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);
}	

CLevel_FinalStage * CLevel_FinalStage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_FinalStage*		pInstance = new CLevel_FinalStage(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_FinalStage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_FinalStage::Free()
{
	__super::Free();	
}
