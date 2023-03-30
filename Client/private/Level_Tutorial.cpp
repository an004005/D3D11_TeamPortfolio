#include "stdafx.h"
#include "..\public\Level_Tutorial.h"

#include <Imgui_AnimModifier.h>
#include <Imgui_CameraManager.h>
#include <Imgui_PostProcess.h>

#include "GameInstance.h"
#include "Material.h"
#include "JsonStorage.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "FactoryMethod.h"
#include "Imgui_Batch.h"
#include "Imgui_CurveManager.h"
#include "Imgui_PhysX.h"
#include "SkyBox.h"
#include "GameManager_Tutorial.h"

#define ADD_PLAYER

CLevel_Tutorial::CLevel_Tutorial(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tutorial::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PhysX::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CameraManager::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CurveManager::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_Batch::Create(m_pDevice, m_pContext));

	CGameInstance::GetInstance()->Add_EmptyLayer(LEVEL_NOW, L"Layer_MapKineticObject");

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(PLAYERTEST_LAYER_CAMERA)))
		return E_FAIL;

#ifdef ADD_PLAYER
	if (FAILED(Ready_Layer_Player(PLATERTEST_LAYER_PLAYER)))
		return E_FAIL;
#endif

	if (FAILED(Ready_Layer_UI(PLAYERTEST_LAYER_FRONTUI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(PLAYERTEST_LAYER_MAP)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(PLAYERTEST_LAYER_POSTVFX)))
		return E_FAIL;

	Ready_Layer_SASPortrait();

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Tutorial/TutorialBatch_Enemy.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Tutorial/Tutorial_UI.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Tutorial/MapKineticObject_Test.json");

	// 기본 게임매니저 셋팅
	CGameManager_Tutorial::SetGameManager(CGameManager_Tutorial::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_Tutorial::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameManager_Tutorial::GetInstance()->Tick(TimeDelta);

}

void CLevel_Tutorial::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_Tutorial::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : TUTORIAL"));

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
#ifdef ADD_PLAYER
	FAILED_CHECK(CFactoryMethod::MakePlayerPrototypes(m_pDevice, m_pContext));
#endif

	FAILED_CHECK(CFactoryMethod::MakeMonsterExPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeUIPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeSAS_Portrait_Prototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeKineticPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeTriggerPrototypes(m_pDevice, m_pContext));

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Lights()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));


	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/ShadowCam/Tutorial_ShadowCam.json");
	CGameInstance::GetInstance()->Add_Camera("ShadowCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic", &json);
	CGameInstance::GetInstance()->SetShadowCam(CGameInstance::GetInstance()->FindCamera("ShadowCamera"));

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Player(const _tchar * pLayerTag)
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

HRESULT CLevel_Tutorial::Ready_Layer_UI(const _tchar * pLayerTag)
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

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Alarm", &json));
	
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Main.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Main", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MouseCousor.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_MouseCousor", &json));

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_SkyBox")));

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_Tutorial.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Effect(const _tchar * pLayerTag)
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

HRESULT CLevel_Tutorial::Ready_Layer_SASPortrait()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Clone_GameObject(LAYER_SAS, L"Prototype_SASPortrait");
	return S_OK;
}

CLevel_Tutorial * CLevel_Tutorial::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Tutorial*		pInstance = new CLevel_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tutorial::Free()
{
	__super::Free();

	CGameManager_Tutorial::GetInstance()->DestroyInstance();
}
