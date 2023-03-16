#include "stdafx.h"
#include "Level_Batch.h"
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "JsonLib.h"
#include "Texture.h"
#include "Imgui_PropertyEditor.h"
#include "JsonStorage.h"
#include "GameUtils.h" 
#include "Material.h"
#include "SkyBox.h"
#include "Batch.h"
#include "BronJon.h"
#include "BuddyLumi.h"
#include "FlowerLeg.h"
#include "SkummyPandou.h"
#include "SkummyPool.h"
#include "Trigger.h"
#include "FL_Controller.h"
#include "SkmP_Controller.h"
#include "Boss1.h"
#include "Boss1_AIController.h"
#include "BdLm_Controller.h"
#include "SkPd_Controller.h"
#include "BrJ_Controller.h"
#include "FactoryMethod.h"
#include "VFX_Manager.h"

CLevel_Batch::CLevel_Batch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Batch::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CVFX_Manager::GetInstance()->Initialize(LEVEL_BATCH);
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

	if (FAILED(Ready_Layer_Batch(TEXT("Layer_Batch"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Batch::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_Batch::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT CLevel_Batch::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : BatchTool"));

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Lights()
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

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();


	//Batch
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_Batch", CBatch::Create(m_pDevice, m_pContext)));

	//MonsterModel

	{														// Bin\Resources\Model\AnimModel\Monster\Goat
		auto pGoat = CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/AnimModel/Monster/Goat/Goat.anim_model",
			_float4x4::CreateScale({ 0.1f, 0.1f, 0.1f }));
		pGoat->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/Goat/Anim/"); // Bin\Resources\Model\AnimModel\Monster\Goat\Anim
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("TestMonsterGoat"), pGoat));
	}

	{
		auto pFlowerLeg = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/FlowerLeg.anim_model");
		pFlowerLeg->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterFlowerLeg"), pFlowerLeg));
	}

	{						// Bin\Resources\Model\AnimModel\Monster\BuddyLumi
		auto pBuddyLumi = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/BuddyLumi/BuddyLumi.anim_model");
		pBuddyLumi->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/BuddyLumi/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBuddyLumi"), pBuddyLumi));
	}

	{
		auto pSkummyPool = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/SkummyPool/SkummyPool.anim_model");
		pSkummyPool->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPool/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterSkummyPool"), pSkummyPool));
	}

	{
		_float4x4	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.f));
		auto pSkMpBullet = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/StaticModel/Monster/SkPmBullet/SkMp_Bullet.static_model", PivotMatrix);
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BulletSkummyPool"), pSkMpBullet));
	}

	{
		auto pSkummyPandou = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/SkummyPandou.anim_model");
		pSkummyPandou->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterSkummyPandou"), pSkummyPandou));
	}

	{
		auto pBronJon = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/BronJon/BronJon.anim_model");
		pBronJon->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/BronJon/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBronJon"), pBronJon));
	}

	{
		auto pBoss1 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/boss1_em320/boss_1.anim_model");
		pBoss1->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/boss1_em320/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBoss1"), pBoss1));

		

		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1_Controller"), CBoss1_AIController::Create())))
			return E_FAIL;
	}


	//Monster
	FAILED_CHECK(Push_Back_Prototype(L"BronJon", CBronJon::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(L"BuddyLumi", CBuddyLumi::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(L"FlowerLeg", CFlowerLeg::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(L"SkummyPandou", CSkummyPandou::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(L"SkummyPool", CSkummyPool::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(TEXT("Prototype_MonsterBoss1"), CBoss1::Create(m_pDevice, m_pContext), MONSTER));

	//Controller	
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_FL_Controller"), CFL_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_SkmP_Controller"), CSkmP_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_SkPd_Controller"), CSkPd_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_BdLm_Controller"), CBdLm_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_BrJ_Controller"), CBrJ_Controller::Create()));

	FAILED_CHECK(CFactoryMethod::MakeUIPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeEffectPrototypes(m_pDevice, m_pContext));


	//SkyBox
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_NOW, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/StaticModel/Sky/SkySphere.static_model"))))
		return E_FAIL;

	//Trigger
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_NOW, TEXT("Prototype_GameObject_Trigger"),
		CTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// For_SkySphere
	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_Env", TEXT("Prototype_GameObject_SkyBox")));

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag.c_str(), TEXT("Prototype_GameObject_Camera_Dynamic")));
	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Layer_Batch(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json;// = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Batch/Batch_Test.json");
	
	json["ProtosInfo"] = Json::array();

	for (auto& info : m_ProtosInfo)
		json["ProtosInfo"].emplace_back(info);

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Batch"), &json));
	return S_OK;
}

HRESULT CLevel_Batch::Ready_Layer_Map(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_ConstructionSite3F.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));
	return S_OK;
}

HRESULT CLevel_Batch::Push_Back_Prototype(const _tchar * pLayerTag, CGameObject * pGameObject, BATCHTYPE eType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, pLayerTag, pGameObject));
	m_ProtosInfo.emplace_back(pLayerTag, eType);

	return S_OK;
}

CLevel_Batch* CLevel_Batch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Batch*	pInstance = new CLevel_Batch(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Batch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Batch::Free()
{
	CLevel::Free();
}
