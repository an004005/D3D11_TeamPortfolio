#include "stdafx.h"
#include "..\public\FactoryMethod.h"
#include "GameInstance.h"

// Monster
#include "TestMonster.h"
#include "FlowerLeg.h"
#include "FL_Controller.h"

#include "BuddyLumi.h"
#include "BdLm_Controller.h"

#include "SkummyPool.h"
#include "SkMpBullet.h" // SkummPool's Bullet
#include "SkmP_Controller.h"

#include "SkummyPandou.h"
#include "SkPd_Controller.h"

#include "BronJon.h"
#include "BrJ_Controller.h"

#include "Boss1.h"
#include "Boss1_AIController.h"

#include "WaterBall.h"

// Player Setting
#include "Player.h"
#include "Controller.h"
#include "CamSpot.h"
#include "Weapon_wp0190.h"
#include "Indicator.h"
#include "MapKinetic_Object.h"
#include "TrailSystem.h"
#include "EffectSystem.h"

// Effect Setting
#include "PostVFX_Scifi.h"
#include "EffectGroup.h"
#include "ParticleSystem.h"
#include "ParticleGroup.h"
#include "PostVFX_Distortion.h"
#include "SAS_Portrait.h"

//UI
#include "MonsterHpUI.h"
#include "MonsterLockonUI.h"

CFactoryMethod::CFactoryMethod()
{
}


CFactoryMethod::~CFactoryMethod()
{
}


HRESULT CFactoryMethod::MakeEnermyPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	{														// Bin\Resources\Model\AnimModel\Monster\Goat
		auto pGoat = CModel::Create(pDevice, pContext, "../Bin/Resources/Model/AnimModel/Monster/Goat/Goat.anim_model",
			_float4x4::CreateScale({ 0.1f, 0.1f, 0.1f }));
		pGoat->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/Goat/Anim/"); // Bin\Resources\Model\AnimModel\Monster\Goat\Anim
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("TestMonsterGoat"), pGoat));
	}

	{
		auto pFlowerLeg = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/FlowerLeg.anim_model");
		pFlowerLeg->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterFlowerLeg"), pFlowerLeg));
	}

	{						// Bin\Resources\Model\AnimModel\Monster\BuddyLumi
		auto pBuddyLumi = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/Monster/BuddyLumi/BuddyLumi.anim_model");
		pBuddyLumi->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/BuddyLumi/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBuddyLumi"), pBuddyLumi));
	}

	{
		auto pSkummyPool = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/Monster/SkummyPool/SkummyPool.anim_model");
		pSkummyPool->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPool/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterSkummyPool"), pSkummyPool));
	}

	{
		_float4x4	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.f));
		auto pSkMpBullet = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/StaticModel/Monster/SkPmBullet/SkMp_Bullet.static_model", PivotMatrix);
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BulletSkummyPool"), pSkMpBullet));
	}

	{
		auto pSkummyPandou = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/SkummyPandou.anim_model");
		pSkummyPandou->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterSkummyPandou"), pSkummyPandou));
	}

	{
		//		_float4x4	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
		auto pBronJon = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/Monster/BronJon/BronJon.anim_model");
		pBronJon->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/BronJon/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBronJon"), pBronJon));
	}

	{
		auto pBoss1 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/Monster/boss1_em320/boss_1.anim_model");
		pBoss1->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/boss1_em320/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBoss1"), pBoss1));
	}

	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("FlowerLeg"), CFlowerLeg::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BuddyLumi"), CBuddyLumi::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPool"), CSkummyPool::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkMpBullet"), CSkMpBullet::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPandou"), CSkummyPandou::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BronJon"), CBronJon::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1"), CBoss1::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Prototype_WaterBall"), CWaterBall::Create(pDevice, pContext)));

	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_FL_Controller"), CFL_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_SkmP_Controller"), CSkmP_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_SkPd_Controller"), CSkPd_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_BdLm_Controller"), CBdLm_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_BrJ_Controller"), CBrJ_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1_Controller"), CBoss1_AIController::Create()));

	return S_OK;
}

HRESULT CFactoryMethod::MakePlayerPrototypes(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->Add_Prototype(L"Player", CPlayer::Create(pDevice, pContext));
	pGameInstance->Add_Prototype(L"CamSpot", CCamSpot::Create(pDevice, pContext));

	auto pModel_Player = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");

	pModel_Player->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Animation/");
	FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Player", pModel_Player));

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"),
		CController::Create())))
		return E_FAIL;

	_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationZ(XMConvertToRadians(180.f));
	pGameInstance->Add_Prototype(L"PlayerWeapon", CWeapon_wp0190::Create(pDevice, pContext));
	auto pModel_Weapon = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);
	FAILED_CHECK(pGameInstance->Add_Prototype(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", pModel_Weapon));

	pGameInstance->Add_Prototype(L"Indicator", CIndicator::Create(pDevice, pContext));

	return S_OK;
}

HRESULT CFactoryMethod::MakeEffectPrototypes(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Scifi"),
		CPostVFX_Scifi::Create(pDevice, pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Distortion"),
		CPostVFX_Distortion::Create(pDevice, pContext))))
		return E_FAIL;

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectSystem", CEffectSystem::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectGroup", CEffectGroup::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_TrailSystem", CTrailSystem::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_ParticleSystem", CParticleSystem::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_ParticleGroup", CParticleGroup::Create(pDevice, pContext)));
	return S_OK;
}

HRESULT CFactoryMethod::MakeSAS_Portrait_Prototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	{
		auto pModel_ch300 = CModel::Create(pDevice, pContext,
		   "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch300/ch300.anim_model");
		pModel_ch300->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch300/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Ch300_Portrait", pModel_ch300));
	}

	{
		auto pModel_ch500 = CModel::Create(pDevice, pContext,
		   "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch500/ch500.anim_model");
		pModel_ch500->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch500/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Ch500_Portrait", pModel_ch500));
	}

	{
		pGameInstance->Add_Prototype(L"Prototype_SASPortrait", CSAS_Portrait::Create(pDevice, pContext));
		pGameInstance->Add_Prototype(L"ProtoPostVFX_SASPortrait", CPostVFX_SAS_Portrait::Create(pDevice, pContext));
		
	}

	return S_OK;
}

HRESULT CFactoryMethod::MakeUITestPrototypes(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_MonsterHP", CMonsterHpUI::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_MonsterLockon", CMonsterLockonUI::Create(pDevice, pContext)));

	return S_OK;
}

void CFactoryMethod::Free()
{
}
