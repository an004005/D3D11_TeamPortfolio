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
#include "Indicator.h"
#include "MapKinetic_Object.h"
#include "PostVFX_Scifi.h"
#include "EffectGroup.h"
#include "ParticleSystem.h"
#include "PostVFX_Distortion.h"
#include "Batch.h"
#include "Trigger.h"
 
#include "BuddyLumi.h"
#include "BronJon.h"
#include "SkummyPool.h"
#include "SkMpBullet.h"
#include "SkmP_Controller.h"
#include "BrJ_Controller.h"

#include "TrailSystem.h"
#include "EffectSystem.h"

#include "Boss1.h"
#include "Boss1_AIController.h"
#include "FlowerLeg.h"
#include "FL_Controller.h"
#include "SkmP_Controller.h"
#include "Imgui_CurveManager.h"
#include "VFX_Manager.h"
#include "ParticleGroup.h"

#include "FactoryMethod.h"
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
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CurveManager::Create(m_pDevice, m_pContext));
	CVFX_Manager::GetInstance()->Initialize(LEVEL_PLAYERTEST);

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

	if (FAILED(Ready_Layer_Batch(TEXT("Layer_Batch"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Kinetic(TEXT("Layer_Kinetic"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Effect(TEXT("Layer_PostVFX"))))
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
		CGameInstance::GetInstance()->Clone_GameObject(L"test", L"Indicator");
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
	// LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDirection = _float4(-cosf(XMConvertToRadians(60.f)), -sinf(XMConvertToRadians(60.f)), 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& filePath)
	// {
	// 	string fileName = CGameUtils::GetFileName(filePath);
	// 	CGameInstance::GetInstance()->Add_Prototype(
	// 		s2ws(fileName).c_str(),
	// 		CMaterial::Create(m_pDevice, m_pContext, filePath.c_str()));
	// });

	pGameInstance->Add_Prototype(L"CamSpot", CCamSpot::Create(m_pDevice, m_pContext));

	{	// 플레이어 모델과 애니메이션
		auto pModel_Player = CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");
		pModel_Player->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Animation/");
		FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Player", pModel_Player));
		
		pGameInstance->Add_Prototype(L"Player", CPlayer::Create(m_pDevice, m_pContext));
	}

	{	// 플레이어 컨트롤러
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"),
			CController::Create())))
			return E_FAIL;
	}

	{	// 무기
		_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationZ(XMConvertToRadians(180.f));
		pGameInstance->Add_Prototype(L"PlayerWeapon", CWeapon_wp0190::Create(m_pDevice, m_pContext));
		auto pModel_Weapon = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", pModel_Weapon));
	}

	pGameInstance->Add_Prototype(L"Indicator", CIndicator::Create(m_pDevice, m_pContext));
	
	// 키네틱 오브젝트 모델
	CGameUtils::ListFilesRecursive("../Bin/Resources/Model/StaticModel/MapStaicModels/Kinetic/",
		[this](const string& fileName)
	{
		char szFileExt[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szFileExt, MAX_PATH);

		if (0 == strcmp(szFileExt, ".static_model"))
		{
			FAILED_CHECK(Create_Model(s2ws(fileName), fileName.c_str()));
		}
	});
	FAILED_CHECK(pGameInstance->Add_Prototype(L"Proto_KineticObject_Table", CMapKinetic_Object::Create(m_pDevice, m_pContext)));

	//{
	//	auto pBuddyLumi = CModel::Create(m_pDevice, m_pContext,
	//		"../Bin/Resources/Model/AnimModel/Monster/BuddyLumi/BuddyLumi.anim_model");
	//	pBuddyLumi->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/BuddyLumi/Anim/");
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBuddyLumi"), pBuddyLumi));
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BuddyLumi"), CBuddyLumi::Create(m_pDevice, m_pContext)));
	//}

	//{
	//	auto pBronJon = CModel::Create(m_pDevice, m_pContext,
	//		"../Bin/Resources/Model/AnimModel/Monster/BronJon/BronJon.anim_model");
	//	pBronJon->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/BronJon/Anim/");
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBronJon"), pBronJon));
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BronJon"), CBronJon::Create(m_pDevice, m_pContext)));
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_BrJ_Controller"), CBrJ_Controller::Create()));
	//}

	{	// 이펙트 프로토타입
		if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Scifi"),
			CPostVFX_Scifi::Create(m_pDevice, m_pContext)))) 
			return E_FAIL;

		if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Distortion"),
			CPostVFX_Distortion::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectSystem", CEffectSystem::Create(m_pDevice, m_pContext)));
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectGroup", CEffectGroup::Create(m_pDevice, m_pContext)));
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_TrailSystem", CTrailSystem::Create(m_pDevice, m_pContext)));
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_ParticleSystem", CParticleSystem::Create(m_pDevice, m_pContext)));
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_ParticleGroup", CParticleGroup::Create(m_pDevice, m_pContext)));
	}


	//// boss
	//{
	//	auto pBoss1 = CModel::Create(m_pDevice, m_pContext,
	//		"../Bin/Resources/Model/AnimModel/Monster/boss1_em320/boss_1.anim_model");
	//	pBoss1->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/boss1_em320/Anim/");
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBoss1"), pBoss1));


	//	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1"), CBoss1::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;

	//	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1_Controller"), CBoss1_AIController::Create())))
	//		return E_FAIL;

	//}

	//// 스커미풀
	//{
	//	auto pSkummyPool = CModel::Create(m_pDevice, m_pContext,
	//		"../Bin/Resources/Model/AnimModel/Monster/SkummyPool/SkummyPool.anim_model");
	//	pSkummyPool->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPool/Anim/");
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterSkummyPool"), pSkummyPool));
	//}
	//{
	//	_float4x4	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.f));
	//	auto pSkMpBullet = CModel::Create(m_pDevice, m_pContext,
	//		"../Bin/Resources/Model/StaticModel/Monster/SkPmBullet/SkMp_Bullet.static_model", PivotMatrix);
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BulletSkummyPool"), pSkMpBullet));
	//}

	//FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPool"), CSkummyPool::Create(m_pDevice, m_pContext)));
	//FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkMpBullet"), CSkMpBullet::Create(m_pDevice, m_pContext)));
	//FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_SkmP_Controller"), CSkmP_Controller::Create()));
	//// ~스커미풀
	//FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_SkmP_Controller"), CSkmP_Controller::Create()));

	////Model_flowerLeg
	//{
	//	auto pFlowerLeg = CModel::Create(m_pDevice, m_pContext,
	//		"../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/FlowerLeg.anim_model");
	//	pFlowerLeg->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/Anim/");
	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterFlowerLeg"), pFlowerLeg));
	//
	//	FAILED_CHECK(pGameInstance->Add_Prototype(L"Prototype_GameObject_FlowerLeg", CFlowerLeg::Create(m_pDevice, m_pContext)));

	//	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_FL_Controller"), CFL_Controller::Create()));
	//}

	FAILED_CHECK(CFactoryMethod::MakeEnermyPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeUITestPrototypes(m_pDevice, m_pContext));

	//Batch
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_Batch", CBatch::Create(m_pDevice, m_pContext)));

	//Trigger
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_NOW, TEXT("Prototype_GameObject_Trigger"),
		CTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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
	PreviewData["KineticModel"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Batch(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Batch/Batch_Test.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Batch"), &json));
	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Map(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/KineticTestMap.json");
	//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_DownTown.json");
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_ConstructionSite3F.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Kinetic(const _tchar * pLayerTag)
{
	/*CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json Test;
	Test["ModelTags"] = "../Bin/Resources/Model/StaticModel/MapStaicModels/Kinetic/Table/Table.static_model";
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Proto_KineticObject_Table"), &Test));*/

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Json BuddyLumiModel;
	//BuddyLumiModel["Model"] = "MonsterBuddyLumi";

	//Json BuddyLumiTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumiTrigger.json");

	//if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("BuddyLumi"),& BuddyLumiModel)))
	//return E_FAIL;

	//Json BronJonModel;
	//BronJonModel["Model"] = "MonsterBronJon";

	//if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("BronJon"), &BronJonModel)))
	//	return E_FAIL;

	//auto pObj = pGameInstance->Clone_GameObject_Get(pLayerTag, L"Prototype_MonsterBoss1");
	//_float4 pos = pObj->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	//pos.y += 1.f;
	//pObj->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, pos);

	//Json SkummyPoolModel;
	//SkummyPoolModel["Model"] = "MonsterSkummyPool";

	//if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("SkummyPool"), &SkummyPoolModel)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	//Json ScifiEffect = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_DefaultAttack_1.json");
	//pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &ScifiEffect);
	
	Json Test = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Scifi", &Test);

	Json Distortion = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Distortion/Distortion_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Distortion", &Distortion);

	return S_OK;
}

HRESULT CLevel_PlayerTest::Create_Model(const wstring & pProtoTag, const char * pModelPath)
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
