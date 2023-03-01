#include "stdafx.h"
#include "Level_PlayerTest.h"

#include <Material.h>
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "Model.h"
#include "JsonLib.h"
#include "Imgui_AnimModifier.h"
#include "GameUtils.h"
#include "AnimationInstance.h"
#include "Player.h"
#include "Controller.h"
#include "CamSpot.h"
#include "JsonStorage.h"
#include "Weapon_wp0190.h"
#include "Imgui_PhysX.h"
#include "Imgui_PostProcess.h"
#include "Imgui_CameraManager.h"

#include "TrailSystem.h"

CLevel_PlayerTest::CLevel_PlayerTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_PlayerTest::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PhysX::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CameraManager::Create(m_pDevice, m_pContext));

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_PlayerTest::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_PlayerTest::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
	if (CGameInstance::GetInstance()->KeyDown(DIK_9))
	{
		CGameInstance::GetInstance()->Clone_GameObject(L"test", L"indicator");
	}
}

HRESULT CLevel_PlayerTest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : PlayerTest"));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	FAILED_CHECK(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& filePath)
	{
		string fileName = CGameUtils::GetFileName(filePath);
		CGameInstance::GetInstance()->Add_Prototype(
			s2ws(fileName).c_str(),
			CMaterial::Create(m_pDevice, m_pContext, filePath.c_str()));
	});

	pGameInstance->Add_Prototype(L"Player", CPlayer::Create(m_pDevice, m_pContext));
	pGameInstance->Add_Prototype(L"CamSpot", CCamSpot::Create(m_pDevice, m_pContext));

	auto pModel_Player = CModel::Create(m_pDevice, m_pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");

	pModel_Player->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Animation/");
	FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Player", pModel_Player));

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"),
		CController::Create())))
		return E_FAIL;

	_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationZ(XMConvertToRadians(180.f));
	pGameInstance->Add_Prototype(L"PlayerWeapon", CWeapon_wp0190::Create(m_pDevice, m_pContext));
	auto pModel_Weapon = CModel::Create(m_pDevice, m_pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);
	FAILED_CHECK(pGameInstance->Add_Prototype(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", pModel_Weapon));

	//FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_TrailSystem", CTrailSystem::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");
	//
	// if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	// 	return E_FAIL;


	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json PreviewData;
	PreviewData["Model"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Map(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/TestMap.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));
	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Effect(const _tchar * pLayerTag)
{
	return S_OK;
}

CLevel_PlayerTest* CLevel_PlayerTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_PlayerTest*	pInstance = new CLevel_PlayerTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_PlayerTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_PlayerTest::Free()
{
	CLevel::Free();
}
