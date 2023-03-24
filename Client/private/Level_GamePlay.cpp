#include "stdafx.h"
#include "Level_GamePlay.h"

#include <Imgui_AnimModifier.h>

#include "GameInstance.h"
#include "Material.h"
#include "Controller.h"
#include "Floors.h"
#include "JsonStorage.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "Imgui_PhysX.h"
#include "Boss1.h"
#include "Indicator.h"
#include "Player.h"
#include "SkyBox.h"

#include "Imgui_CurveManager.h"
#include "FactoryMethod.h"
#include "Imgui_Batch.h"
#include "Imgui_LevelSwitcher.h"
#include "VFX_Manager.h"
#include "SAS_Cable.h"
#include "Imgui_CameraManager.h"
#include "Imgui_LightManager.h"

#define ADD_PLAYER

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PhysX::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CameraManager::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CurveManager::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_Batch::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LightManager::Create(m_pDevice, m_pContext));



	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	Ready_Layer_Effect();

	Ready_Layer_SASPortrait(LAYER_SAS);

	CGameInstance::GetInstance()->Add_EmptyLayer(LEVEL_NOW, L"Layer_MapKineticObject");

	// m_TestSound.CloneSound("Ambient_Bridge");
	// m_TestSound.PlaySound("Ambient_Bridge");

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	// if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	// {
	// 	CGameInstance::GetInstance()->Clone_GameObject(L"LayerTest", L"Indicator");
	// }
}

void CLevel_GamePlay::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : GAMEPLAY"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();


	// CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	// {
	// 	char szFileName[MAX_PATH]{};
	// 	_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	// 	CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	// });


	pGameInstance->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));

#ifdef ADD_PLAYER
	// player
	{
	FAILED_CHECK(CFactoryMethod::MakePlayerPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeSAS_Portrait_Prototypes(m_pDevice, m_pContext));
	}
#endif

	
	FAILED_CHECK(CFactoryMethod::MakeMonsterExPrototypes(m_pDevice, m_pContext));



	// FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_PostVFX_ColorGrading", CPostVFX_ColorGrading::Create(m_pDevice, m_pContext)));

	CFactoryMethod::MakeUIPrototypes(m_pDevice, m_pContext);
	CFactoryMethod::MakeEffectPrototypes(m_pDevice, m_pContext);
	FAILED_CHECK(CFactoryMethod::MakeKineticPrototypes(m_pDevice, m_pContext));

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_SkyBox")));

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_ConstructionSite3F.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));


	Json PreviewData;

	{
		PreviewData["Model"] = "Prototype_Model_em200";
		PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND;
		auto pBoss = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("ModelPreview"), &PreviewData);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");


	//
	// const Json& json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/ShadowCam.json");
	// auto pCam = pGameInstance->Clone_GameObject_Get(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), (void*)&json);
	// pGameInstance->SetShadowCam(dynamic_cast<CCamera*>(pCam));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();


#ifdef ADD_PLAYER

	Json PreviewData;
	PreviewData["Model"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();


	{
		// PreviewData["Model"] = "Model_Ch300_Portrail";
		// PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND_TOON;
		// auto pBoss = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("ModelPreview"), &PreviewData);
	}
	// {
	// 	PreviewData["Model"] = "Model_Ch500_Portrait";
	// 	PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND_TOON;
	// 	auto pBoss = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("ModelPreview"), &PreviewData);
	// }



	// Model_Ch300_Portrail

	// auto pObj = pGameInstance->Clone_GameObject_Get(pLayerTag, L"Prototype_MonsterBoss1");
	// _float4 pos = pObj->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	// pos.y += 1.f;
	// pObj->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, pos);




	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Json Test = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Scifi", &Test);

	Json Distortion = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Distortion/Distortion_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Distortion", &Distortion);

	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"Prototype_PostVFX_ColorGrading");


	Json Penetrate = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Penetrate.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Penetrate", &Penetrate);

	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SASPortrait(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Clone_GameObject(LAYER_SAS, L"Prototype_SASPortrait");

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}