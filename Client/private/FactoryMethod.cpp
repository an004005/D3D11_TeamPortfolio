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
// Canvas
#include "Canvas.h"
#include "Canvas_Item.h"
#include "Canvas_SASInfoLeft.h"
#include "Canvas_SASInfoLeftMove.h"
#include "Canvas_SASInfoRight.h"
#include "Canvas_SASInfoRightMove.h"
#include "Canvas_PlayerInfo.h"
#include "Canvas_PlayerInfoMove.h"
#include "Canvas_Drive.h"
#include "Canvas_DriveMove.h"
#include "Canvas_SASSkill.h"
#include "Canvas_SASSkillMove.h"
#include "Canvas_ItemMove.h"
#include "Canvas_Tutorial.h"
#include "Canvas_BossHp.h"
#include "Canvas_Alarm.h"

// Default
#include "DefaultUI.h"
#include "ButtonUI.h"
// SAS SKill
#include "SASSkillIconUI.h"
#include "SASSkillGaugeUI.h"
#include "SASSkillLightUI.h"
#include "SASSkillNameUI.h"
#include "SASSkillFullCircleUI.h"
// Direve
#include "Drive_RightDotUI.h"
#include "Drive_GaugeUI.h"
// Item
#include "Item_NameUI.h"
#include "Item_IconUI.h"
#include "Item_GaugeUI.h"
#include "Item_PushArrowUI.h"
#include "Item_LeftArrowUI.h"
#include "Item_RightArrowUI.h"
// PlayerInfo
#include "PlayerInfo_HpUI.h"
#include "PlayerInfo_HpBackUI.h"
#include "PlayerInfo_HpBothEndsUI.h"
#include "PlayerInfo_PsychokinesisUI.h"
#include "PlayerInfo_PsychokinesisBackUI.h"
#include "PlayerInfo_GaugeBackGround.h"
// SASInfo Left
#include "SASInfoLeftHpUI.h"
#include "SASInfoLeftHpBackUI.h"
#include "SASInfoLeftHpBothEndsUI.h"
#include "SASInfoRightHpUI.h"
#include "SASInfoRightHpBackUI.h"
#include "SASInfoRightHpBothEndsUI.h"
// Tutorial
#include "TutorialUI.h"
#include "Tutorial_CheckUI.h"
#include "Tutorial_YesNoUI.h"
#include "Tutorial_TipsUI.h"
#include "Tutorial_SuccessUI.h"

//Boss
#include "Boss_HpUI.h"
#include "Boss_HpBackUI.h"
#include "Boss_ShildUI.h"

// Alarm
#include "NextMapNameUI.h"
#include "Boss_AppearUI.h"
#include "Boss_AppearBackUI.h"
#include "LevelUpUI.h"

// 3D UI
#include "MonsterHpUI.h"
#include "MonsterLockonUI.h"
#include "GravikenisisMouseUI.h"

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

HRESULT CFactoryMethod::MakeUIPrototypes(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	{
		// Canvas_Frount_UI
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Item"),
			CCanvas_Item::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_ItemMove */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_ItemMove"),
			CCanvas_ItemMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfoLeft"),
			CCanvas_SASInfoLeft::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfoLeftMove"),
			CCanvas_SASInfoLeftMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_PlayerInfo"),
			CCanvas_PlayerInfo::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_PlayerInfoMove"),
			CCanvas_PlayerInfoMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Drive"),
			CCanvas_Drive::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_DriveMove"),
			CCanvas_DriveMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfoRight"),
			CCanvas_SASInfoRight::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfoRightMove"),
			CCanvas_SASInfoRightMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASSkill"),
			CCanvas_SASSkill::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SASSkillMove */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASSkillMove"),
			CCanvas_SASSkillMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_Tutorial*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Tutorial"),
			CCanvas_Tutorial::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_BossHp*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_BossHp"),
			CCanvas_BossHp::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_Alarm*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Alarm"),
			CCanvas_Alarm::Create(pDevice, pContext))))
			return E_FAIL;

	}

	{
		// UI
		/* For.Prototype_GameObject_Default_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Default_UI"),
			CDefaultUI::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Button_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Button_UI"),
			CButtonUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> SAS Skill
		/* For.Prototype_GameObject_SASSkillIcon_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillIcon_UI"),
			CSASSkillIconUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SASSkillGauge_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillGauge_UI"),
			CSASSkillGaugeUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SASSkillLight_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillLight_UI"),
			CSASSkillLightUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SASSkillCtrl_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillName_UI"),
			CSASSkillNameUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SASSkillFullCircle_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillFullCircle_UI"),
			CSASSkillFullCircleUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> Drive
		/* For.Prototype_GameObject_Drive_RightDotUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Drive_RightDotUI"),
			CDrive_RightDotUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Drive_RightDotUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Drive_GaugeUI"),
			CDrive_GaugeUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> Item
		/* For.Prototype_GameObject_Item_GaugeUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_GaugeUI"),
			CItem_GaugeUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_IconUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_IconUI"),
			CItem_IconUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_NameUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_NameUI"),
			CItem_NameUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_PushArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_PushArrowUI"),
			CItem_PushArrowUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_LeftArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_LeftArrowUI"),
			CItem_LeftArrowUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_RightArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_RightArrowUI"),
			CItem_RightArrowUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> PlayerInfo
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_HpUI"),
			CPlayerInfo_HpUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_HpBackUI"),
			CPlayerInfo_HpBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_HpBothEndsUI"),
			CPlayerInfo_HpBothEndsUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_PsychokinesisUI"),
			CPlayerInfo_PsychokinesisUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_PsychokinesisBackUI"),
			CPlayerInfo_PsychokinesisBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_GaugeBackGround"),
			CPlayerInfo_GaugeBackGround::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> SASInfoLeft
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoLeft_HpUI"),
			CSASInfoLeftHpUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoLeft_HpBackUI"),
			CSASInfoLeftHpBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoLeft_HpBothEndsUI"),
			CSASInfoLeftHpBothEndsUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> SASInfoRight
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoRight_HpUI"),
			CSASInfoRightHpUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoRight_HpBackUI"),
			CSASInfoRightHpBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoRight_HpBothEndsUI"),
			CSASInfoRightHpBothEndsUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Tutorial
		/* For.Prototype_GameObject_TutorialUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("TutorialUI"),
			CTutorialUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_CheckUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Tutorial_CheckUI"),
			CTutorial_CheckUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_YesNoUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Tutorial_YesNoUI"),
			CTutorial_YesNoUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_TipsUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Tutorial_TipsUI"),
			CTutorial_TipsUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_SuccessUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Tutorial_SuccessUI"),
			CTutorial_SuccessUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Boss
		/* For.Prototype_GameObject_Boss_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Boss_HpUI"),
			CBoss_HpUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Boss_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Boss_HPBackUI"),
			CBoss_HpBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Boss_ShildUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Boss_ShildUI"),
			CBoss_ShildUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Alarm
		/* For.Prototype_GameObject_NextRoomNameUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("NextMapNameUI"),
			CNextMapNameUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Boss_AppearUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Boss_AppearUI"),
			CBoss_AppearUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Boss_AppearBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Boss_AppearBackUI"),
			CBoss_AppearBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_LevelUpUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("LevelUpUI"),
			CLevelUpUI::Create(pDevice, pContext))))
			return E_FAIL;

	}
	
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_MonsterHP", CMonsterHpUI::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_MonsterLockon", CMonsterLockonUI::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_GravikenisisMouseUI", CGravikenisisMouseUI::Create(pDevice, pContext)));

}

void CFactoryMethod::Free()
{
}
