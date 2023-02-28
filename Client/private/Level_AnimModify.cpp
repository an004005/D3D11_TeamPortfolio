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
#include "Terrain.h"
#include "JsonStorage.h"
#include "Controller.h"

#include "Player.h"

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

	if (FAILED(Ready_Layer_Map(L"Layer_Map")))
		return E_FAIL;

	return S_OK;
}

void CLevel_AnimModify::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);

	//PeekPosSetting();
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
	auto pModel_Player = CModel::Create(m_pDevice, m_pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");

	// 모델에 애니메이션 추가하는 방법
	pModel_Player->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Animation/");

	// 프로토타입 추가 방법
	FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Player", pModel_Player));

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

	{
		_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationZ(XMConvertToRadians(180.f));
		auto pModel_Weapon = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", pModel_Weapon));
	}

	{
		// 플레이어 이벤트 콜러 지정용
		pGameInstance->Add_Prototype(L"Player", CPlayer::Create(m_pDevice, m_pContext));
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"),
			CController::Create())))
			return E_FAIL;
	}

	// PJW Monster Model Anim Control Purpose

	//// Goat
	//auto pModel_Goat = CModel::Create(m_pDevice, m_pContext,
	//	"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Goat/Goat.anim_model",
	//	_float4x4::CreateScale({ 0.01f, 0.01f, 0.01f }) * _float4x4::CreateRotationY(XMConvertToRadians(-90.f)));

	//pModel_Goat->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Goat/Animation/");

	//FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Goat", pModel_Goat));

	// Terrain
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Terrain")));
	
	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameObject* pGameObject = nullptr;
	pGameObject = (pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic")));
	//pGameObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(65.f, 5.f, 60.f, 1.f));
	//pGameObject->GetTransform()->LookAt(XMVectorSet(65.f, 0.f, 65.f, 1.f));
	NULL_CHECK(pGameObject);

	return S_OK;
}

HRESULT CLevel_AnimModify::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
						// Model_Player
	Json PreviewData; //	MonsterBuddyLumi	MonsterSkummyPool	MonsterFlowerLeg	MonsterSkummyPandou
	PreviewData["Model"] = "Model_Player";

	/*if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("ModelPreview"), &PreviewData)))
		return E_FAIL;*/

	auto pPlayer = (pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("ModelPreview"), &PreviewData));

	PreviewData["Model"] = "../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model";
	auto pwp = (pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("ModelPreview"), &PreviewData));
	
	static_cast<CModelPreviwer*>(pwp)->SetAttachTo("RightWeapon", static_cast<CModelPreviwer*>(pPlayer));

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

HRESULT CLevel_AnimModify::Ready_Layer_Map(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/TestMap.json");

	CGameObject* pGameObject = nullptr;
	pGameObject = (pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));
	//pGameObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(65.f, 0.f, 65.f, 1.f));

	return S_OK;
}

void CLevel_AnimModify::PeekPosSetting(void)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameObject* pTerrain = nullptr;
	_float4		 vPeekingPos = { 0.f, 0.f, 0.f, 0.f };
	NULL_CHECK(pTerrain = pGameInstance->GetLayer(LEVEL_NOW, TEXT("Layer_Terrain"))->GetGameObjects().front());
	static_cast<CTerrain*>(pTerrain)->PickTerrain(vPeekingPos);
	pGameInstance->SetPeekingPos(XMLoadFloat4(&vPeekingPos));
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
