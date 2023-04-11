#include "stdafx.h"
#include "..\public\Level_FinalStage.h"

#include <Imgui_AnimModifier.h>

#include "GameInstance.h"
#include "GameManager_Tutorial.h"
#include "Imgui_Batch.h"

#include "RedString.h"
#include "GameUtils.h"
#include "Imgui_EffectBrowser.h"
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

	CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_FinalStage"), TEXT("TestTarget"))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(0.f, 2.f, 0.f, 1.f));

	CGameInstance::GetInstance()->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));
	//CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/FinalStage/Kinetic_Normal.json");

	CGameInstance::GetInstance()->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));

	// Json PreviewData;
	// {
	// 	PreviewData["Model"] = "Model_Player";
	// 	PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND_TOON;
	// 	CGameInstance::GetInstance()->Clone_GameObject(L"Preview", TEXT("ModelPreview"), &PreviewData);
	// 	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/FinalStage/Kinetic_Normal.json");
	// 	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);
	// }

	Json PreviewData;
	{
		PreviewData["Model"] = "Prototype_Model_em8200";
		PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND_TOON;
		auto pBoss = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_FinalStage"), TEXT("ModelPreview"), &PreviewData);
	}

	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_EffectBrowser::Create(m_pDevice, m_pContext));

	//CGameUtils::ListFiles("../Bin/Resources/Restrings/BranFieldStrings/", [](const string& filePath)
	//{
	//	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(filePath);
	//	CGameInstance::GetInstance()->Clone_GameObject(LAYER_MAP_DECO, L"Prototype_CombinedRedString", &json);
	//});

	// Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_BrainField.json");
	// FAILED_CHECK(CGameInstance::GetInstance()->Clone_GameObject(PLAYERTEST_LAYER_MAP, TEXT("Prototype_GameObject_ScarletMap"), &json));

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLevel_FinalStage::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(-cosf(XMConvertToRadians(60.f)), -sinf(XMConvertToRadians(60.f)), 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_FinalStage::Tick(_double TimeDelta)
{
	// CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);
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
