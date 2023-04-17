#include "stdafx.h"
#include "Level_StageDefault.h"

#include <Camera.h>
#include <Imgui_AnimModifier.h>
#include <Imgui_CameraManager.h>
#include <Imgui_PostProcess.h>
#include "GameInstance.h"
#include "JsonStorage.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "FactoryMethod.h"
#include "Imgui_Batch.h"
#include "Imgui_CurveManager.h"
#include "Imgui_PhysX.h"
#include "SkyBox.h"
#include "UI_Manager.h"
#include "GameManager.h"
#include "Imgui_Cheat.h"
#include "Imgui_LightManager.h"
#include "PlayerInfoManager.h"
#include "ScarletMap.h"

CLevel_StageDefault::CLevel_StageDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_Tutorial.json";
}

HRESULT CLevel_StageDefault::Initialize()
{
	FAILED_CHECK(CLevel::Initialize());

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PhysX::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CameraManager::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CurveManager::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_Batch::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_Cheat::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LightManager::Create(m_pDevice, m_pContext));

	CUI_Manager::GetInstance()->Clear();
	CGameInstance::GetInstance()->Add_EmptyLayer(LEVEL_NOW, L"Layer_MapKineticObject");

	CGameInstance::GetInstance()->ClearPx();

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(PLAYERTEST_LAYER_CAMERA)))
		return E_FAIL;
	if (m_bPlayerSpawn)
	{
		if (FAILED(Ready_Layer_Player(PLATERTEST_LAYER_PLAYER)))
			return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI(PLAYERTEST_LAYER_FRONTUI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(PLAYERTEST_LAYER_MAP)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(PLAYERTEST_LAYER_POSTVFX)))
		return E_FAIL;

	if (FAILED(Ready_Layer_SASPortrait(LAYER_SAS)))
		return E_FAIL;

	if (m_bPlayerSpawn)
		CGameInstance::GetInstance()->FindCamera("PlayerCamera")->SetMainCamera();

	m_SetMainCamOnce.Reset();

	return S_OK;
}

void CLevel_StageDefault::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);

	if (auto pLoadingCam = CGameInstance::GetInstance()->FindCamera("LoadingCam"))
	{
		if (pLoadingCam == CGameInstance::GetInstance()->GetMainCam())
		{
			if (m_bPlayerSpawn)
				CGameInstance::GetInstance()->FindCamera("PlayerCamera")->SetMainCamera();
		}
	}

	if (CGameInstance::GetInstance()->KeyDown(DIK_F1))
	{
		CPlayerInfoManager::GetInstance()->Set_Exp(85);
	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_F2))
	{
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin += 25;
	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_F3))
	{
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP += 25;
	}
}

HRESULT CLevel_StageDefault::Render()
{
	const wstring strLevel = L"Level : " + m_strLevelName;
	SetWindowText(g_hWnd, strLevel.c_str());
	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (m_bPlayerSpawn)
	{
		FAILED_CHECK(CFactoryMethod::MakePlayerPrototypes(m_pDevice, m_pContext));
	}
	FAILED_CHECK(CFactoryMethod::MakeMonsterExPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeUIPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeSAS_Portrait_Prototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeKineticPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeTriggerPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeAIPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeNPCPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeTWallPrototype(m_pDevice, m_pContext));

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Prototype_GameObject_SkyBox") == nullptr)
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Lights()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(-cosf(XMConvertToRadians(60.f)), -sinf(XMConvertToRadians(60.f)), 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	if (m_strShadowCamJsonPath.empty() == false)
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson(m_strShadowCamJsonPath);
		CGameInstance::GetInstance()->Add_Camera("ShadowCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic", &json);
		CGameInstance::GetInstance()->SetShadowCam(CGameInstance::GetInstance()->FindCamera("ShadowCamera"));
	}

	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json PreviewData;
	PreviewData["Model"] = "Model_Player";
	PreviewData["KineticModel"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));

	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Frount_UI Canvas_FrontUI 
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Item.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Item", &json));
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ItemMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_ItemMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfoLeft.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfoLeft", &json));
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfoLeftMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfoLeftMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_PlayerInfo.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_PlayerInfo", &json));
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_PlayerInfoMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_PlayerInfoMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Drive.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Drive", &json));
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_DriveMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_DriveMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfoRight.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfoRight", &json));
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfoRightMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfoRightMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASSkill.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASSkill", &json));		// 움직이지 않은 UI
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASSkillMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASSkillMove", &json));	// 움직이는 UI

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Main.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Main", &json));
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Shop.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Shop", &json));
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_WorldMap.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_WorldMap", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MouseCousor.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_MouseCousor", &json));
	
	// UI
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/HPlossUI.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"HPlossUI", &json));
	
	// Effect
	CGameInstance::GetInstance()->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_GravikenisisMouseUI"));
	CGameInstance::GetInstance()->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_GravikenisisGUI"));
	
	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Layer_Map(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	auto pSkyBox = dynamic_cast<CSkyBox*>(pGameInstance->Clone_GameObject_Get(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_SkyBox")));
	pSkyBox->GetParams().iPass = 0;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(m_strMapJsonPath);

	CPlayerInfoManager::GetInstance()->m_pDefaultMap = dynamic_cast<CScarletMap*>(pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));

	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Layer_Effect(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json ColorGrading = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/ColorGrading.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"Prototype_PostVFX_ColorGrading", &ColorGrading);

	Json Scifi = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Scifi", &Scifi);

	Json Distortion = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Distortion/Distortion_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Distortion", &Distortion);

	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Layer_SASPortrait(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Clone_GameObject(pLayerTag, L"Prototype_SASPortrait");

	return S_OK;
}

HRESULT CLevel_StageDefault::Ready_Layer_AI(const _tchar* pLayerTag)
{
	if (m_bPlayerSpawn == false)
		return S_OK;

	Json PreviewData;
	PreviewData["Model"] = "Model_AI_CH0300";

	CGameObject* pAI_CH0300 = nullptr;
	NULL_CHECK(pAI_CH0300 = CGameInstance::GetInstance()->Clone_GameObject_Get(pLayerTag, TEXT("AI_CH0300"), &PreviewData));

	Json Tsugumi;
	Tsugumi["Model"] = "Model_AI_CH0500";

	CGameObject* pAI_CH0500 = nullptr;
	NULL_CHECK(pAI_CH0500 = CGameInstance::GetInstance()->Clone_GameObject_Get(pLayerTag, TEXT("AI_CH0500"), &Tsugumi));


	return S_OK;
}
