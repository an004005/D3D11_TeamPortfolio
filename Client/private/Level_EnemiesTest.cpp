#include "stdafx.h"
#include "Level_EnemiesTest.h"
#include "GameInstance.h"
#include "Material.h" 
#include "GameUtils.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "Imgui_LevelSwitcher.h"
#include "Imgui_MapEditor.h"
#include "Model.h"
#include "JsonLib.h"
#include "AnimationInstance.h"
#include "Texture.h"
#include "MapNonAnim_Object.h"
#include "JsonStorage.h"

// AJH's Initial Setting
//#include "Controller.h"
//#include "Floors.h"
//#include "Navigation.h"
//#include "PostVFX_WhiteOut.h"
//#include "JsonStorage.h"
//#include "Camera.h"

// Monster
#include "TestMonster.h"
#include "FlowerLeg.h"
#include "BuddyLumi.h"
#include "SkummyPool.h"
#include "SkMpBullet.h" // SkummPool's Bullet
#include "SkummyPandou.h"

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
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
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
	__super::Tick(TimeDelta);
}

void CLevel_EnemiesTest::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
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

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

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
		auto pSkMpBullet = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/StaticModel/Monster/SkPmBullet/SkMp_Bullet.static_model");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BulletSkummyPool"), pSkMpBullet));
	}

	{
		auto pSkummyPandou = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/SkummyPandou.anim_model");
		pSkummyPandou->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterSkummyPandou"), pSkummyPandou));
	}
	
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("TestMonster"), CTestMonster::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("FlowerLeg"), CFlowerLeg::Create(m_pDevice, m_pContext)));		
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BuddyLumi"), CBuddyLumi::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPool"), CSkummyPool::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkMpBullet"), CSkMpBullet::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPandou"), CSkummyPandou::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("TestMonster"))))
		return E_FAIL;*/

	Json FlowerLegModel;
	FlowerLegModel["Model"] = "MonsterFlowerLeg";
		
	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("FlowerLeg"), &FlowerLegModel)))
		return E_FAIL;

	Json BuddyLumiModel;
	BuddyLumiModel["Model"] = "MonsterBuddyLumi";

	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("BuddyLumi"), &BuddyLumiModel)))
		return E_FAIL;

	Json SkummyPoolModel;
	SkummyPoolModel["Model"] = "MonsterSkummyPool";

	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("SkummyPool"), &SkummyPoolModel)))
		return E_FAIL;

	Json SkummyPandouModel;
	SkummyPandouModel["Model"] = "MonsterSkummyPandou";

	/*if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("SkummyPandou"), &SkummyPandouModel)))
	return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Bullet(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/TestMap.json");
	
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
