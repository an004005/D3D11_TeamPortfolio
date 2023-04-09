#include "stdafx.h"
#include "Level_EnemiesTest.h"

#include "GameInstance.h"
#include "Material.h" 
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "Imgui_LevelSwitcher.h"
#include "Imgui_AnimModifier.h"
#include "Imgui_PhysX.h"
#include "Imgui_CameraManager.h"
#include "Imgui_CurveManager.h"
#include "Model.h"
#include "JsonLib.h"
#include "JsonStorage.h"
#include "SkyBox.h"
#include "FactoryMethod.h"
#include "Trigger.h"
#include "Batch.h"
#include "EffectGroup.h"
#include "VFX_Manager.h"
#include "Imgui_Batch.h"
#include "GameManager.h"

//#define ADD_PLAYER
//#define ADD_PREVIEW "Prototype_Model_em1200"

CLevel_EnemiesTest::CLevel_EnemiesTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_EnemiesTest::Initialize()
{
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

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Bullet(TEXT("Layer_Bullet"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Effect(TEXT("Layer_PostVFX"))))
		return E_FAIL;

#ifdef ADD_PLAYER
	if (FAILED(Ready_Layer_UI(TEXT("Layer_FrontUI"))))
		return E_FAIL;
#endif // ADD_UI

	

	Ready_Layer_SASPortrait();

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_EnemiesTest::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_SPACE))
	{
		CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, L"em0650_Bullet_Birth")->Start_EffectWork();
	}
}

void CLevel_EnemiesTest::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
	// if (CGameInstance::GetInstance()->KeyDown(DIK_9))
	// {
	// 	CGameInstance::GetInstance()->Clone_GameObject_Get(L"test", TEXT("FlowerLeg"))
	// 		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(20.f, 3.f, 6.f, 1.f));
	// }
}

HRESULT CLevel_EnemiesTest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : EnemiesTest"));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	/*LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);*/

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	// LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDirection = _float4(-cosf(XMConvertToRadians(60.f)), -sinf(XMConvertToRadians(60.f)), 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

#ifdef ADD_PLAYER
	FAILED_CHECK(CFactoryMethod::MakePlayerPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeSAS_Portrait_Prototypes(m_pDevice, m_pContext));
#endif
	//FAILED_CHECK(CFactoryMethod::MakeEnermyPrototypes(m_pDevice, m_pContext));

	FAILED_CHECK(CFactoryMethod::MakeUIPrototypes(m_pDevice, m_pContext));

	FAILED_CHECK(CFactoryMethod::MakeMonsterExPrototypes(m_pDevice, m_pContext));
	
	pGameInstance->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_BackGround(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// For_SkySphere
	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_Env", TEXT("Prototype_GameObject_SkyBox")));

#ifdef ADD_PREVIEW
	
	Json PreviewData;
	{
		PreviewData["Model"] = ADD_PREVIEW;
		PreviewData["RenderGroup"] = CRenderer::RENDER_NONALPHABLEND;
		auto pBoss = pGameInstance->Clone_GameObject_Get(pLayerTag.c_str(), TEXT("ModelPreview"), &PreviewData);

	}
#endif // ADD_PREVIEW

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/tests.json");


	//FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_Gift", TEXT("Prototype_EMGift")));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	// 	return E_FAIL;

	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

#ifndef ADD_PLAYER
	pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("TestTarget"))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(0.f, 2.f, 0.f, 1.f));
#endif // !ADD_PLAYER

#ifdef ADD_ENEMY
	pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT(ADD_ENEMY))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(3.f, 3.f, 3.f, 1.f));

#endif // ADD_ENEMY

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Bullet(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();	
	return S_OK;
}


HRESULT CLevel_EnemiesTest::Ready_Layer_Player(const _tchar * pLayerTag)
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


HRESULT CLevel_EnemiesTest::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
																// Map_ConstructionSite3F
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_DownTown.json");
	
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_UI(const _tchar * pLayerTag)
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

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASSkill", &json));		// ¿òÁ÷ÀÌÁö ¾ÊÀº UI

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASSkillMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASSkillMove", &json));	// ?€ì§ì´??UI

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Main.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Main", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MouseCousor.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_MouseCousor", &json));

	//CGameUtils::ListFilesRecursive("../Bin/Resources/Objects/UI/", [&](const string& filePath)
	//{
	//	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(filePath);
	//	wstring protoTag = s2ws(json["Prototype_GameObject"]);
	//	pGameInstance->Clone_GameObject(pLayerTag, protoTag.c_str(), &json);
	//});

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_SASPortrait()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Clone_GameObject(LAYER_SAS, L"Prototype_SASPortrait");

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Effect(const _tchar * pLayerTag)
{
	 CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	 Json Test = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_PostVFX.json");
	 pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Scifi", &Test);

	 Json Distortion = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Distortion/Distortion_PostVFX.json");
	 pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Distortion", &Distortion);

	//Json Penetrate = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Penetrate.json");
	//pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Penetrate", &Penetrate);

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Create_Model(const wstring & pProtoTag, const char * pModelPath)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CComponent* pComponent = nullptr;

	pComponent = CModel::Create(m_pDevice, m_pContext, pModelPath);
	assert(pComponent != nullptr);

	FAILED_CHECK(pGameInstance->Add_Prototype(
		pProtoTag.c_str(),
		pComponent));

	//m_pProtosTags.emplace_back(pProtoTag,  NON_INSTANCE);

	return S_OK;
}

CLevel_EnemiesTest * CLevel_EnemiesTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_EnemiesTest*		pInstance = new CLevel_EnemiesTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EnemiesTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_EnemiesTest::Free()
{
	__super::Free();
}
