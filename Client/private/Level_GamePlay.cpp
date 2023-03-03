#include "stdafx.h"
#include "Level_GamePlay.h"

#include <Imgui_AnimModifier.h>

#include "GameInstance.h"
#include "Material.h"
#include "Controller.h"
#include "Floors.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Material.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "Imgui_PhysX.h"
#include "TestCamera.h"
#include "Boss1.h"
#include "Boss1_AIController.h"
#include "CamSpot.h"
#include "EffectSystem.h"
#include "Player.h"
#include "Weapon_wp0190.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PhysX::Create(m_pDevice, m_pContext));

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

	// m_TestSound.CloneSound("Ambient_Bridge");
	// m_TestSound.PlaySound("Ambient_Bridge");

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
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


	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	{
		char szFileName[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	});

	pGameInstance->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));

	// player
	{
		pGameInstance->Add_Prototype(L"Player", CPlayer::Create(m_pDevice, m_pContext));
		pGameInstance->Add_Prototype(L"CamSpot", CCamSpot::Create(m_pDevice, m_pContext));

		{
			auto pModel_Player = CModel::Create(m_pDevice, m_pContext,
				"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");

			pModel_Player->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Animation/");
			FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Player", pModel_Player));
		}

		_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationZ(XMConvertToRadians(180.f));
		pGameInstance->Add_Prototype(L"PlayerWeapon", CWeapon_wp0190::Create(m_pDevice, m_pContext));
		auto pModel_Weapon = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", pModel_Weapon));

		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectSystem", CEffectSystem::Create(m_pDevice, m_pContext)));

	}


	{
		auto pBoss1 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/boss1_em320/boss_1.anim_model");
		pBoss1->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/boss1_em320/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBoss1"), pBoss1));
	}

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"), CController::Create())))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_TestCamera"), CTestCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1"), CBoss1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1_Controller"), CBoss1_AIController::Create())))
		return E_FAIL;

	//// 23.02.20 PJW Work

	//{														// Bin\Resources\Model\AnimModel\Monster\Goat
	//	auto pGoat = CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/AnimModel/Monster/Goat/Goat.anim_model",
	//		_float4x4::CreateScale({ 0.1f, 0.1f, 0.1f }));
	//	pGoat->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/Goat/Anim/"); // Bin\Resources\Model\AnimModel\Monster\Goat\Anim
	//	pGameInstance->Add_Prototype(TEXT("TestMonsterGoat"), pGoat);
	//}

	//{														// \Bin\Resources\Model\AnimModel\Monster\FlowerLeg
	//	auto pFlowerLeg = CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/FlowerLeg.anim_model",
	//		_float4x4::CreateScale({ 0.1f, 0.1f, 0.1f }));
	//	pFlowerLeg->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/Anim/"); // \Bin\Resources\Model\AnimModel\Monster\FlowerLeg\Anim
	//	pGameInstance->Add_Prototype(TEXT("MonsterFlowerLeg"), pFlowerLeg);
	//}

	//pGameInstance->Add_Prototype(TEXT("TestMonster"), CTestMonster::Create(m_pDevice, m_pContext));

	//// Model Component Origin Create

	//// ~23.02.20 PJW Work

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();



	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/TestMap.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));

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

	Json PreviewData;
	PreviewData["Model"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Json PreviewData;
	// PreviewData["Model"] = "MonsterBoss1";
	// auto pBoss = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("ModelPreview"), &PreviewData);

	auto pObj = pGameInstance->Clone_GameObject_Get(pLayerTag, L"Prototype_MonsterBoss1");
	_float4 pos = pObj->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	pos.y += 1.f;
	pObj->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, pos);

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