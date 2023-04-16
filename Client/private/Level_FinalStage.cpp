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
#include "InvisibleWall.h"
#include "Imgui_CameraManager.h"
#include "Imgui_CamAnimEditor.h"

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

	m_BGM.CloneSound(m_MainSound); // 기본 bgm

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
	 	return E_FAIL;


	// _matrix matTarget = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 0.f, 0.f);
	// CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_FinalStage"), TEXT("TestTarget"))
	// 	->GetTransform()->Set_WorldMatrix(matTarget);

	// Set_State(CTransform::STATE_TRANSLATION, _float4(0.f, 0.f, 0.f, 1.f));

	CGameInstance::GetInstance()->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));
	//CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/FinalStage/Kinetic_Normal.json");

	// CGameInstance::GetInstance()->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));

	// Json PreviewData;
	// {
	// 	PreviewData["Model"] = "Model_Player";
	// 	PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND_TOON;
	// 	CGameInstance::GetInstance()->Clone_GameObject(L"Preview", TEXT("ModelPreview"), &PreviewData);
	// 	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/FinalStage/Kinetic_Normal.json");
	// 	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);
	// }

	// Json PreviewData;
	// {
	// 	PreviewData["Model"] = "Prototype_Model_em8200";
	// 	PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND_TOON;
	// 	auto pBoss = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_FinalStage"), TEXT("ModelPreview"), &PreviewData);
	// }

	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_EffectBrowser::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CameraManager::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CamAnimEditor::Create(m_pDevice, m_pContext));

	//CGameUtils::ListFiles("../Bin/Resources/Restrings/BranFieldStrings/", [](const string& filePath)
	//{
	//	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(filePath);
	//	CGameInstance::GetInstance()->Clone_GameObject(LAYER_MAP_DECO, L"Prototype_CombinedRedString", &json);
	//});

	/////////////////////////////////
	// 투명 벽 생성 코드 
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/FinalStage/InvisibleWall_NotVisible.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/FinalStage/WaterFall.json");

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/FinalStage/PlayerStartPos.json");

	for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapDecorate")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == L"Prototype_InvisibleWall")
		{
			CInvisibleWall* pInvisibleWall = dynamic_cast<CInvisibleWall*>(iter);
			//Safe_AddRef(pInvisibleWall);
			pInvisibleWall->Activate(true);
			//Safe_Release(pInvisibleWall);
		}		
	}
	/////////////////////////////////

	// 염력 물체 프레셋 (재사용 되어지는)
	// Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_BrainField.json");
	// FAILED_CHECK(CGameInstance::GetInstance()->Clone_GameObject(PLAYERTEST_LAYER_MAP, TEXT("Prototype_GameObject_ScarletMap"), &json));

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_MONSTER, L"Monster_em8200")->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 20.f, 1.f));

	CGameInstance::GetInstance()->LoadFogJson("../Bin/Resources/Batch/Final_fog.json");

	// CGameInstance::GetInstance()->LoadFogJson("");
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

	return S_OK;
}

void CLevel_FinalStage::Tick(_double TimeDelta)
{

	if (m_BGMOnce.IsNotDo())
		m_BGM.PlaySound(m_MainSound);

	//CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);
	__super::Tick(TimeDelta);
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
