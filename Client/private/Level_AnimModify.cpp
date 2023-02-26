#include "stdafx.h"
#include "Level_AnimModify.h"

#include <Material.h>
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "Model.h"
#include "JsonLib.h"
#include "Imgui_AnimModifier.h"
#include "GameUtils.h"
#include "AnimationInstance.h"

CLevel_AnimModify::CLevel_AnimModify(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_AnimModify::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_AnimModel(L"Layer_AnimModel")))
		return E_FAIL;

	return S_OK;
}

void CLevel_AnimModify::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_AnimModify::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT CLevel_AnimModify::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : AnimTool"));

	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	FAILED_CHECK(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	 CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& filePath)
	 {
	 	string fileName = CGameUtils::GetFileName(filePath);
	 	CGameInstance::GetInstance()->Add_Prototype(
	 		s2ws(fileName).c_str(),
	 		CMaterial::Create(m_pDevice, m_pContext, filePath.c_str()));
	 });

	pGameInstance->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));
//	pGameInstance->Add_Prototype(L"GoatPreview", CModelPreviwer::Create(m_pDevice, m_pContext));

	// 모델 추가하는 방법
	auto pModel_TestPlayer = CModel::Create(m_pDevice, m_pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/TestPlayer/Test.anim_model");

	// 모델에 애니메이션 추가하는 방법
	pModel_TestPlayer->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/TestPlayer/Animation/");

	// 프로토타입 추가 방법
	FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_TestPlayer", pModel_TestPlayer));

	// PJW Monster Model Anim Control Purpose
	{
		auto pBuddyLumi = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/BuddyLumi/BuddyLumi.anim_model");
		pBuddyLumi->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/BuddyLumi/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBuddyLumi"), pBuddyLumi));
	}

	{
		auto pFlowerLeg = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/FlowerLeg.anim_model");
		pFlowerLeg->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterFlowerLeg"), pFlowerLeg));
	}

	{
		auto pSkummyPool = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/SkummyPool/SkummyPool.anim_model");
		pSkummyPool->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPool/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterSkummyPool"), pSkummyPool));
	}

	{
		auto pSkummyPandou = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/SkummyPandou.anim_model");
		pSkummyPandou->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterSkummyPandou"), pSkummyPandou));
	}

	// PJW Monster Model Anim Control Purpose

	//// Goat
	//auto pModel_Goat = CModel::Create(m_pDevice, m_pContext,
	//	"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Goat/Goat.anim_model",
	//	_float4x4::CreateScale({ 0.01f, 0.01f, 0.01f }) * _float4x4::CreateRotationY(XMConvertToRadians(-90.f)));

	//pModel_Goat->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Goat/Animation/");

	//FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Goat", pModel_Goat));

	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	
	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json PreviewData; // MonsterSkummyPandou  Model_TestPlayer  MonsterSkummyPool
	PreviewData["Model"] = "Model_TestPlayer";

	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("ModelPreview"), &PreviewData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Layer_AnimModel(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	//Json PreviewData;
	//PreviewData["Model"] = "Model_Goat";

	//if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("GoatPreview"), &PreviewData)))
	//	return E_FAIL;

	return S_OK;
}

CLevel_AnimModify* CLevel_AnimModify::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_AnimModify*	pInstance = new CLevel_AnimModify(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_AnimModify");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_AnimModify::Free()
{
	CLevel::Free();
}
