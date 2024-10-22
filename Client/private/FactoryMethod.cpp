#include "stdafx.h"
#include "..\public\FactoryMethod.h"
#include "GameInstance.h"
#include "Model.h"
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

#include "OilBullet.h"

#include "FlowerLeg_Invisible.h"
#include "FLInvisible_Controller.h"
#include "EM0110.h"
#include "EM0200.h"
#include "EM0210.h"
#include "EM0220.h"
#include "EM0221.h"
#include "EM0400.h"
#include "EM0650.h"
#include "EM0700.h" 
#include "EM0800.h"
#include "EM0900.h"
#include "EM1100.h"
#include "EM1200.h"
#include "TestTarget.h"
#include "Bullet.h"
#include "EMCable.h"
#include "EMBrain.h"
#include "EMGift.h"
#include "EM8200_CopyRush.h"
#include "EM8200_BrainField.h"
#include "EM8200_BrainCrushCables.h"

// Kinetic Object
#include "SpecialObject.h"
#include "Special_Train.h"
#include "Special_TelephonePole.h"
#include "Special_HBeam_Bundle.h"
#include "Special_HBeam_Single.h"
#include "Special_DropObject_Bundle.h"
#include "Special_DropObject_Single.h"
#include "Special_TankLorry.h"
#include "Special_TankLorry_Head.h"
#include "Special_TankLorry_Tank.h"
#include "Special_TankLorry_Trailer.h"
#include "Special_IronBars_Door.h"
#include "Special_IronBars_Bars.h"
#include "Special_IronBars.h"
#include "Special_IronBars_SingleBars.h"
#include "Special_IronBars_MultiBars.h"
#include "Special_Container.h"

// Player Setting
#include "Player.h"
#include "Controller.h"
#include "CamSpot.h"
#include "Weapon_wp0190.h"
#include "Indicator.h"
#include "MapKinetic_Object.h"
#include "TrailSystem.h"
#include "EffectSystem.h"
#include "Weapon_Player.h"
#include "Sheath_Player.h"

// Effect Setting
#include "PostVFX_Scifi.h"
#include "EffectGroup.h"
#include "ParticleSystem.h"
#include "ParticleGroup.h"
#include "PostVFX_Distortion.h"
#include "SAS_Portrait.h"
#include "SAS_Cable.h"

//UI
#include "MonsterShildUI.h"
#include "Level_Loading_Simple.h"
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
#include "Canvas_BossHpMove.h"
#include "Canvas_Alarm.h"
#include "Canvas_Main.h"
#include "Canvas_ItemWindow.h"
#include "Canvas_Acquisition.h"
#include "Canvas_LeftTalk.h"
#include "Canvas_MouseCousor.h"
#include "Canvas_Shop.h"
#include "Canvas_ShopListBar.h"
#include "Canvas_ListCheck.h"
#include "Canvas_Shortage.h"
#include "Canvas_SaleListCheck.h"
#include "Canvas_SAMouseLeft.h"
#include "Canvas_SAGragting_S.h"
#include "Canvas_SAGragting_Go.h"
#include "Canvas_SARebar.h"
#include "Canvas_SAContainer_Down.h"
#include "Canvas_MainTalk.h"
#include "Canvas_Quest.h"
#include "Canvas_WorldMap.h"
#include "Canvas_MapBar.h"
#include "Canvas_BrainField.h"
#include "Canvas_BrainFieldMove.h"

// Default
#include "DefaultUI.h"
#include "ButtonUI.h"
#include "ShaderUI.h"
// SAS SKill
#include "SASSkillIconUI.h"
#include "SASSkillGaugeUI.h"
#include "SASSkillLightUI.h"
#include "SASSkillNameUI.h"
#include "SASSkillFullCircleUI.h"
// Drive
#include "Drive_GaugeUI.h"
// Item
#include "Item_NameUI.h"
#include "Item_IconUI.h"
#include "Item_LightUI.h"
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
// Short
#include "NextMapNameUI.h"
#include "Boss_AppearUI.h"
#include "Boss_AppearBackUI.h"
#include "LevelUpUI.h"
#include "MapNameUI.h"
#include "QuestBackGroundUI.h"
#include "DistanceUI.h"
#include "HPlossUI.h"
// Main
#include "FullUI.h"
#include "Main_FaceUI.h"
#include "Main_BarUI.h"
#include "Main_PickUI.h"
#include "Main_SkillIconUI.h"
#include "Main_SkillNameUI.h"
#include "ItemIconUI.h"
#include "Main_OnMouseUI.h"
#include "Main_OnMouseColorUI.h"
#include "Main_BrainMapIconPickUI.h"
#include "Main_BrainGaugeUI.h"
#include "Main_BrainUI.h"
#include "Main_OnePickUI.h"
// Mouse
#include "MouseCousorUI.h"
#include "MouseCousorLightUI.h"
// Acquisition
#include "AcquisitionUI.h"
// Left Talk
#include "Talk_FaceUI.h"
#include "Talk_BackGroundUI.h"
// SpecialAttack
#include "SA_AxisUI.h"
#include "SA_RotationUI.h"

// 3D UI
#include "EM0320.h"
#include "EM0750.h"
#include "MonsterHpUI.h"
#include "MonsterLockonUI.h"
#include "GravikenisisMouseUI.h"
#include "GravikenisisGUI.h"
#include "NoticeNeonUI.h"
#include "SimpleTrigger.h"
#include "SpawnTrigger.h"
// UI Trigger
#include "TipTrigger.h"
#include "StoryTrigger.h"
#include "Alarm_Trigger.h"
// Lsat
#include "LastCheckUI.h"

// AI
#include "AI_CH0300.h"
#include "Weapon_wp0300.h"
#include "AI_CH0500.h"
#include "EM8200.h"
#include "Weapon_wp0500.h"

// NPC
#include "Shop_NPC.h"

// TeleportableWall
#include "HS_TeleportableWall.h"

// Item
#include "Consumption_Item.h"

#include "BrainField.h"

CFactoryMethod::CFactoryMethod()
{
}


CFactoryMethod::~CFactoryMethod()
{
}


HRESULT CFactoryMethod::MakeEnermyPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return S_OK;
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

	// Invisible
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("FlowerLegInvisible"), CFlowerLeg_Invisible::Create(pDevice, pContext)));
	// ~Invisible
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("FlowerLeg"), CFlowerLeg::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BuddyLumi"), CBuddyLumi::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPool"), CSkummyPool::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkMpBullet"), CSkMpBullet::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("SkummyPandou"), CSkummyPandou::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("BronJon"), CBronJon::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1"), CBoss1::Create(pDevice, pContext)));
	//FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Prototype_WaterBall"), CWaterBall::Create(pDevice, pContext)));
	// Invisible
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_FLInvisible_Controller"), CFLInvisible_Controller::Create()));
	// ~Invisible
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_FL_Controller"), CFL_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_SkmP_Controller"), CSkmP_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_SkPd_Controller"), CSkPd_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_BdLm_Controller"), CBdLm_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Proto_BrJ_Controller"), CBrJ_Controller::Create()));
	FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1_Controller"), CBoss1_AIController::Create()));

	return S_OK;
}

HRESULT CFactoryMethod::MakeMonsterExPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("TestTarget"), CTestTarget::Create(pDevice, pContext)));



	auto pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/FlowerLeg.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/Anim/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em200"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em200"), CEM0200::Create(pDevice, pContext)));

	{
		auto pEm320Model = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/Monster/boss1_em320/boss_1.anim_model");
		pEm320Model->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/boss1_em320/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em320"), pEm320Model));
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em320"), CEM0320::Create(pDevice, pContext)));
 		//FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("Prototype_OilBullet"), COilBullet::Create(pDevice, pContext)));
	}



	/* EM1200*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em1200/Model/SM_em1200.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em1200/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em1200"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em1200"), CEM1200::Create(pDevice, pContext)));

	/* EM1100*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em1100/Model/SM_em1100.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em1100/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em1100"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em1100"), CEM1100::Create(pDevice, pContext)));

	/* EM0900*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0900/Model/SM_em0900.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em0900/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em900"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em900"), CEM0900::Create(pDevice, pContext)));

	/* EM0800*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0800/Model/SM_em0800.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em0800/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em800"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em800"), CEM0800::Create(pDevice, pContext)));

	/* EM0750*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/SkummyPandou.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/SkummyPandou/Anim/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em750"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em750"), CEM0750::Create(pDevice, pContext)));


	/* EM0700*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0700/Model/SM_em0700.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em0700/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em700"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em700"), CEM0700::Create(pDevice, pContext)));


	/* EM0650*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0600/Model/SM_em0650.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em0600/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em650"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em650"), CEM0650::Create(pDevice, pContext)));

	/* EM0400*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0400/Model/SM_em0400.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em0400/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em400"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em400"), CEM0400::Create(pDevice, pContext)));

	/* EM0210*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0200/Model/SM_em0210.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em0200/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em210"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em210"), CEM0210::Create(pDevice, pContext)));

	/* EM0220*/
	_float4x4 Pivot = _float4x4::CreateScale({ 0.015f, 0.015f, 0.015f }) * _float4x4::CreateRotationY(XMConvertToRadians(-180.f));
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0200/Model/SM_em0220.anim_model", Pivot);
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em0200/AnimFor0220/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em220"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em220"), CEM0220::Create(pDevice, pContext)));

	/* EM0221*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0200/Model/SM_em0221.static_model");
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em221"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em221"), CEM0221::Create(pDevice, pContext)));

	/* EM0110*/
	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/em0100/Model/SM_em0110.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em0100/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em110"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em110"), CEM0110::Create(pDevice, pContext)));

	/********
	 * Karen *
	 ********/
	// _matrix RotMat = XMMatrixScaling(0.01f, 0.01f,0.01f) *XMMatrixRotationX(XMConvertToRadians(180.f));
	pEMModel = CModel::Create(pDevice, pContext,"../Bin/Resources/Model/AnimModel/Monster/em8200/Model/SM_em8200.anim_model");
	pEMModel->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/em8200/Animation/");

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_em8200"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em8200"), CEM8200::Create(pDevice, pContext)));


	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em8200_CopyRush"), CEM8200_CopyRush::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em8200_BrainField"), CEM8200_BrainField::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em8200_BrainCrushCables"), CEM8200_BrainCrushCables::Create(pDevice, pContext)));
	
	/********
	 * Karen *
	 ********/


	//몬스터에서 쓰는 클래스
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Bullet"), CBullet::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_EMCable"), CEMCable::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_EMGift"), CEMGift::Create(pDevice, pContext)));

	pEMModel = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Monster/co2000/Model/SM_co2001.static_model");
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Model_co2001"), pEMModel));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_EMBrain"), CEMBrain::Create(pDevice, pContext)));

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_BrainField", CBrainField::Create(pDevice, pContext)));

	return S_OK;
}

HRESULT CFactoryMethod::MakePlayerPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;


	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	{// SAS 케이블
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_SASCable", CSAS_Cable::Create(pDevice, pContext)));
	}



	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Player", CPlayer::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"CamSpot", CCamSpot::Create(pDevice, pContext)));

	auto pModel_Player = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");
	pModel_Player->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Animation/");
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Player", pModel_Player));

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_LocalController"), CController::Create()));


	{// 플레이어 무기 모델들
		_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(180.f));
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerWeapon", CWeapon_Player::Create(pDevice, pContext)));

		auto pModel_wp0106 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_106/wp0106.static_model", WeaponPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_106/wp0106.static_model", pModel_wp0106));

		auto pModel_wp0126 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_126/wp0126.static_model", WeaponPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_126/wp0126.static_model", pModel_wp0126));

		auto pModel_wp0190 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model2", pModel_wp0190));
	}

	{// 플레이어 칼집 모델들

		_matrix SheathPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);// *XMMatrixRotationZ(XMConvertToRadians(180.f));
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerSheath", CSheath_Player::Create(pDevice, pContext)));

		auto pModel_wp0107 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_107/wp0107.static_model", SheathPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_107/wp0107.static_model", pModel_wp0107));

		auto pModel_wp0127 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_127/wp0127.static_model", SheathPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_127/wp0127.static_model", pModel_wp0127));

		auto pModel_wp0191 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_191/wp0191.static_model", SheathPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_191/wp0191.static_model", pModel_wp0191));
	}

//	_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(180.f));
//	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"PlayerWeapon", CWeapon_wp0190::Create(pDevice, pContext)));
//	auto pModel_Weapon = CModel::Create(pDevice, pContext,
//		"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);


//	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model2", pModel_Weapon));


	return S_OK;
}

HRESULT CFactoryMethod::MakeEffectPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();


	return S_OK;
}

HRESULT CFactoryMethod::MakeSAS_Portrait_Prototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	{
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_SASPortrait", CSAS_Portrait::Create(pDevice, pContext)));
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"ProtoPostVFX_SASPortrait", CPostVFX_SAS_Portrait::Create(pDevice, pContext)));
	}

	return S_OK;
}

HRESULT CFactoryMethod::MakeKineticPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	auto pModel_Train = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Kinetic/Train/Train.anim_model");
	pModel_Train->LoadAnimations("../Bin/Resources/Model/AnimModel/Kinetic/Train/Animation/");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Train", pModel_Train));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_Train", CSpecial_Train::Create(pDevice, pContext)));


	auto pModel_TelephonePole = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/TelephonePole/Pole_A.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_TelephonePole", pModel_TelephonePole));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_TelephonePole", CSpecial_TelephonePole::Create(pDevice, pContext)));


	auto pModel_HBeam_Bundle = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/HBeam/HBeam_Bundle.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_HBeam_Bundle", pModel_HBeam_Bundle));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_HBeam_Bundle", CSpecial_HBeam_Bundle::Create(pDevice, pContext)));


	auto pModel_HBeam_Single = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/HBeam/HBeam_Single.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_HBeam_Single", pModel_HBeam_Single));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_HBeam_Single", CSpecial_HBeam_Single::Create(pDevice, pContext)));


	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_DropObject_Bundle", CSpecial_DropObject_Bundle::Create(pDevice, pContext)));


	auto pModel_DropObject_Single = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/HBeam/HBeam_Single.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_DropObject_Single", pModel_DropObject_Single));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_DropObject_Single", CSpecial_DropObject_Single::Create(pDevice, pContext)));


	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_TankLorry", CSpecial_TankLorry::Create(pDevice, pContext)));

	auto pModel_TankLorry_Head = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/TankLorry/TankLorry_Head.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_TankLorry_Head", pModel_TankLorry_Head));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_TankLorry_Head", CSpecial_TankLorry_Head::Create(pDevice, pContext)));

	auto pModel_TankLorry_Trailer = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/TankLorry/TankLorry_Trailer.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_TankLorry_Trailer", pModel_TankLorry_Trailer));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_TankLorry_Trailer", CSpecial_TankLorry_Trailer::Create(pDevice, pContext)));


	auto pModel_TankLorry_Tank_01 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/TankLorry/TankLorry_Tank_01.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_TankLorry_Tank_0", pModel_TankLorry_Tank_01));
	auto pModel_TankLorry_Tank_02 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/TankLorry/TankLorry_Tank_02.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_TankLorry_Tank_1", pModel_TankLorry_Tank_02));
	auto pModel_TankLorry_Tank_03 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/TankLorry/TankLorry_Tank_03.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_TankLorry_Tank_2", pModel_TankLorry_Tank_03));
	auto pModel_TankLorry_Tank_04 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/TankLorry/TankLorry_Tank_04.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_TankLorry_Tank_3", pModel_TankLorry_Tank_04));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_TankLorry_Tank", CSpecial_TankLorry_Tank::Create(pDevice, pContext)));


	auto pModel_IronBars_Door = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Kinetic/IronBars/IronBars_Door.anim_model");
	pModel_IronBars_Door->LoadAnimations("../Bin/Resources/Model/AnimModel/Kinetic/IronBars/Animation/");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_IronBars_Door", pModel_IronBars_Door));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_IronBars_Door", CSpecial_IronBars_Door::Create(pDevice, pContext)));

	auto pModel_IronBars_Bars = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/AnimModel/Kinetic/IronBars/IronBars_Bars.anim_model");
	pModel_IronBars_Bars->LoadAnimations("../Bin/Resources/Model/AnimModel/Kinetic/IronBars/Animation/");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_IronBars_Bars", pModel_IronBars_Bars));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_IronBars_Bars", CSpecial_IronBars_Bars::Create(pDevice, pContext)));

	auto pModel_IronBars_SingleBars = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/IronBars/IronBars_SingleBars.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_IronBars_SingleBars", pModel_IronBars_SingleBars));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_IronBars_SingleBars", CSpecial_IronBars_SingleBars::Create(pDevice, pContext)));

	auto pModel_IronBars_MultiBars = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/IronBars/IronBars_MultiBars.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_IronBars_MultiBars", pModel_IronBars_MultiBars));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_IronBars_MultiBars", CSpecial_IronBars_MultiBars::Create(pDevice, pContext)));

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_IronBars", CSpecial_IronBars::Create(pDevice, pContext)));


	auto pModel_Container_01 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/Container/Container_01.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Container_01", pModel_Container_01));
	auto pModel_Container_02 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/Container/Container_02.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Container_02", pModel_Container_02));
	auto pModel_Container_03 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/Container/Container_03.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Container_03", pModel_Container_03));
	auto pModel_Container_04 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/Container/Container_04.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Container_04", pModel_Container_04));
	auto pModel_Container_05 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/Container/Container_05.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Container_05", pModel_Container_05));
	auto pModel_Container_06 = CModel::Create(pDevice, pContext,
		"../Bin/Resources/Model/StaticModel/Kinetic/Container/Container_06.static_model");
	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Container_06", pModel_Container_06));

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_Special_Container", CSpecial_Container::Create(pDevice, pContext)));

	return S_OK;
}

HRESULT CFactoryMethod::MakeAIPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;

	// hanabi
	{
		CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"AI_CH0300", CAI_CH0300::Create(pDevice, pContext));

		auto pModel_Ch0300 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch300/ch300.anim_model");
		pModel_Ch0300->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch300/Animation/");
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_AI_CH0300", pModel_Ch0300));

		/*_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(180.f));
		CGameInstance::GetInstance()->Add_Prototype(L"HanabiWeapon", CWeapon_wp0300::Create(pDevice, pContext));
		auto pModel_Weapon = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_300/wp0300.static_model", WeaponPivot);
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_300/wp0300.static_model", pModel_Weapon));*/

		_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(180.f));
		CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"HanabiWeapon", CWeapon_wp0300::Create(pDevice, pContext));
		auto pModel_Weapon = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_300/wp0300.static_model", WeaponPivot);
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_300/wp0300.static_model", pModel_Weapon));
	}

	// tsugumi
	{
		CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"AI_CH0500", CAI_CH0500::Create(pDevice, pContext));

		auto pModel_Ch0500 = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch500/ch500.anim_model");
		pModel_Ch0500->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch500/Animation/");
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_AI_CH0500", pModel_Ch0500));

		_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);// *XMMatrixRotationZ(XMConvertToRadians(180.f));
		CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"TsugumiWeapon", CWeapon_wp0500::Create(pDevice, pContext));
		auto pModel_Weapon = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_500/wp0500.static_model", WeaponPivot);
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_500/wp0500.static_model", pModel_Weapon));
	}

	return S_OK;
}

HRESULT CFactoryMethod::MakeTriggerPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_SpawnTrigger"), CSpawnTrigger::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_SimpleTrigger"), CSimpleTrigger::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_TipTrigger"), CTipTrigger::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_StoryTrigger"), CStoryTrigger::Create(pDevice, pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Alarm_Trigger"), CAlarm_Trigger::Create(pDevice, pContext))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CFactoryMethod::MakeUIPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	{
		// Canvas_Frount_UI
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Item"),
			CCanvas_Item::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_ItemMove */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_ItemMove"),
			CCanvas_ItemMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SASInfoLeft"),
			CCanvas_SASInfoLeft::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SASInfoLeftMove"),
			CCanvas_SASInfoLeftMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_PlayerInfo"),
			CCanvas_PlayerInfo::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_PlayerInfoMove"),
			CCanvas_PlayerInfoMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Drive"),
			CCanvas_Drive::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_DriveMove"),
			CCanvas_DriveMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SASInfoRight"),
			CCanvas_SASInfoRight::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SASInfoRightMove"),
			CCanvas_SASInfoRightMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SASSkill"),
			CCanvas_SASSkill::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SASSkillMove */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SASSkillMove"),
			CCanvas_SASSkillMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_Tutorial*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Tutorial"),
			CCanvas_Tutorial::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_BossHp*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_BossHp"),
			CCanvas_BossHp::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_BossHpMove*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_BossHpMove"),
			CCanvas_BossHpMove::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_Alarm*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Alarm"),
			CCanvas_Alarm::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_Main*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Main"),
			CCanvas_Main::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_ItemWindow */
		if (FAILED(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_ItemWindow"),
			CCanvas_ItemWindow::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_Acquisition */
		if (FAILED(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Acquisition"),
			CCanvas_Acquisition::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_LeftTalk */
		if (FAILED(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_LeftTalk"),
			CCanvas_LeftTalk::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_MouseCousor*/
		if (FAILED(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_MouseCousor"),
			CCanvas_MouseCousor::Create(pDevice, pContext))))
			return E_FAIL;



		
		/* For.Prototype_GameObject_Canvas_Shop*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Shop"),
			CCanvas_Shop::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_ShopListBar*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_ShopListBar"),
			CCanvas_ShopListBar::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_ListCheck*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_ListCheck"),
			CCanvas_ListCheck::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_Shortage*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Shortage"),
			CCanvas_Shortage::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SaleListCheck*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SaleListCheck"),
			CCanvas_SaleListCheck::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_SAMouseLeft*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SAMouseLeft"),
			CCanvas_SAMouseLeft::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SAGragting_S*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SAGragting_S"),
			CCanvas_SAGragting_S::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SAGragting_Go*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SAGragting_Go"),
			CCanvas_SAGragting_Go::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SARebar*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SARebar"),
			CCanvas_SARebar::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SAContainer_Down*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_SAContainer_Down"),
			CCanvas_SAContainer_Down::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_MainTalk*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_MainTalk"),
			CCanvas_MainTalk::Create(pDevice, pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_Quest*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_Quest"),
			CCanvas_Quest::Create(pDevice, pContext))))
			return E_FAIL;

		// WorldMap
		/* For.Prototype_GameObject_Canvas_WorldMap*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_WorldMap"),
			CCanvas_WorldMap::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_MapBar*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_MapBar"),
			CCanvas_MapBar::Create(pDevice, pContext))))
			return E_FAIL;

		// BrainFieldUI
		/* For.Prototype_GameObject_Canvas_BrainFieldUI*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_BrainField"),
			CCanvas_BrainField::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_BrainFieldMove*/
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Canvas_BrainFieldMove"),
			CCanvas_BrainFieldMove::Create(pDevice, pContext))))
			return E_FAIL;


		
	}

	{
		// UI
		/* For.Prototype_GameObject_Default_UI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Default_UI"),
			CDefaultUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Button_UI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Button_UI"),
			CButtonUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Shader_UI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Shader_UI"),
			CShaderUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_ItemIconUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("ItemIconUI"),
			CItemIconUI::Create(pDevice, pContext))))
			return E_FAIL;


		// Frount_UI -> SAS Skill
		/* For.Prototype_GameObject_SASSkillIcon_UI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASSkillIcon_UI"),
			CSASSkillIconUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SASSkillGauge_UI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASSkillGauge_UI"),
			CSASSkillGaugeUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SASSkillLight_UI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASSkillLight_UI"),
			CSASSkillLightUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SASSkillCtrl_UI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASSkillName_UI"),
			CSASSkillNameUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SASSkillFullCircle_UI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASSkillFullCircle_UI"),
			CSASSkillFullCircleUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> Drive
		/* For.Prototype_GameObject_Drive_Drive_GaugeUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Drive_GaugeUI"),
			CDrive_GaugeUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> Item
		/* For.Prototype_GameObject_Item_GaugeUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Item_GaugeUI"),
			CItem_GaugeUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_IconUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Item_IconUI"),
			CItem_IconUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_LightUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Item_LightUI"),
			CItem_LightUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_NameUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Item_NameUI"),
			CItem_NameUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_PushArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Item_PushArrowUI"),
			CItem_PushArrowUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_LeftArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Item_LeftArrowUI"),
			CItem_LeftArrowUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_RightArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Item_RightArrowUI"),
			CItem_RightArrowUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> PlayerInfo
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("PlayerInfo_HpUI"),
			CPlayerInfo_HpUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("PlayerInfo_HpBackUI"),
			CPlayerInfo_HpBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("PlayerInfo_HpBothEndsUI"),
			CPlayerInfo_HpBothEndsUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("PlayerInfo_PsychokinesisUI"),
			CPlayerInfo_PsychokinesisUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisBackUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("PlayerInfo_PsychokinesisBackUI"),
			CPlayerInfo_PsychokinesisBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisBackUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("PlayerInfo_GaugeBackGround"),
			CPlayerInfo_GaugeBackGround::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> SASInfoLeft
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASInfoLeft_HpUI"),
			CSASInfoLeftHpUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASInfoLeft_HpBackUI"),
			CSASInfoLeftHpBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASInfoLeft_HpBothEndsUI"),
			CSASInfoLeftHpBothEndsUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Frount_UI -> SASInfoRight
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASInfoRight_HpUI"),
			CSASInfoRightHpUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASInfoRight_HpBackUI"),
			CSASInfoRightHpBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SASInfoRight_HpBothEndsUI"),
			CSASInfoRightHpBothEndsUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Tutorial
		/* For.Prototype_GameObject_TutorialUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("TutorialUI"),
			CTutorialUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_CheckUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Tutorial_CheckUI"),
			CTutorial_CheckUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_YesNoUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Tutorial_YesNoUI"),
			CTutorial_YesNoUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_TipsUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Tutorial_TipsUI"),
			CTutorial_TipsUI::Create(pDevice, pContext))))
			return E_FAIL;
		
		// Main
		/* For.Prototype_GameObject_MainUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("FullUI"),
			CFullUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_FaceUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_FaceUI"),
			CMain_FaceUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_BarUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_BarUI"),
			CMain_BarUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_PickUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_PickUI"),
			CMain_PickUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_SkillIconUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_SkillIconUI"),
			CMain_SkillIconUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_SkillNameUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_SkillNameUI"),
			CMain_SkillNameUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_OnMouseUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_OnMouseUI"),
			CMain_OnMouseUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_BrainMapIconPickUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_BrainMapIconPickUI"),
			CMain_BrainMapIconPickUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_OnMouseColorUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_OnMouseColorUI"),
			CMain_OnMouseColorUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_BrainGaugeUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_BrainGaugeUI"),
			CMain_BrainGaugeUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_BrainUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_BrainUI"),
			CMain_BrainUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Main_OnePickUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Main_OnePickUI"),
			CMain_OnePickUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Mouse
		/* For.Prototype_GameObject_MouseCousorUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("MouseCousorUI"),
			CMouseCousorUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_MouseCousorUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("MouseCousorLightUI"),
			CMouseCousorLightUI::Create(pDevice, pContext))))
			return E_FAIL;

		// Short
		/* For.Prototype_GameObject_NextRoomNameUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("NextMapNameUI"),
			CNextMapNameUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Boss_AppearUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Boss_AppearUI"),
			CBoss_AppearUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Boss_AppearBackUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Boss_AppearBackUI"),
			CBoss_AppearBackUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_LevelUpUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("LevelUpUI"),
			CLevelUpUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_MapNameUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("MapNameUI"),
			CMapNameUI::Create(pDevice, pContext))))
			return E_FAIL;
		// Acquisition
		/* For.Prototype_GameObject_AcquisitionUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("AcquisitionUI"),
			CAcquisitionUI::Create(pDevice, pContext))))
			return E_FAIL;
		// Left Talk
		/* For.Prototype_GameObject_Talk_FaceUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Talk_FaceUI"),
			CTalk_FaceUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Talk_BackGroundUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Talk_BackGroundUI"),
			CTalk_BackGroundUI::Create(pDevice, pContext))))
			return E_FAIL;
		// Quset
		/* For.Prototype_GameObject_QuestBackGroundUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("QuestBackGroundUI"),
			CQuestBackGroundUI::Create(pDevice, pContext))))
			return E_FAIL;
		// Distance
		/* For.Prototype_GameObject_DistanceUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("DistanceUI"),
			CDistanceUI::Create(pDevice, pContext))))
			return E_FAIL;
		// HPlossUI
		/* For.Prototype_GameObject_HPlossUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("HPlossUI"),
			CHPlossUI::Create(pDevice, pContext))))
			return E_FAIL;

		// SpecialAttack
		/* For.Prototype_GameObject_SA_AxisUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SA_AxisUI"),
			CSA_AxisUI::Create(pDevice, pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_SA_RotationUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("SA_RotationUI"),
			CSA_RotationUI::Create(pDevice, pContext))))
			return E_FAIL;

		// LastCheckUI
		/* For.Prototype_GameObject_LastCheckUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("LastCheckUI"),
			CLastCheckUI::Create(pDevice, pContext))))
			return E_FAIL;




		
	}

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_MonsterHP", CMonsterHpUI::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_MonsterLockon", CMonsterLockonUI::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_MonsterShield", CMonsterShildUI::Create(pDevice, pContext)));

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_GravikenisisMouseUI", CGravikenisisMouseUI::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_GravikenisisGUI", CGravikenisisGUI::Create(pDevice, pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_NoticeNeonUI", CNoticeNeonUI::Create(pDevice, pContext)));

	return S_OK;
}

HRESULT CFactoryMethod::MakeNPCPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	{
		auto pModel_NPC = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/NPC/SM_2300.anim_model");
		pModel_NPC->LoadAnimations("../Bin/Resources/Model/AnimModel/NPC/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_NPC", pModel_NPC));

		/* For.Prototype_ShopNPC */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Shop_NPC"),
			CShop_NPC::Create(pDevice, pContext))))
			return E_FAIL;	
	}

	{
		auto pModel_ConsumptionItem = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/AnimModel/Item/Consumption/co0700.anim_model");
		pModel_ConsumptionItem->LoadAnimations("../Bin/Resources/Model/AnimModel/Item/Consumption/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_ConsumptionItem", pModel_ConsumptionItem));

		auto pModel_Flower = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/StaticModel/MapStaicModels/Instancing/Subway_Hospital/Hospital/props_merges/SM_Rose_Flower.static_model");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Flower", pModel_Flower));

		/* For.Prototype_Consumption_Item */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("ConsumptionItem"),
			CConsumption_Item::Create(pDevice, pContext))))
			return E_FAIL;
	}

	CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"ProtoType_LoadingModel", CLoadingModel::Create(pDevice, pContext));

	return S_OK;
}

HRESULT CFactoryMethod::MakeTWallPrototype(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	static _bool bCreated = false;
	if (bCreated)
		return S_OK;

	bCreated = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	{		
		auto pModel_TWall = CModel::Create(pDevice, pContext,
			"../Bin/Resources/Model/StaticModel/MapStaicModels/Default/TeleportWall.static_model");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_TWall", pModel_TWall));

		/* For.Prototype_GameObject_Main_OnePickUI */
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("TeleportableWall"),
			CHS_TeleportableWall::Create(pDevice, pContext))))
			return E_FAIL;
	}

	return S_OK;
}

void CFactoryMethod::Free()
{
}
