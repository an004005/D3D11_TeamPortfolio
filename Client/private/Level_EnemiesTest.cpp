#include "stdafx.h"
#include "Level_EnemiesTest.h"
#include "GameInstance.h"
#include "Material.h" 
#include "GameUtils.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "Imgui_LevelSwitcher.h"
#include "Imgui_AnimModifier.h"
#include "Imgui_MapEditor.h"
#include "Imgui_PhysX.h"
#include "Imgui_CameraManager.h"
#include "Model.h"
#include "JsonLib.h"
#include "AnimationInstance.h"
#include "Texture.h"
#include "MapNonAnim_Object.h"
#include "JsonStorage.h"
#include "RigidBody.h"

// Player Setting
#include "Player.h"
#include "Controller.h"
#include "CamSpot.h"
#include "Weapon_wp0190.h"
#include "Indicator.h"
#include "TrailSystem.h"

// Monster
#include "TestMonster.h"
#include "FlowerLeg.h"
#include "BuddyLumi.h"
#include "SkummyPool.h"
#include "SkMpBullet.h" // SkummPool's Bullet
#include "SkummyPandou.h"
#include "BronJon.h"

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


	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
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

	return S_OK;
}

void CLevel_EnemiesTest::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_EnemiesTest::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
	if (CGameInstance::GetInstance()->KeyDown(DIK_9))
	{
		CGameInstance::GetInstance()->Clone_GameObject(L"test", L"Indicator");
	}
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

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	{
		char szFileName[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	});

	// 02.28 KKB Player


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

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_TrailSystem", CTrailSystem::Create(m_pDevice, m_pContext)));

	pGameInstance->Add_Prototype(L"Indicator", CIndicator::Create(m_pDevice, m_pContext));

	// ~02.28 KKB Player
	
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
		
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("TestMonster"), CTestMonster::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("FlowerLeg"), CFlowerLeg::Create(m_pDevice, m_pContext)));		
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BuddyLumi"), CBuddyLumi::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPool"), CSkummyPool::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkMpBullet"), CSkMpBullet::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPandou"), CSkummyPandou::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BronJon"), CBronJon::Create(m_pDevice, m_pContext)));

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

	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("TestMonster"))))
		return E_FAIL;*/

	Json FlowerLegModel;
	FlowerLegModel["Model"] = "MonsterFlowerLeg";
		
	/*if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("FlowerLeg"), &FlowerLegModel)))
		return E_FAIL;*/
	
	Json BuddyLumiModel;
	BuddyLumiModel["Model"] = "MonsterBuddyLumi";

	/*Json BuddyLumiTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumiTrigger.json");*/

	/*if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("BuddyLumi"),& BuddyLumiModel)))
		return E_FAIL;*/

	Json SkummyPoolModel;
	SkummyPoolModel["Model"] = "MonsterSkummyPool";

	/*if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("SkummyPool"), &SkummyPoolModel)))
		return E_FAIL;*/

	Json SkummyPandouModel;
	SkummyPandouModel["Model"] = "MonsterSkummyPandou";
	
	/*if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("SkummyPandou"), &SkummyPandouModel)))
	return E_FAIL;*/

	Json BronJonModel;
	BronJonModel["Model"] = "MonsterBronJon";

	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("BronJon"), &BronJonModel)))
		return E_FAIL;

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

	Json PreviewData;
	PreviewData["Model"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Tutorial.json");
	
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));
	
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
