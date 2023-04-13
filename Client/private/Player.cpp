#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "Shader.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "BaseAnimInstance.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Controller.h"
#include "ScarletWeapon.h"
#include "Camera.h"
#include "ScarletWeapon.h"
#include "Weapon_wp0190.h"
#include "RigidBody.h"
#include "EffectSystem.h"
#include "ControlledRigidBody.h"
#include "MapKinetic_Object.h"
#include "PhysX_Manager.h"
#include "Monster.h"
#include <random>
#include "RigidBody.h"
#include "EffectGroup.h"
#include "CamSpot.h"
#include "VFX_Manager.h"
#include "Material.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"

#include "MonsterLockonUI.h"
#include "EMUI.h"

#include "NoticeNeonUI.h"
#include "JsonLib.h"
#include "ImguiUtils.h"
#include "SAS_Portrait.h"
#include "Weapon_wp0190.h"
#include "SpecialObject.h"
#include "SAS_Cable.h"

#include "PlayerInfoManager.h"
#include "Special_Train.h"
#include "Special_TelephonePole.h"
#include "Special_HBeam_Bundle.h"
#include "Special_HBeam_Single.h"
#include "Special_DropObject_Bundle.h"
#include "Special_TankLorry.h"
#include "Special_IronBars.h"
#include "Special_Container.h"

#include "Enemy.h"
#include "PostVFX_Penetrate.h"
#include "PlayerStartPosition.h"
#include "SuperSpeedTrail.h"
#include "PostVFX_SuperSpeed.h"
#include "PostVFX_Teleport.h"
#include "AnimCam.h"

#include "Canvas_SAMouseLeft.h"
#include "PlayerHotFixer.h"
#include "BrainField.h"

#include "Weapon_Player.h"
#include "Sheath_Player.h"
#include "GameManager.h"

#include "EM0210.h"
#include "EM0700.h"
#include "EM0800.h"

#include "Renderer.h"
#include "Map_KineticBatchPreset.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CScarletCharacter(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CScarletCharacter(rhs)
{
}

_float4x4 CPlayer::GetBoneMatrix(const string& strBoneName, _bool bPivotapply)
{
	if (m_pModel->Get_BonePtr(strBoneName) == nullptr)
		return XMMatrixIdentity();
	
	return m_pModel->GetBoneMatrix(strBoneName, bPivotapply);
}

_float4x4 CPlayer::GetPivotMatrix()
{
	if (m_pModel == nullptr)
		return XMMatrixIdentity();

	return m_pModel->GetPivotMatrix();
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_AnimSocket()))
		return E_FAIL;

	if (FAILED(Setup_Parts()))
		return E_FAIL;

	if (FAILED(SetUp_Event()))
		return E_FAIL;

	if (FAILED(SetUp_EffectEvent()))
		return E_FAIL;

	if (FAILED(SetUp_ProductionEvent()))
		return E_FAIL;

	if (FAILED(SetUp_DriveModeProductionStateMachine()))
		return E_FAIL;

	if (FAILED(Setup_KineticStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_HitStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_KineticComboStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_JustDodgeStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_RigidBody()))
		return E_FAIL;

	if (FAILED(SetUp_AttackDesc()))
		return E_FAIL;

	if (FAILED(SetUp_Sound()))
		return E_FAIL;

	if (FAILED(CPlayerInfoManager::GetInstance()->Initialize()))
		return E_FAIL;

	if (FAILED(SetUp_TrainStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_TelephonePoleStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_BrainCrashStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_HBeamStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_TeleportStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_DropObjectStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_TankLorryStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_IronBarsStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_ContainerStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_BrainFieldKineticStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_BrainFieldKineticComboStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_BrainFieldAttackStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_BrainFieldFallStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_BrainFieldProductionStateMachine()))
		return E_FAIL;

	if (FAILED(SetUp_DeBuffStateMachine()))
		return E_FAIL;

	m_pGameInstance->Add_EmptyLayer(LEVEL_NOW, LAYER_KINETIC);
	m_pGameInstance->Add_EmptyLayer(LEVEL_NOW, LAYER_PLAYEREFFECT);
	m_pGameInstance->Add_EmptyLayer(LEVEL_NOW, L"Layer_MapKineticObject");

	ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
	ZeroMemory(&m_AttackDesc, sizeof(DAMAGE_PARAM));

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pTransformCom->SetTransformDesc({ 5.f, XMConvertToRadians(720.f) });

	m_pPlayerCam = m_pGameInstance->Add_Camera("PlayerCamera", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Player");
	Safe_AddRef(m_pPlayerCam);
	CPlayerInfoManager::GetInstance()->Set_PlayerCam(m_pPlayerCam);

	m_pPlayer_AnimCam = dynamic_cast<CAnimCam*>(m_pGameInstance->Add_Camera("PlayerAnimCamera", LEVEL_NOW, L"Layer_Camera", L"Prototype_AnimCam"));
	Safe_AddRef(m_pPlayer_AnimCam);

	Json Penetrate = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Penetrate.json");
	m_pSAS_Penetrate = dynamic_cast<CPostVFX_Penetrate*>(m_pGameInstance->Clone_GameObject_Get(LAYER_PLAYEREFFECT, L"ProtoPostVFX_Penetrate", &Penetrate));
	Safe_AddRef(m_pSAS_Penetrate);

	Json SuperSpeed = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/SuperSpeed.json");
	m_pSuperSpeedPostVFX = dynamic_cast<CPostVFX_SuperSpeed*>(m_pGameInstance->Clone_GameObject_Get(LAYER_PLAYEREFFECT, L"ProtoPostVFX_SuperSpeed", &SuperSpeed));
	Safe_AddRef(m_pSuperSpeedPostVFX);

	Json Teleport = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Teleport.json");
	m_pTeleportPostVFX = dynamic_cast<CPostVFX_Teleport*>(m_pGameInstance->Clone_GameObject_Get(LAYER_PLAYEREFFECT, L"ProtoPostVFX_Teleport", &Teleport));
	Safe_AddRef(m_pTeleportPostVFX);
	m_pTeleportPostVFX->GetParam().Floats[0] = 1.f;

	m_pModel->FindMaterial(L"MI_ch0100_HOOD_0")->SetActive(false);

	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_00");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_01");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_02");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_03");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_04");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_05");

	m_pGameInstance->Add_EmptyLayer(LEVEL_NOW, L"Layer_MapKineticObject");

	m_NoticeTick.Initialize(1.0, false);
	m_DetectTimer.Initialize(1.0, false);

	m_strWeaponAttachBone = "RightWeapon";

	return S_OK;
}

void CPlayer::BeginTick()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (auto pStartPos = pGameInstance->Find_OneObjectByType<CPlayerStartPosition>(LEVEL_NOW, LAYER_TRIGGER))
	{
		m_pTransformCom->CopyWorld(pStartPos->GetTransform());
		pStartPos->SetDelete();
	}

//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-5.f, 2.f, 5.f, 1.f));
	__super::BeginTick();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Player")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == TEXT("CamSpot"))
		{
			m_pCamSpot = static_cast<CCamSpot*>(iter);
		}
	}

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, LAYER_SAS)->GetGameObjects())
	{
		if (auto pSasPortrait = dynamic_cast<CSAS_Portrait*>(iter))
		{
			m_pSasPortrait = pSasPortrait;
		}
	}
	// ?åÏä§??
	//m_pKineticAnimModel->SetPlayAnimation("AS_no0000_271_AL_Pcon_cReL_Lv4");

	// ?ÑÏπò ÏßÄ?ïÏ? ?¨Í∏∞?êÏÑú
}

void CPlayer::Tick(_double TimeDelta)
{
	m_fTimeDelta = (_float)TimeDelta;
	m_DetectTimer.Tick(TimeDelta);

	__super::Tick(TimeDelta);
	m_pModel->Tick(TimeDelta);
	m_pTrail->Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_NUMPAD7))
	{
		m_bBrainField = true;
	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_NUMPAD8))
	{
		m_bBrainField = false;
	}

	CPlayerInfoManager::GetInstance()->Set_BrainField(m_bBrainField);

	Production_Tick(TimeDelta);

	if (!m_DetectList.empty())
	{
		CPlayerInfoManager::GetInstance()->Set_BattleState(true);
		CPlayerInfoManager::GetInstance()->Set_KineticEnetgyType(0);
	}
	else
	{
		CPlayerInfoManager::GetInstance()->Set_BattleState(false);
		CPlayerInfoManager::GetInstance()->Set_KineticEnetgyType(2);
	}

	CPlayerInfoManager::GetInstance()->Tick(TimeDelta);

	KineticObject_OutLineCheck();
	//SpecialObject_OutLineCheck();
	Update_TargetUI();

	m_fKineticCombo_Slash -= TimeDelta;
	m_fKineticCombo_Kinetic -= TimeDelta;
	m_fJustDodgeAble -= TimeDelta;

	Check_Parts();
	NetualChecker(TimeDelta);
	Update_CautionNeon();
	SasMgr();

	if (CGameInstance::GetInstance()->KeyDown(DIK_Q))
	{
		Enemy_Targeting(false);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_E))
	{
		CPlayerInfoManager::GetInstance()->Camera_Random_Shake(0.1f);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_NUMPAD1))
	{
		CPlayerInfoManager::GetInstance()->Camera_Axis_Sliding({1.f, 0.f, 0.f, 0.f}, 0.1f);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_NUMPAD2))
	{
		CPlayerInfoManager::GetInstance()->Camera_Axis_Sliding({ 0.f, 1.f, 0.f, 0.f }, 0.1f);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_NUMPAD3))
	{
		CPlayerInfoManager::GetInstance()->Camera_Axis_Sliding({ 0.f, 0.f, 1.f, 0.f }, 0.1f);
	}

	 if (m_pPlayerCam->IsMainCamera())
		m_pController->Tick(TimeDelta);
	 else
		m_pController->Invalidate();

	MoveStateCheck(TimeDelta);
	BehaviorCheck(TimeDelta);

	// µÂ∂Û¿Ã∫Í∏µÂ ø¨√‚, √÷øÏº±¿˚
	m_pDriveModeProductionStateMachine->Tick(TimeDelta);
	if (!m_bDriveMode_Activate)
		m_pBrainFieldProductStateMachine->Tick(TimeDelta);

	if (CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_TELEPORT)) 
		m_pTeleportStateMachine->Tick(TimeDelta);

	m_pHitStateMachine->Tick(TimeDelta);

	HitCheck();

	m_pBrainCrashStateMachine->Tick(TimeDelta);

	// ?πÏàòÍ∏?
	if (false == m_bKineticSpecial)
	{
		SpecialObject_Targeting();
	}

	if (!m_bHit && !m_bBrainCrash && !m_bBrainField)
	{
		if (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject())
		{
			if (m_pTrainStateMachine_Left->GetCurStateName() == "TRAIN_LEFT_NOUSE" &&
				m_pTelephonePoleStateMachine_Left->GetCurStateName() == "TELEPHONEPOLE_LEFT_NOUSE" &&
				m_pHBeamStateMachine_Left->GetCurStateName() == "HBEAM_LEFT_NOUSE" &&
				m_pDropObjectStateMachine->GetCurStateName() == "DROP_NOUSE" &&
				m_pTankLorryStateMachine->GetCurStateName() == "TANKLORRY_NOUSE" &&
				m_pIronBarsStateMachine->GetCurStateName() == "IRONBARS_NOUSE" &&
				m_pContainerStateMachine->GetCurStateName() == "CONTAINER_NOUSE")
			{
				static_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->SetOutline(true);
			}
			else
			{
				static_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->SetOutline(false);
			}

			if (SPECIAL_TRAIN == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pTrainStateMachine_Left->Tick(TimeDelta);
			}

			else if (SPECIAL_TELEPHONEPOLE == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pTelephonePoleStateMachine_Left->Tick(TimeDelta);
			}

			else if (SPECIAL_HBEAM_BUNDLE == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pHBeamStateMachine_Left->Tick(TimeDelta);
			}

			else if (SPECIAL_DROPOBJECT_BUNDLE == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pDropObjectStateMachine->Tick(TimeDelta);
			}

			else if (SPECIAL_TANKLORRY == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pTankLorryStateMachine->Tick(TimeDelta);
			}

			else if (SPECIAL_IRONBARS == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pIronBarsStateMachine->Tick(TimeDelta);
			}

			else if (SPECAIL_CONTAINER == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pContainerStateMachine->Tick(TimeDelta);
			}
		}

		if (static_cast<CCamSpot*>(m_pCamSpot)->GetCamMod() == CCamSpot::MOD_ATTACH &&
			nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
		{
			m_pTransformCom->LookAt_NonY(CPlayerInfoManager::GetInstance()->Get_TargetedMonster()
				->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
		}
	}
	// ~?πÏàòÍ∏?

	if(!m_bHit && !m_bBrainCrash && !m_bBrainField)
		m_pJustDodgeStateMachine->Tick(TimeDelta);

	if (!m_bHit && (false == m_bKineticCombo) && (false == m_bKineticSpecial) && !m_bBrainCrash)
	{
		if (m_bBrainField)
		{
			m_pBrainFieldKineticStateMachine->Tick(TimeDelta);
		}
		else
		{
			m_pKineticStataMachine->Tick(TimeDelta);
		}
	}
	if (!m_bHit && (false == m_bKineticSpecial) && !m_bBrainCrash)
	{
		if (m_bBrainField)
		{
			m_pBrainFieldKineticComboStateMachine->Tick(TimeDelta);
		}
		else
		{
			m_pKineticComboStateMachine->Tick(TimeDelta);
		}
	}

	if (m_bHit)
	{
		m_pKineticComboStateMachine->SetState("KINETIC_COMBO_NOUSE");
		m_pKineticStataMachine->SetState("NO_USE_KINETIC");
		m_pJustDodgeStateMachine->SetState("JUSTDODGE_NONUSE");
		m_pTrainStateMachine_Left->SetState("TRAIN_LEFT_NOUSE");
		m_pTelephonePoleStateMachine_Left->SetState("TELEPHONEPOLE_LEFT_NOUSE");
		m_pHBeamStateMachine_Left->SetState("HBEAM_LEFT_NOUSE");
		m_pDropObjectStateMachine->SetState("DROP_NOUSE");
		m_pTeleportStateMachine->SetState("TELEPORTATTACK_NOUSE");
		m_pTankLorryStateMachine->SetState("TANKLORRY_NOUSE");
		m_pIronBarsStateMachine->SetState("IRONBARS_NOUSE");
		m_pContainerStateMachine->SetState("CONTAINER_NOUSE");
		for (auto& iter : m_vecWeapon)
		{
			static_cast<CScarletWeapon*>(iter)->Trail_Setting(false);
			static_cast<CScarletWeapon*>(iter)->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, false);
		}
		static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();

		m_pBrainFieldKineticStateMachine->SetState("BF_NO_USE_KINETIC");
		m_pBrainFieldKineticComboStateMachine->SetState("BF_NO_USE_KINETIC_COMBO");
		m_pBrainFieldAttackStateMachine->SetState("NO_USE_BRAINFIELD");
		m_pBrainFieldFallStateMachine->SetState("BF_NO_USE_KINETIC_FALL");

		if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
		{
			static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Dynamic();
		}

		CAnimation* pKineticAnim = nullptr;
		if (pKineticAnim = m_pKineticAnimModel->GetPlayAnimation())
		{
			pKineticAnim->Reset();
		}

		m_pCurve = nullptr;
		for (auto& iter : m_pModel->GetMaterials())
		{
			iter->GetParam().Floats[0] = 0.f;
			iter->GetParam().Float4s[0] = { 0.f, 0.f, 0.f, 0.f };
		}
	}

	if (m_bBrainField)
	{
		m_pASM->SetCurState("IDLE");
		m_pKineticComboStateMachine->SetState("KINETIC_COMBO_NOUSE");
		m_pKineticStataMachine->SetState("NO_USE_KINETIC");
		m_pJustDodgeStateMachine->SetState("JUSTDODGE_NONUSE");
		m_pTrainStateMachine_Left->SetState("TRAIN_LEFT_NOUSE");
		m_pTelephonePoleStateMachine_Left->SetState("TELEPHONEPOLE_LEFT_NOUSE");
		m_pHBeamStateMachine_Left->SetState("HBEAM_LEFT_NOUSE");
		m_pDropObjectStateMachine->SetState("DROP_NOUSE");
		m_pTeleportStateMachine->SetState("TELEPORTATTACK_NOUSE");
		m_pTankLorryStateMachine->SetState("TANKLORRY_NOUSE");
		m_pIronBarsStateMachine->SetState("IRONBARS_NOUSE");
		m_pContainerStateMachine->SetState("CONTAINER_NOUSE");
		m_pTeleportStateMachine->SetState("TELEPORTATTACK_NOUSE");
		m_pDriveModeProductionStateMachine->SetState("DRIVEMODE_NOUSE");
	}
	else
	{
		m_pASM->SetCurState_BrainField("IDLE");
		m_pBrainFieldKineticStateMachine->SetState("BF_NO_USE_KINETIC");
		m_pBrainFieldKineticComboStateMachine->SetState("BF_NO_USE_KINETIC_COMBO");
		m_pBrainFieldAttackStateMachine->SetState("NO_USE_BRAINFIELD");
		m_pBrainFieldFallStateMachine->SetState("BF_NO_USE_KINETIC_FALL");
	}

	SeperateCheck();

	// ¿œπ› ªÛ≈¬¿œ ∞ÊøÏ
	if (!m_bBrainField)
	{
		m_pASM->Tick(TimeDelta);
	}
	else if (m_bBrainField)
	{
		m_pASM->Tick_BrainField(TimeDelta);
		m_pBrainFieldAttackStateMachine->Tick(TimeDelta);
		m_pBrainFieldFallStateMachine->Tick(TimeDelta);
	}
	// ∫Í∑π¿Œ « µÂ ¿€µø¡ﬂ¿Œ ∞ÊøÏ

	m_strBefStateName = m_strCurStateName;
	m_strCurStateName = m_pASM->GetCurStateName();
	if (m_strBefStateName != m_strCurStateName)
	{
		Event_Trail(false);

		for (auto& iter : m_vecWeapon)
		{
			static_cast<CScarletWeapon*>(iter)->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, false);
		}

		m_bActionAble = true;
	}

	// Î°úÏª¨ Î¨¥Î∏åÍ∞Ä ?ÜÎäî???êÏ†ê?ºÎ°úÎ∂Ä??Î≤óÏñ¥???†ÎãàÎ©îÏù¥?òÏùÑ ?°Í∏∞ ?ÑÌïú Ï°∞Ïπò
	// !! BaseAninInstance?êÏÑú Î≥¥Í∞Ñ???ùÎÇòÍ≥?Normal?ÖÎç∞?¥Ìä∏Í∞Ä ?????¥Îãπ ÏΩîÎìúÎ•??§Ìñâ?úÌÇ§?ÑÎ°ù ?òÏûê
	if (0.f != XMVectorGetX(XMVector3Length(m_vSyncEffectLocal)))
	{
		_bool bTest = m_pASM->isSocketLerping();

		if (m_pIronBarsStateMachine->GetCurStateName() == "IRONBARS_NOUSE" && bTest)
		{
			m_pTransformCom->LocalMove(m_vSyncEffectLocal);
			m_vSyncEffectLocal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
	}// ??ÉÅ ASM?????¥ÌõÑ??Î∞úÏÉù?òÎèÑÎ°??úÎã§
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if (m_bCanMove)
	{
		//IM_LOG("Move");

		_float fSpeedControl = 0.f;

		if (m_bWalk)				fSpeedControl = TimeDelta;

		if (m_bCanRun)				fSpeedControl *= 2.f;

		m_pTransformCom->Move(fSpeedControl, m_vMoveDir);
	}
//	IM_LOG("%d", m_bCanMove);
	
	if (CPlayerInfoManager::GetInstance()->Get_Copy())
	{
		for (auto& iter : m_vecWeapon)
		{
			iter->Tick(TimeDelta);
			CPlayerInfoManager::GetInstance()->Set_PlayerAttackEnable(m_bAttackEnable);

			_bool bCol = false;

			if (iter == m_vecWeapon.at(0))
			{
				if (!bCol)
					bCol = Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART), ECOPYCOLTYPE::COPYCOL_MAIN);
				else
					Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART), ECOPYCOLTYPE::COPYCOL_MAIN);
			}
			else if (iter == m_vecWeapon.at(1))
			{
				if (!bCol)
					bCol = Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART), ECOPYCOLTYPE::COPYCOL_LEFT);
				else
					Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART), ECOPYCOLTYPE::COPYCOL_LEFT);
			}
			else if (iter == m_vecWeapon.at(2))
			{
				if (!bCol)
					bCol = Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART), ECOPYCOLTYPE::COPYCOL_RIGHT);
				else
					Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART), ECOPYCOLTYPE::COPYCOL_RIGHT);
			}

			if (bCol)
			{
				m_fKineticCombo_Slash = 5.f;
			}
		}

		for (auto& iter : m_vecSheath)
		{
			iter->Tick(TimeDelta);
		}
	}
	else
	{
		m_vecWeapon.front()->Tick(TimeDelta);

		_bool bCol = Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(m_vecWeapon.front())->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART), ECOPYCOLTYPE::COPYCOL_MAIN);
		CPlayerInfoManager::GetInstance()->Set_PlayerAttackEnable(m_bAttackEnable);

		if (bCol)
		{
			m_fKineticCombo_Slash = 5.f;
		}

		m_vecSheath.front()->Tick(TimeDelta);
	}

	EnemyReportCheck();

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	SocketLocalMoveCheck();

	if (pGameInstance->KeyDown(DIK_F1))
	{
		static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
	}

	CPlayerInfoManager::GetInstance()->Set_PlayerWorldMatrix(m_pTransformCom->Get_WorldMatrix());

	/*if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		physx::PxSweepHit hitBuffer[4];
		physx::PxSweepBuffer overlapOut(hitBuffer, 4);
		CapsuleSweepParams params2;
		params2.sweepOut = &overlapOut;
		params2.fRadius = 0.5f;
		params2.fHalfHeight = 2.f;
		params2.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		params2.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		params2.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		params2.fDistance = 3.f;
		params2.iTargetType = CTB_MONSTER;

		if (CGameInstance::GetInstance()->SweepCapsule(params2))
		{
			for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
			{
				auto pHit = overlapOut.getAnyHit(i);
				CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
				if (auto pMonster = dynamic_cast<CMonster*>(pCollidedObject))
				{
					DAMAGE_PARAM tParam;
					tParam.iDamage = 1;
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					pMonster->TakeDamage(tParam);
				}
			}
		}
	}*/

	//	//physx::PxRaycastHit hitBuffer[1];
	//	//physx::PxRaycastBuffer rayOut(hitBuffer, 1);

	//	//RayCastParams param;
	//	//param.rayOut = &rayOut;
	//	//param.vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//	//param.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//	//param.fDistance = 5.f;
	//	//param.iTargetType = CTB_MONSTER;
	//	//param.bSingle = true;
	//	//if (CGameInstance::GetInstance()->RayCast(param))
	//	//{
	//	//	for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
	//	//	{
	//	//		auto pHit = rayOut.getAnyHit(i);
	//	//		CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
	//	//		if (auto pMonster = dynamic_cast<CMonster*>(pCollidedObject))
	//	//		{
	//	//			DAMAGE_PARAM tParam;
	//	//			tParam.iDamage = 1;
	//	//			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//	//			pMonster->TakeDamage(tParam);
	//	//		}

	//	//	}
	//	//}
	//}

	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject() && m_bHit)
	{
		static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Dynamic();
	}

	m_pBattleChecker->Update_Tick(m_pTransformCom);
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);


	if (CPlayerInfoManager::GetInstance()->Get_Copy())
	{
		for (auto& iter : m_vecWeapon)
		{
			iter->Late_Tick(TimeDelta);
		}
		for (auto& iter : m_vecSheath)
		{
			iter->Late_Tick(TimeDelta);
		}
	}
	else
	{
		m_vecWeapon.front()->Late_Tick(TimeDelta);
		m_vecSheath.front()->Late_Tick(TimeDelta);
	}


	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->SetOutline(true);
	}

	if (m_bVisible && (nullptr != m_pRenderer))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
	}


	m_pSAS_Cable->Tick(TimeDelta);
	m_pSAS_Cable->Late_Tick(TimeDelta);
	m_pBrainField->Tick(TimeDelta);
	m_pBrainField->Late_Tick(TimeDelta);
}

void CPlayer::AfterPhysX()
{
	__super::AfterPhysX();

	m_pBattleChecker->Update_AfterPhysX(m_pTransformCom);

	if (CPlayerInfoManager::GetInstance()->Get_Copy())
	{
		_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		vRight.Normalize();
		_float4 vLeft = vRight * -1.f;
		vRight.w = 0.f;
		vLeft.w = 0.f;

		_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
		_matrix matLeft = matWorld;
		_matrix matRight = matWorld;
		matLeft.r[3] += vRight;
		matRight.r[3] += vLeft;

		static_cast<CWeapon_Player*>(m_vecWeapon.at(0))->Setup_BoneMatrix(m_pModel, matWorld, m_strWeaponAttachBone);
		static_cast<CScarletWeapon*>(m_vecWeapon.at(0))->Trail_Tick(m_fTimeDelta);
	
		static_cast<CWeapon_Player*>(m_vecWeapon.at(1))->Setup_BoneMatrix(m_pModel, matLeft, m_strWeaponAttachBone);
		static_cast<CScarletWeapon*>(m_vecWeapon.at(1))->Trail_Tick(m_fTimeDelta);

		static_cast<CWeapon_Player*>(m_vecWeapon.at(2))->Setup_BoneMatrix(m_pModel, matRight, m_strWeaponAttachBone);
		static_cast<CScarletWeapon*>(m_vecWeapon.at(2))->Trail_Tick(m_fTimeDelta);

		m_vecSheath.at(0)->Setup_BoneMatrix(m_pModel, matWorld);
		m_vecSheath.at(1)->Setup_BoneMatrix(m_pModel, matLeft);
		m_vecSheath.at(2)->Setup_BoneMatrix(m_pModel, matRight);
	}
	else
	{
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

		static_cast<CWeapon_Player*>(m_vecWeapon.at(0))->Setup_BoneMatrix(m_pModel, matWorld, m_strWeaponAttachBone);
		static_cast<CScarletWeapon*>(m_vecWeapon.at(0))->Trail_Tick(m_fTimeDelta);
		m_vecSheath.at(0)->Setup_BoneMatrix(m_pModel, matWorld);
	}

	m_pCamSpot->SetUp_BoneMatrix(m_pModel, m_pTransformCom->Get_WorldMatrix());
	// m_pRange->Update_Tick(m_pTransformCom);

	 //if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	 //{
	 //	list<CAnimation*> TestAnim;
	 //	TestAnim.push_back(m_pModel->Find_Animation("AS_DriveModeOpen_ch0100_ch0100"));
	 //	m_pASM->InputAnimSocket("Common_AnimSocket", TestAnim);
		//
	 //	auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("DriveModeCam");
	 //	m_pPlayer_AnimCam->StartCamAnim_Return(pCamAnim, m_pPlayerCam, m_pTransformCom->Get_WorldMatrix_f4x4(), m_fCameraLerpTime_In, m_fCameraLerpTime_Out);
	 //}
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	if (CPlayerInfoManager::GetInstance()->Get_Copy())
	{
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		vRight.Normalize();
		_float4 vLeftPos = vPos + (vRight * -1.f);
		_float4 vRightPos = vPos + vRight;

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLeftPos);
		m_pModel->Render(m_pTransformCom);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vRightPos);
		m_pModel->Render(m_pTransformCom);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	}

	return S_OK;
}

HRESULT CPlayer::Render_ShadowDepth()
{
	m_pModel->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CPlayer::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	// ?ÑÏû¨ ?†ÎãàÎ©îÏù¥???ÅÌÉú???∞Îùº ?Ä?§Ìä∏?∑Ï? ?¨Î? ?êÎã®
	if (m_pModel->GetPlayAnimation()->GetName().find("dodge") != string::npos &&
		m_pASM->isSocketExactlyEmpty() &&
		tDamageParams.eAttackType != EAttackType::ATK_END)
	{
		m_fJustDodgeAble = 1.f;
	}
	else if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().bBrainField && CPlayerInfoManager::GetInstance()->Get_BrainMap(EBRAINMAP::BRAINMAP_BRAINFIELD_HARDBODY))
	{
		IM_LOG("BrainField Hardbody");
	}
	else if (CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_HARDBODY))
	{
		// Í≤ΩÏßà?îÎäî ?ºÍ≤© ???∞Î?ÏßÄÎ•?Î¨¥Ïãú?òÍ≥† Í≤åÏù¥ÏßÄÎ•?10 ÍπéÎäî??
		IM_LOG("No Damage");
		CPlayerInfoManager::GetInstance()->Change_SasEnergy(CHANGETYPE::CHANGE_DECREASE, ESASType::SAS_HARDBODY, 10.f);
	}
	else if (m_bJustDodge_Activate || m_bKineticSpecial_Activate || m_bDriveMode_Activate || m_bBrainCrash || m_bBrainField_Prod)
	{
		IM_LOG("JustDodge Activate")
	}
	else if (m_eDeBuff == EDeBuffType::DEBUFF_THUNDER)
	{
		m_DamageDesc.m_iDamage = tDamageParams.iDamage;
		CPlayerInfoManager::GetInstance()->Change_PlayerHP(CHANGE_DECREASE, tDamageParams.iDamage);
	}
	else if (	m_pHitStateMachine->GetCurStateName() == "AIRBORNE" ||
				m_pHitStateMachine->GetCurStateName() == "KNUCKBACK" ||
				m_pHitStateMachine->GetCurStateName() == "FALLDOWN" ||
				m_pHitStateMachine->GetCurStateName() == "FALL" ||
				m_pHitStateMachine->GetCurStateName() == "BREAKFALL" ||
				m_pKineticComboStateMachine->GetCurStateName() == "KINETIC_COMBO_KINETIC04_THROW")
	{
		return;
	}
	else
	{
		if (tDamageParams.eAttackType != EAttackType::ATK_END && tDamageParams.eDeBuff != EDeBuffType::DEBUFF_THUNDER)
			m_bHit = true;

		m_DamageDesc.m_iDamage = tDamageParams.iDamage;
		m_DamageDesc.m_iDamageType = tDamageParams.eAttackType;
		m_DamageDesc.m_vHitDir = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - XMLoadFloat4(&tDamageParams.vHitFrom);
		m_DamageDesc.m_eHitDir = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);

		m_eDeBuff = tDamageParams.eDeBuff;

		// Ï≤¥Î†• ÍπéÏù¥??Î∂ÄÎ∂?
		CPlayerInfoManager::GetInstance()->Change_PlayerHP(CHANGE_DECREASE, tDamageParams.iDamage);

		if (tDamageParams.eAttackType == EAttackType::ATK_HEAVY || tDamageParams.eAttackType == EAttackType::ATK_TO_AIR)
		{
			if (CGameInstance::GetInstance()->Check_ObjectAlive(tDamageParams.pCauser))
				m_pTransformCom->LookAt_NonY(tDamageParams.pCauser->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
			else if (tDamageParams.vHitFrom.w != 0.f)
				m_pTransformCom->LookAt_NonY(tDamageParams.vHitFrom);
		}
	}
}

void CPlayer::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	m_pHotFixer->Tick();

	ImGui::InputFloat("BrainfieldRange", &m_fBF_Range);
	ImGui::InputFloat("BrainfieldHeight", &m_fBF_Height);
	ImGui::InputFloat("BrainfieldGatherPower", &m_fGatherPower);

	if (ImGui::CollapsingHeader("StateCheck"))
	{
		ImGui::Checkbox("CanTurn", &m_bCanTurn);
		ImGui::Checkbox("CanMove", &m_bCanMove);
		ImGui::Checkbox("CanRun", &m_bCanRun);
		ImGui::Checkbox("CanAttackTurn", &m_bCanTurn_Attack);
		ImGui::Checkbox("OnAir", &m_bAir);
		ImGui::Checkbox("OnGravity", &m_bActiveGravity);
	}

	if (ImGui::CollapsingHeader("ProductionTool"))
	{
		if (ImGui::Button("DriveMode_Start"))
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pModel->Find_Animation("AS_DriveModeOpen_ch0100_ch0100"));
			m_pASM->InputAnimSocket("Common_AnimSocket", TestAnim);

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("DriveModeCam");
			m_pPlayer_AnimCam->StartCamAnim_Return(pCamAnim, m_pPlayerCam, m_pTransformCom->Get_WorldMatrix_f4x4(), m_fCameraLerpTime_In, m_fCameraLerpTime_Out);
		}

		ImGui::Checkbox("ZoomMode", &m_bCamZoom);

		ImGui::SliderFloat("CamLerp_In", &m_fCameraLerpTime_In, 0.f, 5.f);
		ImGui::SliderFloat("CamLerp_Out", &m_fCameraLerpTime_Out, 0.f, 5.f);
	}

	if (ImGui::Button("Production_Clear"))
	{
		m_pModel->FindMaterial(L"MI_ch0100_HOOD_0")->SetActive(false);
		m_pModel->FindMaterial(L"MI_ch0100_BODY_0")->GetParam().Ints[1] = 0;
		m_pModel->FindMaterial(L"MI_ch0100_HAIR_0")->SetActive(true);
		m_pModel->FindMaterial(L"MI_ch0100_HAIR_1")->SetActive(true);
		m_bDriveMask = false;
	}

	ImGui::SliderFloat("ThrowPoint_X", &m_vIronBars_ThrowPoins.x, -5.f, 5.f);
	ImGui::SliderFloat("ThrowPoint_Y", &m_vIronBars_ThrowPoins.y, -5.f, 5.f);
	ImGui::SliderFloat("ThrowPoint_Z", &m_vIronBars_ThrowPoins.z, -5.f, 5.f);

	ImGui::InputFloat("ThrowPower", &m_fThrowPower);
	ImGui::InputFloat("ChargePower", &m_fChargePower);

	if (ImGui::CollapsingHeader("pivot2"))
	{
		static GUIZMO_INFO tp2;
		CImguiUtils::Render_Guizmo(&pivot2, tp2, true, true);
	}
	if (ImGui::CollapsingHeader("pivot3"))
	{
		static GUIZMO_INFO tp3;
		CImguiUtils::Render_Guizmo(&pivot3, tp3, true, true);
	}
	if (ImGui::CollapsingHeader("pivot4"))
	{
		static GUIZMO_INFO tp4;
		CImguiUtils::Render_Guizmo(&pivot4, tp4, true, true);
	}
	if (ImGui::CollapsingHeader("pivot5"))
	{
		static GUIZMO_INFO tp5;
		CImguiUtils::Render_Guizmo(&pivot5, tp5, true, true);
	}


	if (ImGui::CollapsingHeader("SAS_Cable"))
	{
		ImGui::Indent(20.f);
		m_pSAS_Cable->Imgui_RenderProperty();
		m_pSAS_Cable->Imgui_RenderComponentProperties();
		ImGui::Unindent(20.f);
	}
	if (ImGui::CollapsingHeader("BrainFieldCable"))
	{
		ImGui::Indent(20.f);
		m_pBrainField->Imgui_RenderProperty();
		m_pBrainField->Imgui_RenderComponentProperties();
		ImGui::Unindent(20.f);
	}

	// HP Bar Check	
	ImGui::Text("HP : %d", CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP);
	ImGui::Text(m_pModel->GetPlayAnimation()->GetName().c_str());

	m_pHitStateMachine->Imgui_RenderProperty();
	//m_pKineticStataMachine->Imgui_RenderProperty();
	//m_pTrainStateMachine_Left->Imgui_RenderProperty();
	//m_pBrainCrashStateMachine->Imgui_RenderProperty();
	//m_pTeleportStateMachine->Imgui_RenderProperty();


	ImGui::SliderFloat("PlayerTurnSpeed", &m_fTurnSpeed, 0.f, 1000.f);
	if (ImGui::Button("TurnAccess"))
	{
		m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(m_fTurnSpeed) });
	}

	ImGui::SliderFloat("JumpPower", &m_fJumpPower, 0.f, 100.f);

	if (ImGui::CollapsingHeader("Weapons"))
	{
		ImGui::Indent(20.f);
		if (m_vecWeapon.empty() == false)
		{
			m_vecWeapon.front()->Imgui_RenderProperty();
			m_vecWeapon.front()->Imgui_RenderComponentProperties();
		}
		ImGui::Unindent(20.f);
	}

	m_pKineticStataMachine->Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Weapon"))
	{
		m_vecWeapon.front()->Imgui_RenderComponentProperties();
	}

	if (ImGui::CollapsingHeader("Spline_Test"))
	{
		ImGui::InputFloat4("v01", &m_vSplinePoint_01.x);
		ImGui::InputFloat4("v02", &m_vSplinePoint_02.x);
		ImGui::InputFloat4("v03", &m_vSplinePoint_03.x);
		ImGui::InputFloat4("v04", &m_vSplinePoint_04.x);
	}

	if (ImGui::CollapsingHeader("Effect_Attach"))
	{
		static string szAnimName = "";
		static _float fEffectPlayTime = 0.f;

		static _float fPrePlayTime = 0.f;
		_float fCurPlayTime = m_pModel->GetPlayAnimation()->GetPlayTime();

		static char szSeachAnim[MAX_PATH] = "";
		ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

		if (ImGui::BeginListBox("Animation List"))
		{
			const string strSearch = szSeachAnim;
			const _bool bSearch = strSearch.empty() == false;

			for (auto& Pair : m_pModel->Get_AnimList())
			{
				if (bSearch)
				{
					if (Pair.first.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = szAnimName == Pair.first;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(Pair.first.c_str(), bSelected))
					szAnimName = Pair.first;
			}
			ImGui::EndListBox();
		}

		// ?§Ïßà?ÅÏù∏ ?¥Ìéô??Î∂ôÏù¥Í∏?
		static char cEffectName[MAX_PATH]{};
		static string szEffectName = "";
		ImGui::InputText("Effect Search", cEffectName, MAX_PATH);
		szEffectName = cEffectName;

		static char cBoneName[MAX_PATH]{};
		static string szBoneName = "";
		ImGui::InputText("Bone Search", cBoneName, MAX_PATH);
		szBoneName = cBoneName;

		ImGui::InputFloat("EffectTime", &fEffectPlayTime);

		ImGui::Checkbox("EffectUpdate", &m_bEffectUpdate);

		static GUIZMO_INFO tp1;
		CImguiUtils::Render_Guizmo(&pivot1, tp1, true, true);

		if ((m_pModel->GetPlayAnimation()->GetName() == szAnimName) &&
			(fPrePlayTime <= fEffectPlayTime) &&
			(fCurPlayTime > fEffectPlayTime))
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, s2ws(szEffectName), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, pivot1, szBoneName, true, m_bEffectUpdate);
		}

		fPrePlayTime = fCurPlayTime;

		if("" != szAnimName)
			m_pModel->Get_AnimList()[szAnimName]->Imgui_RenderProperty();
	}
}

void CPlayer::Update_DeBuff(_double TimeDelta)
{
	CScarletCharacter::Update_DeBuff(TimeDelta);

	m_pDeBuffStateMachine->Tick(TimeDelta);

	//if (m_eDeBuff == EDeBuffType::DEBUFF_FIRE)
	//{
		/*m_FireTick.Tick(TimeDelta);
		if (m_FireTick.Use())
		{
			DAMAGE_PARAM tParam;
			tParam.iDamage = _uint((_float)m_iMaxHP / 100.f * (1.f - m_fFireResist));
			tParam.pCauser = this;
			TakeDamage(tParam);
		}*/
	//}
}

void CPlayer::DeBuff_End()
{
	Update_NoticeNeon();
	
	for (auto pMtrl : m_pModel->GetMaterials())
	{
		if (pMtrl->GetParam().Ints.empty())
		{
			pMtrl->GetParam().Ints.push_back(0);
		}

		pMtrl->GetParam().Ints[0] = 0;
	}
}

void CPlayer::DeBuff_Fire()
{
	m_fDeBuffTime = 8.f;
	Update_NoticeNeon();

	for (auto pMtrl : m_pModel->GetMaterials())
	{
		if (pMtrl->GetParam().Ints.empty())
		{
			pMtrl->GetParam().Ints.push_back(0);
		}

		pMtrl->GetParam().Ints[0] = 1;
	}
}

void CPlayer::DeBuff_Oil()
{
	m_fDeBuffTime = 8.f;
	Update_NoticeNeon();

	for (auto pMtrl : m_pModel->GetMaterials())
	{
		if (pMtrl->GetParam().Ints.empty())
		{
			pMtrl->GetParam().Ints.push_back(0);
		}

		pMtrl->GetParam().Ints[0] = 2;
	}
}

void CPlayer::DeBuff_Thunder()
{
	m_fDeBuffTime = 10.f;
	Update_NoticeNeon();
}

void CPlayer::DeBuff_Water()
{
	m_fDeBuffTime = 10.f;
	Update_NoticeNeon();
}

//CGameObject * CPlayer::Get_TargetedEnemy()
//{
//	if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
//	{
//		return nullptr;
//	}
//
//	return m_pTargetedEnemy;
//}

void CPlayer::CamBoneTest()
{
	//if (CGameInstance::GetInstance()->KeyDown(DIK_7))
	//{
	//	list<CAnimation*>	TestList;
	//	TestList.push_back(m_pModel->Find_Animation("AS_ch0100_271_AL_Pcon_cReL_Lv4"));

	//	m_pASM->InputAnimSocket("JustDodge_AnimSocket", TestList);

	//	m_pCamSpot->Switch_CamMod();
	//}
	//else if (CGameInstance::GetInstance()->KeyDown(DIK_8))
	//{
	//	m_pCamSpot->Switch_CamMod();
	//}
}

void CPlayer::SasMgr()
{
	ESASType InputSas = ESASType::SAS_END;

	if (CGameInstance::GetInstance()->KeyDown(DIK_1))
	{
		InputSas = ESASType::SAS_TELEPORT;
		m_bSASSkillInput[0] = true;
	}
	else if (CGameInstance::GetInstance()->KeyDown(DIK_2))
	{
		InputSas = ESASType::SAS_PENETRATE;
		m_bSASSkillInput[1] = true;
	}
	else if (CGameInstance::GetInstance()->KeyDown(DIK_3))
	{
		InputSas = ESASType::SAS_HARDBODY;
		m_bSASSkillInput[2] = true;

	}
	else if (CGameInstance::GetInstance()->KeyDown(DIK_4))
	{
		InputSas = ESASType::SAS_FIRE;
		m_bSASSkillInput[3] = true;

	}
	else if (CGameInstance::GetInstance()->KeyDown(DIK_5)) 
	{
		InputSas = ESASType::SAS_SUPERSPEED;
		m_bSASSkillInput[4] = true;
	}
	else if (CGameInstance::GetInstance()->KeyDown(DIK_6))
	{
		InputSas = ESASType::SAS_ELETRIC;
		m_bSASSkillInput[5] = true;
	}
	else if (CGameInstance::GetInstance()->KeyDown(DIK_7))
	{
		InputSas = ESASType::SAS_COPY;
		m_bSASSkillInput[6] = true;
	}

	if (InputSas != ESASType::SAS_END)
	{
		_bool bSasIsUsing = false;
		for (auto& SAS : CPlayerInfoManager::GetInstance()->Get_PlayerSasList())
		{
			if (InputSas == SAS) { bSasIsUsing = true; break; }
		}

		if (false == CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[static_cast<_uint>(InputSas)].bUsable &&
			false == bSasIsUsing)
		{
			IM_LOG("Sas_Unusable");
		}
		else
		{
			if (bSasIsUsing)
			{
				CPlayerInfoManager::GetInstance()->Finish_SasType(InputSas);

				if (ESASType::SAS_FIRE == InputSas)
				{
				}
				else if (ESASType::SAS_TELEPORT == InputSas)
				{
				}
				else if (ESASType::SAS_ELETRIC == InputSas)
				{
				}
				else if (ESASType::SAS_PENETRATE == InputSas)
				{
				}
			}
			else
			{
				SasOn.Reset();

				SasGearEffect();

				if (ESASType::SAS_FIRE == InputSas)
				{
					m_pSasPortrait->Start_SAS(InputSas);
					m_pSAS_Cable->EquipCable(ESASType::SAS_FIRE);
				}
				else if (ESASType::SAS_TELEPORT == InputSas)
				{
					m_pSasPortrait->Start_SAS(InputSas);
					m_pSAS_Cable->EquipCable(ESASType::SAS_TELEPORT);
				}
				else if (ESASType::SAS_ELETRIC == InputSas)
				{
					m_pSasPortrait->Start_SAS(InputSas);
					m_pSAS_Cable->EquipCable(ESASType::SAS_ELETRIC);
				}
				else if (ESASType::SAS_PENETRATE == InputSas)
				{
					m_pSasPortrait->Start_SAS(InputSas);
					m_pSAS_Cable->EquipCable(ESASType::SAS_PENETRATE);
				}
				else if (ESASType::SAS_HARDBODY == InputSas)
				{
					m_pSasPortrait->Start_SAS(InputSas);
					m_pSAS_Cable->EquipCable(ESASType::SAS_HARDBODY);
				}
				else if (ESASType::SAS_SUPERSPEED == InputSas)
				{
					m_pSasPortrait->Start_SAS(InputSas);
					m_pSAS_Cable->EquipCable(ESASType::SAS_SUPERSPEED);
				}
				else if (ESASType::SAS_COPY == InputSas)
				{
					m_pSasPortrait->Start_SAS(InputSas);
					m_pSAS_Cable->EquipCable(ESASType::SAS_COPY);
				}

				CPlayerInfoManager::GetInstance()->Set_SasType(InputSas);
			}
		}
	}

	if (m_pSAS_Cable->GetIsActive())
	{
		if (SasOn.IsNotDo())
		{
			// Í≥µÌÜµ
			_matrix MatParticle = XMMatrixRotationX(XMConvertToRadians(80.f));
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, TEXT("Sas_Docking_Finished"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatParticle, "Sheath", true);

			if (ESASType::SAS_FIRE == CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
				if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSwordParticle_Elec))
				{
					m_pSwordParticle_Elec->SetDelete();
					m_pSwordParticle_Elec = nullptr;
				}

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("SAS_FIRE"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
				m_pSwordParticle_Fire = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Weapon_Particle"), LAYER_PLAYEREFFECT);
				_matrix matPivot = XMMatrixRotationY(XMConvertToRadians(180.f));
				m_pSwordParticle_Fire->Start_AttachPivot(this, matPivot, "RightWeapon", true, true);
			}

			if (ESASType::SAS_PENETRATE== CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("SAS_PENETRATE"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
				m_pSAS_Penetrate->Active(true);
			}

			if (ESASType::SAS_SUPERSPEED == CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("SAS_SUPERSPEED"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");

				const vector<wstring> except = { 
					LAYER_SAS, 
					PLAYERTEST_LAYER_CAMERA, 
					PLAYERTEST_LAYER_FRONTUI, 
					LAYER_PLAYEREFFECT, 
					PLATERTEST_LAYER_PLAYER,
					LAYER_KINETIC,
					LAYER_MAPKINETICOBJECT
				};
				CGameInstance::GetInstance()->SetTimeRatio(0.3f, &except);

				m_pTrail->SetActive(true);
				m_pSuperSpeedPostVFX->Active(true);
			}

			if (ESASType::SAS_TELEPORT == CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("SAS_TELEPORT"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			}

			if (ESASType::SAS_HARDBODY == CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("SAS_HARDBODY"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			}

			if (ESASType::SAS_ELETRIC == CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
				if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSwordParticle_Fire))
				{
					m_pSwordParticle_Fire->SetDelete();
					m_pSwordParticle_Fire = nullptr;
				}
			}

			if (ESASType::SAS_COPY == CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
				//CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("SAS_HARDBODY"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
				CPlayerInfoManager::GetInstance()->Set_Copy(true);
			}
		}
	}

	SasStateCheck();

	Visible_Check();
}

void CPlayer::Visible_Check()
{
	if (m_pModel->GetPlayAnimation() == nullptr) return;
	if (m_pTeleportStateMachine->GetCurStateName() != "TELEPORTATTACK_NOUSE") return;

	if (m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_051_AL_sas_dodge_F_start_Telepo" ||
		m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_053_AL_sas_dodge_R_start_Telepo" ||
		m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_055_AL_sas_dodge_B_start_Telepo" ||
		m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_057_AL_sas_dodge_L_start_Telepo" ||
		m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_151_AL_sas_dodge_F_start_Telepo" ||
		m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_153_AL_sas_dodge_R_start_Telepo" ||
		m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_155_AL_sas_dodge_B_start_Telepo" ||
		m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_157_AL_sas_dodge_L_start_Telepo")
	{
		m_bTeleport = true;
	}
	else
	{
		m_bTeleport = false;
	}

	if (true == m_bTeleport)
	{
		m_pCollider->SetFilterData(ECOLLIDER_TYPE_BIT(CTB_PLAYER | CTB_MONSTER | CTB_PSYCHICK_OBJ | CTB_STATIC | CTB_MONSTER_PART | CTB_MONSTER_RANGE));

		if (1.f > m_fTeleportDissolve)
		{
			m_fTeleportDissolve = min(m_fTeleportDissolve + ((_float)m_fTimeDelta * 10.f), 1.f);

			for (auto pMtrl : m_pModel->GetMaterials())
			{
				pMtrl->GetParam().Floats[2] = m_fTeleportDissolve;
			}

			m_pSAS_Cable->CableTeleportDissolve(m_fTeleportDissolve);

			if (1.f == m_fTeleportDissolve)
			{
				TeleportEffectMaker();
			}
		}
	}
	else
	{
		m_pCollider->SetFilterData(ECOLLIDER_TYPE_BIT(CTB_PLAYER | CTB_MONSTER | CTB_PSYCHICK_OBJ | CTB_STATIC | CTB_MONSTER_PART | CTB_MONSTER_RANGE | CTB_TELEPORT_OBJ));

		if (0.f < m_fTeleportDissolve)
		{
			m_fTeleportDissolve = max(m_fTeleportDissolve - ((_float)m_fTimeDelta * 10.f), 0.f);

			for (auto pMtrl : m_pModel->GetMaterials())
			{
				pMtrl->GetParam().Floats[2] = m_fTeleportDissolve;
			}

			m_pSAS_Cable->CableTeleportDissolve(m_fTeleportDissolve);

			if (0.f == m_fTeleportDissolve)
			{
				TeleportEffectMaker();
			}
		}
	}
}

void CPlayer::SasStateCheck()
{

	if (false == CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_FIRE))
	{
		if (m_bBeforeSAS_Using[(_uint)ESASType::SAS_FIRE])
		{
			SasGearReleaseEffect();
			_matrix MatEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Dead_Sas_Effect_Curve"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatEffect, "Sheath", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Sas_Dead_Light"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			_matrix MatParticle = XMMatrixRotationX(XMConvertToRadians(80.f));
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, TEXT("Sas_Dead_Particles"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatParticle, "Sheath", true);
		}

		m_bSASSkillInput[3] = false;
		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSwordParticle_Fire))
		{
			m_pSwordParticle_Fire->SetDelete();
			m_pSwordParticle_Fire = nullptr;
		}
	}

	if (false == CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_PENETRATE))
	{
		if (m_bBeforeSAS_Using[(_uint)ESASType::SAS_PENETRATE])
		{
			SasGearReleaseEffect();
			_matrix MatEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Dead_Sas_Effect_Curve"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatEffect, "Sheath", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Sas_Dead_Light"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			_matrix MatParticle = XMMatrixRotationX(XMConvertToRadians(80.f));
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, TEXT("Sas_Dead_Particles"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatParticle, "Sheath", true);
		}

		m_bSASSkillInput[1] = false;
		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSAS_Penetrate)) m_pSAS_Penetrate->Active(false);
	}

	if (false == CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_TELEPORT))
	{
		if (m_bBeforeSAS_Using[(_uint)ESASType::SAS_TELEPORT])
		{
			SasGearReleaseEffect();
			_matrix MatEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Dead_Sas_Effect_Curve"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatEffect, "Sheath", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Sas_Dead_Light"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			_matrix MatParticle = XMMatrixRotationX(XMConvertToRadians(80.f));
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, TEXT("Sas_Dead_Particles"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatParticle, "Sheath", true);
		}

		m_bSASSkillInput[0] = false;
		m_pTeleportStateMachine->SetState("TELEPORTATTACK_NOUSE");
	}

	if (false == CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_HARDBODY))
	{
		if (m_bBeforeSAS_Using[(_uint)ESASType::SAS_HARDBODY])
		{
			SasGearReleaseEffect();
			_matrix MatEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Dead_Sas_Effect_Curve"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatEffect, "Sheath", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Sas_Dead_Light"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			_matrix MatParticle = XMMatrixRotationX(XMConvertToRadians(80.f));
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, TEXT("Sas_Dead_Particles"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatParticle, "Sheath", true);
		}

		m_bSASSkillInput[2] = false;

		if (0.f <= m_fHardbodyDissolve)
		{
			m_fHardbodyDissolve = max(m_fHardbodyDissolve - ((_float)m_fTimeDelta * 2.f), 0.f);

			for (auto pMtrl : m_pModel->GetMaterials())
			{
				pMtrl->GetParam().Floats[1] = m_fHardbodyDissolve;
			}
		}
	}
	else
	{
		if (1.f >= m_fHardbodyDissolve)
		{
			m_fHardbodyDissolve = min(m_fHardbodyDissolve + ((_float)m_fTimeDelta * 2.f), 1.f);

			for (auto pMtrl : m_pModel->GetMaterials())
			{
				pMtrl->GetParam().Floats[1] = m_fHardbodyDissolve;
			}
		}
	}

	if (false == CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_SUPERSPEED))
	{
		if (m_bBeforeSAS_Using[(_uint)ESASType::SAS_SUPERSPEED])
		{
			SasGearReleaseEffect();
			_matrix MatEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Dead_Sas_Effect_Curve"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatEffect, "Sheath", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Sas_Dead_Light"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			_matrix MatParticle = XMMatrixRotationX(XMConvertToRadians(80.f));
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, TEXT("Sas_Dead_Particles"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatParticle, "Sheath", true);
		}

		m_bSASSkillInput[4] = false;

		CGameInstance::GetInstance()->ResetDefaultTimeRatio();
		m_pSuperSpeedPostVFX->Active(false);
		m_pTrail->SetActive(false);
	}

	if (false == CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_ELETRIC))
	{
		if (m_bBeforeSAS_Using[(_uint)ESASType::SAS_ELETRIC])
		{
			SasGearReleaseEffect();
			_matrix MatEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Dead_Sas_Effect_Curve"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatEffect, "Sheath", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Sas_Dead_Light"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			_matrix MatParticle = XMMatrixRotationX(XMConvertToRadians(80.f));
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, TEXT("Sas_Dead_Particles"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatParticle, "Sheath", true);
		}

		m_bSASSkillInput[5] = false;
		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSwordParticle_Elec))
		{
			m_pSwordParticle_Elec->SetDelete();
			m_pSwordParticle_Elec = nullptr;
		}
	}

	if (false == CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_COPY))
	{
		if (m_bBeforeSAS_Using[(_uint)ESASType::SAS_COPY])
		{
			SasGearReleaseEffect();
			_matrix MatEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Dead_Sas_Effect_Curve"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatEffect, "Sheath", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, TEXT("Sas_Dead_Light"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath");
			_matrix MatParticle = XMMatrixRotationX(XMConvertToRadians(80.f));
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, TEXT("Sas_Dead_Particles"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, MatParticle, "Sheath", true);
		}

		m_bSASSkillInput[6] = false;

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			CPlayerInfoManager::GetInstance()->Set_Copy(false);
		}
	}

	if (CPlayerInfoManager::GetInstance()->Get_PlayerSasList().empty())
	{
		m_pSAS_Cable->UnEquipCable();
	}

	for (_uint i = 0; i < SAS_CNT; ++i)
	{
		m_bBeforeSAS_Using[i] = CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType(i));
	}
}

void CPlayer::ElecSweep()
{
	physx::PxSweepHit hitBuffer[4];
	physx::PxSweepBuffer overlapOut(hitBuffer, 4);
	SphereSweepParams params2;
	params2.sweepOut = &overlapOut;
	params2.fRadius = 2.f;
	params2.vPos = GetColliderPosition();
	params2.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	params2.fDistance = 20.f;
	params2.iTargetType = CTB_MONSTER | CTB_MONSTER_PART;
	params2.fVisibleTime = 1.f;

	if (CGameInstance::GetInstance()->SweepSphere(params2))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pMonster = dynamic_cast<CEnemy*>(pCollidedObject))
			{
				// ?§Ïúï Ï∂©Îèå ???àÏù¥Ï∫êÏä§??Ï∂©ÎèåÎ°??¨Îûò?úÎ≤°???òÍ?
				_vector vRayPos = GetColliderPosition();
				vRayPos = XMVectorSetW(vRayPos, 1.f);
				_vector vRayDir = static_cast<CScarletCharacter*>(pMonster)->GetColliderPosition() - GetColliderPosition();
				vRayDir = XMVectorSetW(vRayDir, 0.f);

				physx::PxRaycastHit hitBuffer[1];
				physx::PxRaycastBuffer rayOut(hitBuffer, 1);

				RayCastParams param;
				param.rayOut = &rayOut;
				param.vOrigin = vRayPos;
				param.vDir = vRayDir;
				param.fDistance = 20.f;
				param.iTargetType = CTB_MONSTER | CTB_MONSTER_PART | CTB_MONSTER_RANGE;
				param.fVisibleTime = 5.f;
				param.bSingle = true;
				if (CGameInstance::GetInstance()->RayCast(param))
				{
					for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
					{
						auto pHit = rayOut.getAnyHit(i);
						CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
						if (auto pEnemy = dynamic_cast<CEnemy*>(pCollidedObject))
						{
							DAMAGE_PARAM tParam;
							ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
							tParam.pCauser = this;
							tParam.eAttackSAS = ESASType::SAS_ELETRIC;
							tParam.eAttackType = EAttackType::ATK_LIGHT;
							tParam.eDeBuff = EDeBuffType::DEBUFF_THUNDER;
							tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_END;
							tParam.iDamage = 100;
							tParam.vHitFrom = GetColliderPosition();
							tParam.vHitPosition = { pHit.position.x, pHit.position.y, pHit.position.z, 1.f };
							tParam.vSlashVector = static_cast<CScarletWeapon*>(m_vecWeapon.front())->Get_SlashVector();
							pMonster->TakeDamage(tParam);
						}
					}
				}
			}
		}
	}
}

void CPlayer::HitLack()
{
	if (m_pASM->GetCurSocketAnimName() == "AS_ch0100_207_AL_atk_justdodge")
	{
		CGameInstance::GetInstance()->SetTimeRatioCurve("Simple_Increase");
	}

	if (m_pASM->GetCurStateName() == "ATK_A3")
	{
		CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack");
	}

	if (m_pASM->GetCurStateName() == "ATK_A5")
	{
		CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack");
	}
}

void CPlayer::CamZoom(_double TimeDelta)
{
	if (m_bCamZoom)
	{
		m_bZoomIsFinish = static_cast<CCamSpot*>(m_pCamSpot)->Cam_Closer(TimeDelta, m_fCameraLerpTime_In);
	}
	else
	{
		m_bZoomIsFinish = static_cast<CCamSpot*>(m_pCamSpot)->Cam_Away(TimeDelta, m_fCameraLerpTime_Out);
	}
}

void CPlayer::SetOutLine(_bool bOutLine)
{
	if (bOutLine)
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().bBrainField)
		{
			for (auto& iter : m_pModel->GetMaterials())
			{
				iter->GetParam().Float4s[0] = { 1.f, 1.f, 0.01f, 1.f };
			}
		}
		else
		{
			for (auto& iter : m_pModel->GetMaterials())
			{
				iter->GetParam().Float4s[0] = { 0.945098f, 0.4f, 1.f, 1.f };
			}
		}
	}
	else if (false == CPlayerInfoManager::GetInstance()->Get_PlayerStat().bDriveMode)
	{
		for (auto& iter : m_pModel->GetMaterials())
		{
			iter->GetParam().Float4s[0] = { 0.f, 0.f, 0.f, 1.f };
		}
	}
}

HRESULT CPlayer::SetUp_DriveModeProductionStateMachine()
{
	m_pDriveModeProductionStateMachine = 
		CFSMComponentBuilder().InitState("DRIVEMODE_NOUSE")
		.AddState("DRIVEMODE_NOUSE")
		.OnStart([&]()
		{
			m_bZoomIsFinish = false;
		})
		.Tick([&](double fTimeDelta)
		{
			m_bDriveMode_Activate = false;

			if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().bDriveMode && (0.f == CPlayerInfoManager::GetInstance()->Get_PlayerStat().fDriveEnergy))
			{
				CPlayerInfoManager::GetInstance()->Set_DriveMode(false);

				m_pModel->FindMaterial(L"MI_ch0100_HOOD_0")->SetActive(false);
				m_pModel->FindMaterial(L"MI_ch0100_BODY_0")->GetParam().Ints[1] = 0;
				m_pModel->FindMaterial(L"MI_ch0100_HAIR_0")->SetActive(true);
				m_pModel->FindMaterial(L"MI_ch0100_HAIR_1")->SetActive(true);
				m_bDriveMask = false;
			}
			else if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().bDriveMode)
			{
				CPlayerInfoManager::GetInstance()->Change_DriveEnergy(CHANGE_DECREASE, fTimeDelta);
			}

			//IM_LOG("DRIVE : %f", CPlayerInfoManager::GetInstance()->Get_PlayerStat().fDriveEnergy);
		})
		.OnExit([&]()
		{

		})
			.AddTransition("DRIVEMODE_NOUSE to DRIVEMODE_CAM_CLOSER", "DRIVEMODE_CAM_CLOSER")
			.Predicator([&]()->_bool {return m_bDirveModeStart; })
			.Priority(0)

		.AddState("DRIVEMODE_CAM_CLOSER")
		.OnStart([&]()
		{
			m_bDriveMode_Activate = true;
			m_bZoomIsFinish = false;
		})
		.Tick([&](double fTimeDelta)
		{
			m_bZoomIsFinish = static_cast<CCamSpot*>(m_pCamSpot)->Cam_Closer(fTimeDelta, 0.3f);
		})
		.OnExit([&]()
		{

		})
			.AddTransition("DRIVEMODE_CAM_CLOSER to DRIVEMODE_ANIMCAM_START", "DRIVEMODE_ANIMCAM_START")
			.Predicator([&]()->_bool {return m_bZoomIsFinish; })
			.Priority(0)

		.AddState("DRIVEMODE_ANIMCAM_START")
		.OnStart([&]()
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pModel->Find_Animation("AS_DriveModeOpen_ch0100_ch0100"));
			m_pASM->InputAnimSocket("Common_AnimSocket", TestAnim);

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("DriveModeCam");
			m_pPlayer_AnimCam->StartCamAnim_Return(pCamAnim, m_pPlayerCam, m_pTransformCom->Get_WorldMatrix_f4x4(), m_fCameraLerpTime_In, m_fCameraLerpTime_Out);
		})
		.Tick([&](double fTimeDelta)
		{

		})
		.OnExit([&]()
		{
			CPlayerInfoManager::GetInstance()->Set_DriveMode(true);
			CPlayerInfoManager::GetInstance()->Set_DriveEnergy(10.f);
		})
			.AddTransition("DRIVEMODE_ANIMCAM_START to DRIVEMODE_CAM_AWAY", "DRIVEMODE_CAM_AWAY")
			.Predicator([&]()->_bool {return m_pModel->Find_Animation("AS_DriveModeOpen_ch0100_ch0100")->IsFinished(); })
			.Priority(0)

		.AddState("DRIVEMODE_CAM_AWAY")
		.OnStart([&]()
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pModel->Find_Animation("AS_ch0100_299_AL_enpc_drive_mode"));
			m_pASM->InputAnimSocket("Common_AnimSocket", TestAnim);

			m_bZoomIsFinish = false;
		})
		.Tick([&](double fTimeDelta)
		{
			m_bZoomIsFinish = static_cast<CCamSpot*>(m_pCamSpot)->Cam_Away(fTimeDelta, 0.f);
		})
		.OnExit([&]()
		{
		})
			.AddTransition("DRIVEMODE_CAM_AWAY to DRIVEMODE_NOUSE", "DRIVEMODE_NOUSE")
			.Predicator([&]()->_bool {return m_bZoomIsFinish; })
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_BrainFieldProductionStateMachine()
{
	m_pBrainFieldProductStateMachine =
		CFSMComponentBuilder().InitState("BRAINFIELD")

		.AddState("BRAINFIELD")
		.OnStart([&]() 
		{ 
			m_bZoomIsFinish = false;
			m_pASM->ClearAnimSocket("Common_AnimSocket");
		})
		.Tick([&](double fTimeDelta) 
		{
			m_bBrainField_Prod = false;

			if (m_bBrainField)
			{
				CPlayerInfoManager::GetInstance()->Change_BrainFieldMaintain(CHANGE_DECREASE, (_float)fTimeDelta);
			}
		})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD to BRAINFIELD_START", "BRAINFIELD_START")
			.Predicator([&]()->_bool { return (!m_bBrainField) && (m_bBrainFieldStart); })
			.Priority(0)

			.AddTransition("BRAINFIELD to BRAINFIELD_FINISH_BF", "BRAINFIELD_FINISH_BF")
			.Predicator([&]()->_bool { return (m_bBrainField) && (m_bBrainFieldStart || 0.f >= CPlayerInfoManager::GetInstance()->Get_PlayerStat().fBrainFieldMaintain); })
			.Priority(0)

		.AddState("BRAINFIELD_START")
		.OnStart([&]() 
		{
			m_bBrainField_Prod = true;

			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pModel->Find_Animation("AS_ch0100_BrainField_start"));
			m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			for (_uint i = 0; i < SAS_CNT; ++i)
			{
				CPlayerInfoManager::GetInstance()->Finish_SasType((ESASType)i);
			}
			SasStateCheck();
			Visible_Check();
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_START to BRAINFIELD_CAM_CLOSER", "BRAINFIELD_CAM_CLOSER")
			.Predicator([&]()->_bool { return (0.9f <= m_pModel->Find_Animation("AS_ch0100_BrainField_start")->GetPlayRatio()); })
			.Priority(0)

		.AddState("BRAINFIELD_CAM_CLOSER")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) { m_bZoomIsFinish = static_cast<CCamSpot*>(m_pCamSpot)->Cam_Closer(fTimeDelta, 0.3f); })
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_CAM_CLOSER to BRAINFIELD_ACTIONCAM_01", "BRAINFIELD_ACTIONCAM_01")
			.Predicator([&]()->_bool { return m_bZoomIsFinish; })
			.Priority(0)

		.AddState("BRAINFIELD_ACTIONCAM_01")
		.OnStart([&]() 
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pModel->Find_Animation("AS_BrainFieldOpen_c01_ch0100"));
			m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(false);

			m_pBrainField->OpenBrainField();

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("BrainField_01");
			m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayerCam, m_pTransformCom, m_fCameraLerpTime_In, m_fCameraLerpTime_Out);
			// m_pBrainField->CloseBrainField();  ≤Ù¥¬∞≈
		})
		.Tick([&](double fTimeDelta) 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Cam_Away(fTimeDelta, 0.3f);

			if (m_pModel->Find_Animation("AS_BrainFieldOpen_c01_ch0100")->GetPlayRatio() >= 0.8f && BF_Start.IsNotDo())
			{
				CPlayerInfoManager::GetInstance()->Set_BrainFieldMaintain(70.f);
				m_bBrainField = true;
				CPlayerInfoManager::GetInstance()->Set_BrainField(m_bBrainField);
			}

		})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_ACTIONCAM_01 to BRAINFIELD_ACTIONCAM_02", "BRAINFIELD_ACTIONCAM_02")
			.Predicator([&]()->_bool { return m_pModel->Find_Animation("AS_BrainFieldOpen_c01_ch0100")->IsFinished(); })
			.Priority(0)

		.AddState("BRAINFIELD_ACTIONCAM_02")
		.OnStart([&]() 
		{
			BF_Start.Reset();
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pModel->Find_Animation("AS_BrainFieldOpen_c02_ch0100"));
			m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			CMap_KineticBatchPreset::GetInstance()->Set_RespawnTime(1.f);

			// ∫Í∑π¿Œ« µÂ ≈∞∏È ≈∞≥◊∆Ω ø¿∫Í¡ß∆ÆµÈ «œ¥√∑Œ ∂∞ø¿∏ß
			for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
			{
				if (false == static_cast<CMapKinetic_Object*>(iter)->GetThrow() && true == static_cast<CMapKinetic_Object*>(iter)->Usable())
				{
					static_cast<CMapKinetic_Object*>(iter)->SetRefloat();
				}
			}
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() 
		{
		})
			.AddTransition("BRAINFIELD_ACTIONCAM_02 to BRAINFIELD", "BRAINFIELD")
			.Predicator([&]()->_bool { return m_pModel->Find_Animation("AS_BrainFieldOpen_c02_ch0100")->IsFinished(); })
			.Priority(0)

		.AddState("BRAINFIELD_FINISH_BF")
		.OnStart([&]() 
		{
			m_bBrainField_Prod = true;

			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF"));
			m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			CMap_KineticBatchPreset::GetInstance()->Set_RespawnTime(5.f);
		})
		.Tick([&](double fTimeDelta) 
		{
			if (m_pASM->GetSocketAnimation("Common_AnimSocket") != m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF"))
			{
				list<CAnimation*> TestAnim;
				TestAnim.push_back(m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF"));
				m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);
			}
		})
		.OnExit([&]() { m_bBrainField = false; })
			.AddTransition("BRAINFIELD_FINISH_BF to BRAINFIELD_FINISH_NF", "BRAINFIELD_FINISH_NF")
			.Predicator([&]()->_bool { return m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF")->IsFinished(); })
			.Priority(0)

		.AddState("BRAINFIELD_FINISH_NF")
		.OnStart([&]() 
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pModel->Find_Animation("AS_ch0100_BrainField_close_NF"));
			m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			m_pBrainField->CloseBrainField();

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(true);

			// ∫Í∑π¿Œ« µÂ ≈∞∏È ≈∞≥◊∆Ω ø¿∫Í¡ß∆ÆµÈ «œ¥√∑Œ ∂∞ø¿∏ß
			for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
			{
				if (false == static_cast<CMapKinetic_Object*>(iter)->GetThrow() && true == static_cast<CMapKinetic_Object*>(iter)->Usable())
				{
					static_cast<CMapKinetic_Object*>(iter)->Set_Dynamic();
				}
			}
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_FINISH_NF to BRAINFIELD", "BRAINFIELD")
			.Predicator([&]()->_bool { return m_pModel->Find_Animation("AS_ch0100_BrainField_close_NF")->IsFinished(); })
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_Components(void * pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModel));

			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), L"Test2",
				(CComponent**)&m_pKineticAnimModel));
		}
	}

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_LocalController"), TEXT("Com_Controller"),
		(CComponent**)&m_pController));
	
	NULL_CHECK(m_pASM = CBaseAnimInstance::Create(m_pModel, this));

	Json PlayerCollider = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Player/PlayerRange.json");
	// FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody",
	// 	L"PlayerRangeCollider", (CComponent**)&m_pRange, &PlayerCollider));

	m_pSAS_Cable = dynamic_cast<CSAS_Cable*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Prototype_GameObject_SASCable"));
	Assert(m_pSAS_Cable != nullptr);
	m_pSAS_Cable->SetTargetInfo(m_pTransformCom, m_pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_SuperSpeedTrail", L"SuperSpeedTrail", (CComponent**)&m_pTrail));
	m_pTrail->SetOwnerModel(m_pModel);
	//m_pTrail->SetActive(true);

	NULL_CHECK(m_pHotFixer = CPlayerHotFixer::Create(this));

	m_pBrainField = dynamic_cast<CBrainField*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Prototype_GameObject_BrainField"));
	Assert(m_pBrainField != nullptr);
	m_pBrainField->SetTargetInfo(this, m_pTransformCom, m_pModel);

	return S_OK;
}

HRESULT CPlayer::SetUp_RigidBody()
{
	m_TransBattleSocket.push_back(m_pModel->Find_Animation("AS_ch0100_004_Up_trans_battle"));

	Json jsonTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PlayerBattleChecker.json");
	Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"BattleChecker", (CComponent**)&m_pBattleChecker, &jsonTrigger);
	NULL_CHECK(m_pBattleChecker);

	m_pBattleChecker->SetOnTriggerIn([this](CGameObject* pGameObject)
	{
		if (auto pEnemy = dynamic_cast<CEnemy*>(pGameObject))
		{
			Detector(true);

			if (m_DetectList.empty())
			{
				m_DetectList.push_back(pEnemy);

				if (false == m_bOnBattle)
				{
					m_pASM->InputAnimSocket("Netual_Saperate_Animation", m_TransBattleSocket);
					m_bOnBattle = true;
				}
			}
			else
			{
				for (auto& iter : m_DetectList)
				{
					if (false == CGameInstance::GetInstance()->Check_ObjectAlive(iter))
						continue;

					if (pEnemy == iter)
						break;

					if (iter == m_DetectList.back())
					{
						m_DetectList.push_back(pEnemy);

						if (false == m_bOnBattle)
						{
							m_pASM->InputAnimSocket("Netual_Saperate_Animation", m_TransBattleSocket);
							m_bOnBattle = true;
						}
					}
				}
			}
		}
	});

	m_pBattleChecker->UpdateChange();

	return S_OK;
}

HRESULT CPlayer::SetUp_Event()
{
	m_pModel->Add_EventCaller("Turn_Enable", [&]() {Event_SetCanTurn(true); });
	m_pModel->Add_EventCaller("Turn_Disable", [&]() {Event_SetCanTurn(false); });

	m_pModel->Add_EventCaller("Move_Enable", [&]() {Event_SetCanMove(true); });
	m_pModel->Add_EventCaller("Move_Disable", [&]() {Event_SetCanMove(false); });

	m_pModel->Add_EventCaller("Run_Enable", [&]() {Event_SetCanRun(true); });
	m_pModel->Add_EventCaller("Run_Disable", [&]() {Event_SetCanRun(false); });

	m_pModel->Add_EventCaller("AtkTurn_Enable", [&]() {Event_SetCanTurn_Attack(true); });
	m_pModel->Add_EventCaller("AtkTurn_Disable", [&]() {Event_SetCanTurn_Attack(false); });

	m_pModel->Add_EventCaller("OnAir_Enable", [&]() {Event_SetOnAir(true); });
	m_pModel->Add_EventCaller("OnAir_Disable", [&]() {Event_SetOnAir(false); });

	m_pModel->Add_EventCaller("OnAir_Enable", [&]() {Event_SetOnAir(true); });
	m_pModel->Add_EventCaller("OnAir_Disable", [&]() {Event_SetOnAir(false); });

	m_pModel->Add_EventCaller("Gravity_Enable", [&]() {Event_SetGravity(true); });
	m_pModel->Add_EventCaller("Gravity_Disable", [&]() {Event_SetGravity(false); });

	m_pModel->Add_EventCaller("MoveLimitReset", [&]() {Event_MoveLimitReset(); });
	m_pModel->Add_EventCaller("Event_AttackLimitReset", [&]() {Event_AttackLimitReset(); });
	m_pModel->Add_EventCaller("Event_ChargeReset", [&]() {Event_ChargeReset(); });

	m_pModel->Add_EventCaller("LookAtDir", [&]() {Event_LookAtMove(); });

	m_pModel->Add_EventCaller("Jump", [&]() {Jump(); });

	m_pModel->Add_EventCaller("CamMod", [&]() {Event_CamMod(); });

	m_pModel->Add_EventCaller("Collision_Start", [&]() {Event_CollisionStart(); });
	m_pModel->Add_EventCaller("Collision_End", [&]() {Event_collisionEnd(); });

	m_pModel->Add_EventCaller("ReleaseFovCurve", [&]() {Event_FinishFovActionCam(); });

	m_pModel->Add_EventCaller("Kinetic_Launch", [&]() {Event_Kinetic_Throw(); });
	m_pModel->Add_EventCaller("Kinetic_Slow", [&]() {Event_KineticSlowAction(); });

	m_pModel->Add_EventCaller("BF_Swing_Start", [&]()
	{
		CGameObject* pKinetic = CPlayerInfoManager::GetInstance()->Get_KineticObject();

		if (nullptr != pKinetic)
		{
			static_cast<CMapKinetic_Object*>(pKinetic)->Set_Swing(true);
		}
	});

	m_pModel->Add_EventCaller("BF_Swing_End", [&]()
	{
		CGameObject* pKinetic = CPlayerInfoManager::GetInstance()->Get_KineticObject();

		if (nullptr != pKinetic)
		{
			static_cast<CMapKinetic_Object*>(pKinetic)->Set_Swing(false);
		}
	});

	m_pModel->Add_EventCaller("Small_CamShake_0.1", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.1f); });
	m_pModel->Add_EventCaller("Small_CamShake_0.2", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.2f); });
	m_pModel->Add_EventCaller("Small_CamShake_0.3", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.3f); });
	m_pModel->Add_EventCaller("Small_CamShake_0.4", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.4f); });
	m_pModel->Add_EventCaller("Small_CamShake_0.5", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.5f); });
	m_pModel->Add_EventCaller("Small_CamShake_0.6", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.6f); });
	m_pModel->Add_EventCaller("Small_CamShake_0.7", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.7f); });
	m_pModel->Add_EventCaller("Small_CamShake_0.8", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.8f); });
	m_pModel->Add_EventCaller("Small_CamShake_0.9", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 0.9f); });
	m_pModel->Add_EventCaller("Small_CamShake_1.0", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.03f, 1.f); });

	m_pModel->Add_EventCaller("Middle_CamShake_0.1", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.1f); });
	m_pModel->Add_EventCaller("Middle_CamShake_0.2", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.2f); });
	m_pModel->Add_EventCaller("Middle_CamShake_0.3", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.3f); });
	m_pModel->Add_EventCaller("Middle_CamShake_0.4", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.4f); });
	m_pModel->Add_EventCaller("Middle_CamShake_0.5", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.5f); });
	m_pModel->Add_EventCaller("Middle_CamShake_0.6", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.6f); });
	m_pModel->Add_EventCaller("Middle_CamShake_0.7", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.7f); });
	m_pModel->Add_EventCaller("Middle_CamShake_0.8", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.8f); });
	m_pModel->Add_EventCaller("Middle_CamShake_0.9", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.9f); });
	m_pModel->Add_EventCaller("Middle_CamShake_1.0", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 1.f); });

	m_pModel->Add_EventCaller("Heavy_CamShake_0.1", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.1f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_0.2", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.2f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_0.3", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.3f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_0.4", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.4f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_0.5", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.5f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_0.6", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.6f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_0.7", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.7f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_0.8", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.8f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_0.9", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.9f); });
	m_pModel->Add_EventCaller("Heavy_CamShake_1.0", [&]() {CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 1.f); });
	
	return S_OK;
}

HRESULT CPlayer::SetUp_EffectEvent()
{
	// Default Effect

	m_pModel->Add_EventCaller("Default_Attack_1", [&]() {Event_Effect("Default_Attack_1"); });
	m_pModel->Add_EventCaller("Default_Attack_2", [&]() {Event_Effect("Default_Attack_2"); });
	m_pModel->Add_EventCaller("Default_Attack_3", [&]() {Event_Effect("Default_Attack_3"); });
	m_pModel->Add_EventCaller("Default_Attack_4_1", [&]() {Event_Effect("Default_Attack_4_1"); });
	m_pModel->Add_EventCaller("Default_Attack_4_2", [&]() {Event_Effect("Default_Attack_4_2"); });
	m_pModel->Add_EventCaller("Default_Attack_4_3", [&]() {Event_Effect("Default_Attack_4_3"); });
	m_pModel->Add_EventCaller("Default_Attack_5", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, -1.5f, -1.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_5", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);

			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_5", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_5", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Default_Attack_5_BackGround", [&]() {Event_Effect("Default_Attack_5_BackGround"); });
	m_pModel->Add_EventCaller("Default_Attack_5_Final", [&]() {Event_Effect("Default_Attack_5_Final"); });
	m_pModel->Add_EventCaller("Default_Attack_5_PreEffect", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, 1.f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_5_PreEffect", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
			
			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_5_PreEffect", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_5_PreEffect", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Default_Attack_Air_Attack_1", [&]() {Event_Effect("Default_Attack_Air_Attack_1"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Attack_2", [&]() {Event_Effect("Default_Attack_Air_Attack_2"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Attack_Chase", [&]() {Event_Effect("Default_Attack_Air_Attack_Chase"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_End", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, -1.1f, -1.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_Air_Dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
			
			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_Air_Dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_Air_Dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Default_Attack_Air_Dash_0", [&]() {Event_Effect("Default_Attack_Air_Dash_0"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Dash_1", [&]() {Event_Effect("Default_Attack_Air_Dash_1"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Hold_0", [&]() {Event_Effect("Default_Attack_Air_Hold_0"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Hold_1", [&]() {Event_Effect("Default_Attack_Air_Hold_1"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Hold_2", [&]() {Event_Effect("Default_Attack_Air_Hold_2"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Hold_3_Landing", [&]() {Event_Effect("Default_Attack_Air_Hold_3_Landing"); });
	m_pModel->Add_EventCaller("Default_Attack_B4", [&]() {Event_Effect("Default_Attack_B4"); });
	m_pModel->Add_EventCaller("Default_Attack_Charging_C4", [&]() {Event_Effect("Default_Attack_Charging_C"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_0", [&]() {Event_Effect("Default_Attack_Dash_0"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_1_01", [&]() {Event_Effect("Default_Attack_Dash_1_01"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_1_02", [&]() {Event_Effect("Default_Attack_Dash_1_02"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_2", [&]() {Event_Effect("Default_Attack_Dash_2"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_3_End", [&]() {Event_Effect("Default_Attack_Dash_3_End"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_Hold_0", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
		{
			_float4x4 EffectPivotMatrix = XMMatrixTranslation(0.f, 0.5f, 0.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_Dash_Hold_0", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
		//Event_Effect("Default_Attack_Dash_Hold_0");
	});
	m_pModel->Add_EventCaller("Default_Attack_Dash_Hold_1", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
		{
			_float4x4 EffectPivotMatrix = XMMatrixTranslation(0.f, 0.5f, 0.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_Dash_Hold_1", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
		//Event_Effect("Default_Attack_Dash_Hold_1"); 
	});
	m_pModel->Add_EventCaller("Default_Attack_Dash_Hold_2", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
		{
			_float4x4 EffectPivotMatrix = XMMatrixTranslation(0.f, 0.5f, 0.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_Dash_Hold_2", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}

		//Event_Effect("Default_Attack_Dash_Hold_2"); 
	});
	m_pModel->Add_EventCaller("Default_Attack_Justdodge_0", [&]() {Event_Effect("Default_Attack_Justdodge_0"); });
	m_pModel->Add_EventCaller("Default_Attack_Justdodge_1", [&]() {Event_Effect("Default_Attack_Justdodge_1"); });
	m_pModel->Add_EventCaller("Default_Attack_Justdodge_2", [&]() {Event_Effect("Default_Attack_Justdodge_2"); });
	m_pModel->Add_EventCaller("Default_Attack_Sas_Attack_0", [&]() {Event_Effect("Default_Attack_Sas_Attack_0"); });
	m_pModel->Add_EventCaller("Default_Attack_Sas_Attack_1", [&]() {Event_Effect("Default_Attack_Sas_Attack_1"); });
	m_pModel->Add_EventCaller("Default_Attack_Sas_Attack_2", [&]() {Event_Effect("Default_Attack_Sas_Attack_2"); });
//	m_pModel->Add_EventCaller("Default_Attack_Slash", [&]() {Event_Effect("Default_Attack_Slash"); });
	m_pModel->Add_EventCaller("Default_Attack_Upper", [&]() {Event_Effect("Default_Attack_Upper"); });
	
	// Fire Effect
	m_pModel->Add_EventCaller("Fire_Attack_1", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixRotationX(XMConvertToRadians(180.f))
				* XMMatrixRotationY(XMConvertToRadians(-45.f))
				* XMMatrixRotationZ(XMConvertToRadians(135.f))
				* XMMatrixTranslation(0.f, 1.f, -1.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_1_001", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		
			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_1_001", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_1_001", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_1_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixRotationX(XMConvertToRadians(180.f))
				* XMMatrixRotationY(XMConvertToRadians(-45.f))
				* XMMatrixRotationZ(XMConvertToRadians(135.f))
				* XMMatrixTranslation(0.f, 1.f, -1.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_1_001_Particle"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		
			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_1_001_Particle"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_1_001_Particle"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_2", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix = XMMatrixScaling(3.f, 3.f, 3.f)
				* XMMatrixRotationZ(XMConvertToRadians(160.f))
				* XMMatrixTranslation(0.f, 0.75f, 0.25f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_2", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);

			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_2", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_2", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_2_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix = XMMatrixScaling(3.f, 3.f, 3.f)
				* XMMatrixRotationZ(XMConvertToRadians(160.f))
				* XMMatrixTranslation(0.f, 0.75f, 0.25f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_2_Particle"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_3", [&]() {Event_Effect("Fire_Attack_3"); });
	m_pModel->Add_EventCaller("Fire_Attack_3_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_3_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_3_twist", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, 0.f, -2.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_3_Double_twist", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);

			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_3_Double_twist", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_3_Double_twist", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_3_twist_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixScaling(10.f, 10.f, 10.f) *
				XMMatrixTranslation(0.f, 0.f, -2.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, L"Fire_Attack_3_Twist_Particle", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", false);
		
		}
	});

	m_pModel->Add_EventCaller("Fire_Attack_4", [&]() {Event_Effect("Fire_Attack_4"); });
	m_pModel->Add_EventCaller("Fire_Attack_4_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_4_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_4_001", [&]() {Event_Effect("Fire_Attack_4_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_4_001_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_4_001_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});


	m_pModel->Add_EventCaller("Fire_Attack_5", [&]() {Event_Effect("Fire_Attack_5"); });
	m_pModel->Add_EventCaller("Fire_Attack_5_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_5_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_5_PreEffect", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix = XMMatrixTranslation(0.f, 1.f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_5_PreEffect", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		
			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_5_PreEffect", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_5_PreEffect", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_1", [&]() {Event_Effect("Fire_Attack_Air_1"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_1_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_1_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_2", [&]() {Event_Effect("Fire_Attack_Air_2"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_2_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_2_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});


	m_pModel->Add_EventCaller("Fire_Attack_Air_Chase", [&]() {Event_Effect("Fire_Attack_Air_Chase"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_chase_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_chase_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_0", [&]() {Event_Effect("Fire_Attack_Air_dash_0"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_0_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_dash_0_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_End", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, -1.1f, -1.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Air_dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);

			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Air_dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Air_dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_End_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, -1.1f, -1.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_dash_0_Particle"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});


	m_pModel->Add_EventCaller("Fire_AttacK_Air_dash_0_001", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix = XMMatrixRotationY(XMConvertToRadians(45.f)) * XMMatrixTranslation(0.f, -0.5f, 0.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_AttacK_Air_dash_0_001", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);

			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Air_dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Air_dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	//m_pModel->Add_EventCaller("Fire_AttacK_Air_dash_0_001_Particle", [&]() 
	//{	
	//	_float4x4 ParticlePivotMatrix = XMMatrixRotationY(XMConvertToRadians(45.f)) * XMMatrixTranslation(0.f, -0.5f, 0.f);
	//	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_AttacK_Air_dash_0_001_Particle"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, ParticlePivotMatrix, "Eff01", true);
	//});


	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1", [&]() {Event_Effect("Fire_Attack_Air_dash_1"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1_Particle", [&]() {
		
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_dash_1_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1_001", [&]() {Event_Effect("Fire_Attack_Air_dash_1_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1_002", [&]() {Event_Effect("Fire_Attack_Air_dash_1_002"); });

	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold", [&]() {Event_Effect("Fire_Attack_Air_Hold");});
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_Hold_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_1", [&]() {Event_Effect("Fire_Attack_Air_Hold_1");});
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_1_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_Hold_1_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_2", [&]() {Event_Effect("Fire_Attack_Air_Hold_2");});
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_2_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_Hold_2_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_Landing", [&]() {Event_Effect("Fire_Attack_Air_Hold_Landing");});
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_Landing_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_Hold_Landing_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Charge03", [&]() {Event_Effect("Fire_Attack_Charge03");});
	m_pModel->Add_EventCaller("Fire_Attack_Charge03_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Charge03_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Charge03_001", [&]() {Event_Effect("Fire_Attack_Charge03_001");});
	m_pModel->Add_EventCaller("Fire_Attack_Charge03_001_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Charge03_001_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Charge03_002", [&]() {Event_Effect("Fire_Attack_Charge03_002");});
	m_pModel->Add_EventCaller("Fire_Attack_Charge03_002_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Charge03_002_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_00", [&]() {Event_Effect("Fire_Attack_Dash_00");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_00_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_00_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_01", [&]() {Event_Effect("Fire_Attack_Dash_01");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_01_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_01_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_001", [&]() {Event_Effect("Fire_Attack_Dash_02_001");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_001_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_02_001_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_002", [&]() {Event_Effect("Fire_Attack_Dash_02_002");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_002_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_02_002_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_03", [&]() {Event_Effect("Fire_Attack_Dash_03");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_03_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_03_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_Hold_001", [&]() {});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_Hold_002", [&]() {});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_Hold_003", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixScaling(1.5f, 1.5f, 1.5f)
				* XMMatrixTranslation(-2.f, -1.5f, 2.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Dash_Hold_003", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);

			if (CPlayerInfoManager::GetInstance()->Get_Copy())
			{
				_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vRight.Normalize();
				_float4 vLeft = vRight * -1.f;

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Dash_Hold_003", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, EffectPivotMatrix, "Eff01", true);
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Dash_Hold_003", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, EffectPivotMatrix, "Eff01", true);
			}
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_Hold_003_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixScaling(1.5f, 1.5f, 1.5f)
				* XMMatrixTranslation(-2.f, -1.5f, 2.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_Hold_003_Particle"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});

	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_01", [&]() {Event_Effect("Fire_Attack_Justdodge_01"); });
	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_02", [&]() {Event_Effect("Fire_Attack_Justdodge_02"); });
	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_03", [&]() {Event_Effect("Fire_Attack_Justdodge_03"); });
	m_pModel->Add_EventCaller("Fire_Attack_Upper", [&]() {Event_Effect("Fire_Attack_Upper");});
	m_pModel->Add_EventCaller("Fire_Attack_Upper_Particle", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Upper_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Elec_Attack_1", [&]() {Event_ElecEffect("Elec_Attack_1"); });
	m_pModel->Add_EventCaller("Elec_Attack_1_Shoot", [&]() {Event_ElecEffect("Elec_Attack_1_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_1_001", [&]() {Event_ElecEffect("Elec_Attack_1_001"); });
	m_pModel->Add_EventCaller("Elec_Attack_2", [&]() {Event_ElecEffect("Elec_Attack_2"); });
	m_pModel->Add_EventCaller("Elec_Attack_2_Shoot", [&]() {Event_ElecEffect("Elec_Attack_2_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_3", [&]() {Event_ElecEffect("Elec_Attack_3"); });
	m_pModel->Add_EventCaller("Elec_Attack_3_Shoot", [&]() {Event_ElecEffect("Elec_Attack_3_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_3_001", [&]() {Event_ElecEffect("Elec_Attack_3_001"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_001", [&]() {Event_ElecEffect("Elec_Attack_4_001"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_002", [&]() {Event_ElecEffect("Elec_Attack_4_002"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_002_Shoot", [&]() {Event_ElecEffect("Elec_Attack_4_002_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_003", [&]() {Event_ElecEffect("Elec_Attack_4_003"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_003_Shoot", [&]() {Event_ElecEffect("Elec_Attack_4_003_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_004", [&]() {Event_ElecEffect("Elec_Attack_4_004"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_004_Shoot", [&]() {Event_ElecEffect("Elec_Attack_4_004_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_005", [&]() {Event_ElecEffect("Elec_Attack_4_005"); });
	m_pModel->Add_EventCaller("Elec_Attack_5_Shoot", [&]() {Event_ElecEffect("Elec_Attack_5_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_5", [&]() {Event_ElecEffect("Elec_Attack_5"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_1", [&]() {Event_ElecEffect("Elec_Attack_Air_1"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_1_Shoot", [&]() {Event_ElecEffect("Elec_Attack_Air_1_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_2", [&]() {Event_ElecEffect("Elec_Attack_Air_2"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_2_Shoot", [&]() {Event_ElecEffect("Elec_Attack_Air_2_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Chase", [&]() {Event_ElecEffect("Elec_Attack_Air_Chase"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_0", [&]() {Event_ElecEffect("Elec_Attack_Air_Dash_0"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_1", [&]() {Event_ElecEffect("Elec_Attack_Air_Dash_1"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_1_001", [&]() {Event_ElecEffect("Elec_Attack_Air_Dash_1_001"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_00", [&]() {Event_ElecEffect("Elec_Attack_Air_Hold_00"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_01", [&]() {Event_ElecEffect("Elec_Attack_Air_Hold_01"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_02", [&]() {Event_ElecEffect("Elec_Attack_Air_Hold_02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_03", [&]() {Event_ElecEffect("Elec_Attack_Air_Hold_03"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_04", [&]() {Event_ElecEffect("Elec_Attack_Air_Hold_04"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_05", [&]() {Event_ElecEffect("Elec_Attack_Air_Hold_05"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_Landing", [&]() {Event_ElecEffect("Elec_Attack_Air_Hold_Landing"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_01", [&]() {Event_ElecEffect("Elec_Attack_Charge_01"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_02", [&]() {Event_ElecEffect("Elec_Attack_Charge_02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_03", [&]() {Event_ElecEffect("Elec_Attack_Charge_03"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_1", [&]() {Event_ElecEffect("Elec_Attack_Dash_1"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_2_001", [&]() {Event_ElecEffect("Elec_Attack_Dash_2_001"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_2_002", [&]() {Event_ElecEffect("Elec_Attack_Dash_2_002"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_3", [&]() {Event_ElecEffect("Elec_Attack_Dash_3"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_End", [&]() {Event_ElecEffect("Elec_Attack_Dash_End"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_0", [&]() {Event_ElecEffect("Elec_Attack_Dash_Hold_0"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_1", [&]() {Event_ElecEffect("Elec_Attack_Dash_Hold_1"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_2", [&]() {Event_ElecEffect("Elec_Attack_Dash_Hold_2"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_2_Shoot", [&]() {Event_ElecEffect("Elec_Attack_Dash_Hold_2_Shoot"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_0", [&]() {Event_ElecEffect("Elec_Attack_Justdodge_0"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_1", [&]() {Event_ElecEffect("Elec_Attack_Justdodge_1"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_2", [&]() {Event_ElecEffect("Elec_Attack_Justdodge_2"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_3", [&]() {Event_ElecEffect("Elec_Attack_Justdodge_3"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_4", [&]() {Event_ElecEffect("Elec_Attack_Justdodge_4"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_5", [&]() {Event_ElecEffect("Elec_Attack_Justdodge_5"); });
	m_pModel->Add_EventCaller("Elec_Attack_Upper_0", [&]() {Event_ElecEffect("Elec_Attack_Upper_0"); });
	m_pModel->Add_EventCaller("Elec_Attack_Upper_1", [&]() {Event_ElecEffect("Elec_Attack_Upper_1"); });

	m_pModel->Add_EventCaller("Elec_Attack_1_Particle", [&]() {Event_ElecParticle ("Elec_Attack_1_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_2_Particle", [&]() {Event_ElecParticle("Elec_Attack_2_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_3_Particle", [&]() {Event_ElecParticle("Elec_Attack_3_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_3_001_Particle", [&]() {Event_ElecParticle("Elec_Attack_3_001_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_001_Particle", [&]() {Event_ElecParticle("Elec_Attack_4_001_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_002_Particle", [&]() {Event_ElecParticle("Elec_Attack_4_002_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_003_Particle", [&]() {Event_ElecParticle("Elec_Attack_4_003_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_004_Particle", [&]() {Event_ElecParticle("Elec_Attack_4_004_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_005_Particle", [&]() {Event_ElecParticle("Elec_Attack_4_005_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_5_Particle", [&]() {Event_ElecParticle("Elec_Attack_5_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_1_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_1_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_2_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_2_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Chase_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Chase_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_0_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Dash_0_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_1_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Dash_1_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_1_001_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Dash_1_001_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_00_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Hold_00_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_01_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Hold_01_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_02_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Hold_02_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_03_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Hold_03_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_04_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Hold_04_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_05_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Hold_05_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_Landing_Particle", [&]() {Event_ElecParticle("Elec_Attack_Air_Hold_Landing_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_01_Particle", [&]() {Event_ElecParticle("Elec_Attack_Charge_01_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_02_Particle", [&]() {Event_ElecParticle("Elec_Attack_Charge_02_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_03_Particle", [&]() {Event_ElecParticle("Elec_Attack_Charge_03_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_1_Particle", [&]() {Event_ElecParticle("Elec_Attack_Dash_1_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_2_001_Particle", [&]() {Event_ElecParticle("Elec_Attack_Dash_2_001_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_2_002_Particle", [&]() {Event_ElecParticle("Elec_Attack_Dash_2_002_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_3_Particle", [&]() {Event_ElecParticle("Elec_Attack_Dash_3_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_End_Particle", [&]() {Event_ElecParticle("Elec_Attack_Dash_End_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_0_Particle", [&]() {Event_ElecParticle("Elec_Attack_Dash_Hold_0_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_1_Particle", [&]() {Event_ElecParticle("Elec_Attack_Dash_Hold_1_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_2_Particle", [&]() {Event_ElecParticle("Elec_Attack_Dash_Hold_2_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_0_Particle", [&]() {Event_ElecParticle("Elec_Attack_Justdodge_0_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_1_Particle", [&]() {Event_ElecParticle("Elec_Attack_Justdodge_1_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_2_Particle", [&]() {Event_ElecParticle("Elec_Attack_Justdodge_2_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_3_Particle", [&]() {Event_ElecParticle("Elec_Attack_Justdodge_3_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_4_Particle", [&]() {Event_ElecParticle("Elec_Attack_Justdodge_4_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_5_Particle", [&]() {Event_ElecParticle("Elec_Attack_Justdodge_5_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Upper_0_Particle", [&]() {Event_ElecParticle("Elec_Attack_Upper_0_Particle"); });
	m_pModel->Add_EventCaller("Elec_Attack_Upper_1_Particle", [&]() {Event_ElecParticle("Elec_Attack_Upper_1_Particle"); });

	m_pModel->Add_EventCaller("Trail_On", [&]() 
	{ 
		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			for (auto& iter : m_vecWeapon)
			{
				static_cast<CScarletWeapon*>(iter)->Trail_Setting(true);
			}
		}
		else
		{
			static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(true);
		}
	});

	m_pModel->Add_EventCaller("Trail_Off", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			for (auto& iter : m_vecWeapon)
			{
				static_cast<CScarletWeapon*>(iter)->Trail_Setting(false);
			}
		}
		else
		{
			static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(false);
		}
	});

	m_pModel->Add_EventCaller("WeaponBright_On", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			for (auto& iter : m_vecWeapon)
			{
				static_cast<CScarletWeapon*>(iter)->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, true);
			}
		}
		else
		{
			static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, true);
		}
	});

	m_pModel->Add_EventCaller("WeaponBright_Off", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			for (auto& iter : m_vecWeapon)
			{
				static_cast<CScarletWeapon*>(iter)->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, false);
			}
		}
		else
		{
			static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, false);
		}
	});

	m_pModel->Add_EventCaller("LandingDust", [&]()
	{
		random_shuffle(m_vecRandomLandingDustName.begin(), m_vecRandomLandingDustName.end());

		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, m_vecRandomLandingDustName.front(), LAYER_PLAYEREFFECT)->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), _float4(0.f, 1.f, 0.f, 0.f));
	});

	m_pModel->Add_EventCaller("KineticCircle", [&]()
	{
		Event_KineticCircleEffect();
		m_SoundStore.PlaySound("fx_kinetic_lifting", m_pTransformCom);
	});

	m_pModel->Add_EventCaller("KineticCircle_Attach", [&]()
	{
		Event_KineticCircleEffect_Attach();
		m_SoundStore.PlaySound("fx_kinetic_lifting", m_pTransformCom);
	});

	return S_OK;
}

HRESULT CPlayer::Setup_KineticStateMachine()
{
	// ?ºÎ†•???¨Ïö©???†ÎãàÎ©îÏù¥??Ï∂îÍ?

	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_start"));
	m_Kinetic_RB_Start.push_back(m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_start"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_loop"));
	m_Kinetic_RB_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_loop"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_cancel"));
	m_Kinetic_RB_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_cancel"));
	m_Kinetic_RB_Throw01_Start.push_back(m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_start"));
	m_Kinetic_RB_Throw01_Start.push_back(m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_cancel"));
	m_Kinetic_RB_Throw01_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_loop"));
//	m_Kinetic_RB_Throw01_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_cancel"));
	m_Kinetic_RB_Throw02_Start.push_back(m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_start"));
	m_Kinetic_RB_Throw02_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_loop"));
	m_Kinetic_RB_Throw02_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_cancel"));
	m_Kinetic_RB_Air_Start.push_back(m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_start"));
	m_Kinetic_RB_Air_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_loop"));
	m_Kinetic_RB_Air_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_cancel"));
	m_Kinetic_RB_Air_Throw01_Start.push_back(m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_start"));
	m_Kinetic_RB_Air_Throw01_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_loop"));
	m_Kinetic_RB_Air_Throw01_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_cancel"));

	// ~?ºÎ†•???¨Ïö©???†ÎãàÎ©îÏù¥??Ï∂îÍ?

	m_pKineticStataMachine = CFSMComponentBuilder().InitState("NO_USE_KINETIC")

		.AddState("NO_USE_KINETIC")
		.OnStart([&]() 
		{
			//SetAbleState({ true, false, false, false, false, true, true, true, true, false });
			m_pASM->ClearAnimSocket("Kinetic_AnimSocket");
			m_pASM->SetCurState(m_pASM->GetCurStateName());
			m_bKineticMove = false;
		})
		.Tick([&](double TimeDelta) 
		{ 
			m_bKineticMove = false; 
			KineticObject_Targeting();
		})

			.AddTransition("NO_USE_KINETIC to KINETIC_RB_START", "KINETIC_RB_START")
			.Predicator([&]()->_bool
			{
				return CanKinetic(15) && m_bKineticRB && !m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) && !m_bHit;
			})
			.Priority(0)

#pragma region KineticRB

		.AddState("KINETIC_RB_START")
			.OnStart([&]() 
			{
				Enemy_Targeting(true);
				m_pASM->InputAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Start);
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->SetParticle();
			})
			.Tick([&](double fTimeDelta)
			{
				m_bKineticMove = true;
			})
			.AddTransition("KINETIC_RB_START to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool{return CPlayerInfoManager::GetInstance()->Get_KineticObject() == nullptr;})
			.Priority(0)

			.AddTransition("KINETIC_RB_START to KINETIC_RB_LOOP", "KINETIC_RB_LOOP")
			.Predicator([&]()->_bool{return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_START to KINETIC_RB_CANCEL", "KINETIC_RB_CANCEL")
			.Predicator([&]()->_bool{return !m_bKineticRB;})
			.Priority(0)

		.AddState("KINETIC_RB_LOOP")
			.OnStart([&]() 
			{ 
				m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Loop);

				if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->CreateLargeParticle();
			})
			.Tick([&](double TimeDelta) 
			{
				m_bKineticMove = true;
				m_fKineticCharge += TimeDelta;
				if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
				{
					random_device rng;
					uniform_real_distribution<_double> dist0(-1.0, 1.0);
					uniform_real_distribution<_double> dist1(-1.0, 1.0);
					uniform_real_distribution<_double> dist2(-1.0, 1.0);

					_float3 vForce = _float3(0.f, m_fChargePower * g_fTimeDelta, 0.f);
					_float3 vTorque = _float3(dist0(rng), dist1(rng), dist2(rng));
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Add_Physical(vForce, vTorque);
				}

				CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
			})
			.OnExit([&]() 
			{
				IM_LOG("CHARGE : %f", CPlayerInfoManager::GetInstance()->Get_KineticCharge());
				m_fKineticCharge = 0.f;
				CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
			})

			.AddTransition("KINETIC_RB_LOOP to KINETIC_RB_THROW_01_START", "KINETIC_RB_THROW_01_START")
			.Predicator([&]()->_bool{return m_fKineticCharge >= 1.f;})
			.Priority(0)

			.AddTransition("KINETIC_RB_LOOP to KINETIC_RB_CANCEL", "KINETIC_RB_CANCEL")
			.Predicator([&]()->_bool{return !m_bKineticRB;})
			.Priority(0)

		.AddState("KINETIC_RB_CANCEL")
			.OnStart([&]() 
			{ 
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->ReleaseParticle();
				m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Cancel); 
			})
			.Tick([&](double fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_CANCEL to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool{return !m_bKineticRB && m_pASM->isSocketEmpty("Kinetic_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_CANCEL to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool
			{
				if (nullptr == m_pASM->GetSocketAnimation("Kinetic_AnimSocket")) return false;
				return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_AnimSocket")->GetPlayRatio() >= 0.2f);
			})
			.Priority(0)
			
#pragma endregion KineticRB

#pragma region KineticRB_Throw

		// ?ºÎ∞ò ?òÏ?Í∏?
		.AddState("KINETIC_RB_THROW_01_START")	// Î£®ÌîÑ / ?§Îìú
			.OnStart([&]() 
			{
				m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw01_Start);

				if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
				{
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->KineticRimLightFix(true);
					Event_Kinetic_Throw();
				}
			})
			.Tick([&](double fTimeDelta) 
			{
				m_bKineticMove = true;
			})

			.AddTransition("KINETIC_RB_THROW_01_START to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_RB_THROW_01_START to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_bLeftClick || m_bDash || m_bJump; })
			.Priority(0)

			.AddTransition("KINETIC_RB_THROW_01_START to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Kinetic_AnimSocket", 0.2f) && m_bKineticRB; })
			.Priority(0)

#pragma endregion KineticRB_Throw

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_HitStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_FL"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_FR"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_402_AL_damage_l_B"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_411_AL_damage_h_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_412_AL_damage_h_B"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_413_AL_damage_h_L"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_414_AL_damage_h_R"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_433_AL_blow_landing_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_434_AL_down_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_435_AL_wakeup_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_438_AL_breakfall_F"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_455_AL_rise_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_457_AL_fall_start"));

	m_Hit_FL_Level01.push_back(m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_FL"));
	m_Hit_F_Level01.push_back(m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_F"));
	m_Hit_FR_Level01.push_back(m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_FR"));
	m_Hit_B_Level01.push_back(m_pModel->Find_Animation("AS_ch0100_402_AL_damage_l_B"));

	m_Hit_F_Level02.push_back(m_pModel->Find_Animation("AS_ch0100_411_AL_damage_h_F"));
	m_Hit_B_Level02.push_back(m_pModel->Find_Animation("AS_ch0100_412_AL_damage_h_B"));
	m_Hit_L_Level02.push_back(m_pModel->Find_Animation("AS_ch0100_413_AL_damage_h_L"));
	m_Hit_R_Level02.push_back(m_pModel->Find_Animation("AS_ch0100_414_AL_damage_h_R"));

	m_Knuckback.push_back(m_pModel->Find_Animation("AS_ch0100_455_AL_rise_start"));

	m_Airborne.push_back(m_pModel->Find_Animation("AS_ch0100_406_AL_damage_air"));

	m_Fall.push_back(m_pModel->Find_Animation("AS_ch0100_457_AL_fall_start"));

	m_FallDown_Back.push_back(m_pModel->Find_Animation("AS_ch0100_433_AL_blow_landing_F"));
	m_FallDown_Back.push_back(m_pModel->Find_Animation("AS_ch0100_434_AL_down_F"));
	m_FallDown_Back.push_back(m_pModel->Find_Animation("AS_ch0100_435_AL_wakeup_F"));

	m_BreakFall_Front.push_back(m_pModel->Find_Animation("AS_ch0100_438_AL_breakfall_F"));

	m_BreakFall_Landing.push_back(m_pModel->Find_Animation("AS_ch0100_141_AL_jump_landing"));

	m_pHitStateMachine = CFSMComponentBuilder().InitState("NON_HIT")
		.AddState("NON_HIT")
			.OnStart([&]() 
			{ 
				//SetAbleState({ false, false, false, false, false, true, true, true, true, false });
				m_pModel->Reset_LocalMove(true);

				if (!m_pASM->isSocketEmpty("Hit_AnimSocket"))
					m_pASM->GetSocketAnimation("Hit_AnimSocket")->Reset();

				m_pASM->ClearAnimSocket("Hit_AnimSocket");
			})
			.Tick([&](double fTimeDelta) {m_bHit = false; })
			.OnExit([&]() 
			{ 
				m_pCamSpot->Reset_CamMod();
				m_pASM->SetCurState("IDLE");
				m_pASM->SetCurState_BrainField("IDLE");
				m_bSeperateAnim = false; m_bHit = true;
				SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})
				.AddTransition("NON_HIT to AIRBORNE", "AIRBORNE")
				.Predicator([&]()->_bool {return m_bHit && (m_DamageDesc.m_iDamageType == EAttackType::ATK_TO_AIR); })
				.Priority(0)

				.AddTransition("NON_HIT to KNUCKBACK", "KNUCKBACK")
				.Predicator([&]()->_bool {return m_bHit && (m_DamageDesc.m_iDamageType == EAttackType::ATK_HEAVY); })
				.Priority(0)

				.AddTransition("NON_HIT to HIT_LIGHT", "HIT_LIGHT")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_LIGHT); })
				.Priority(0)

				.AddTransition("NON_HIT to HIT_MIDIUM", "HIT_MIDIUM")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE); })
				.Priority(0)

		.AddState("KNUCKBACK")
			.OnStart([&]() 
			{ 
				CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.2f);

				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Knuckback);
			})
			.Tick([&](double TimeDelta) 
			{ 
				m_bWalk = false;
				_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTransformCom->Get_WorldMatrix(), "AS_ch0100_455_AL_rise_start");
				m_pTransformCom->LocalMove(vOpTest);
			})
				.AddTransition("KNUCKBACK to FALLDOWN", "FALLDOWN")
				.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Hit_AnimSocket"); })
				.Priority(0)

		.AddState("AIRBORNE")
			.OnStart([&]() 
			{
				CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.2f);

				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Airborne); 
				Jump();
			})
			.Tick([&](double TimeDelta) 
			{ 
				m_bWalk = false;
				m_bAir = true;
			})
				.AddTransition("AIRBORNE to FALL", "FALL")
				.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Hit_AnimSocket"); })
				.Priority(1)

				.AddTransition("AIRBORNE to FALLDOWN", "FALLDOWN")
				.Predicator([&]()->_bool {return m_bAir && m_pASM->isSocketAlmostFinish("Hit_AnimSocket");; })
				.Priority(0)

		.AddState("FALL")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Hit_AnimSocket", m_Fall); })
			.Tick([&](double TimeDelta) 
			{
				m_bWalk = false;
				m_bAir = true;

				if (m_pASM->isSocketAlmostFinish("Hit_AnimSocket"))
				{
					m_pASM->AttachAnimSocket("Hit_AnimSocket", m_Fall);
				}
			})
				.AddTransition("FALL to FALLDOWN", "FALLDOWN")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Priority(0)

				.AddTransition("FALL to BREAKFALL", "BREAKFALL")
				.Predicator([&]()->_bool {return m_bBreakFall; })
				.Priority(0)

		.AddState("FALLDOWN")
			.OnStart([&]() { m_pASM->InputAnimSocket("Hit_AnimSocket", m_FallDown_Back); })
			.Tick([&](double TimeDelta) 
			{
				m_bWalk = false;
				//m_bAir = false;
			})
			.OnExit([&]()
			{
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
				m_bHit = false;
			})
				.AddTransition("FALLDOWN to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("FALLDOWN to BREAKFALL", "BREAKFALL")
				.Predicator([&]()->_bool {return !m_pASM->isSocketPassby("Hit_AnimSocket", 0.f) && m_bBreakFall; })
				.Priority(0)

		//.AddState("BREAKFALL_FRONT")
		//	.OnStart([&]() { m_pASM->InputAnimSocket("Hit_AnimSocket", m_BreakFall_Front); })
		//	.Tick([&](double TimeDelta) 
		//	{
		//		m_bWalk = false;
		//		m_bActiveGravity = false;
		//		//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), "AS_ch0100_438_AL_breakfall_F");
		//		//m_pTransformCom->LocalMove(vLocal);
		//	})
		//	.OnExit([&]() {m_bActiveGravity = true; })
		//		.AddTransition("BREAKFALL_FRONT to NON_HIT", "NON_HIT")
		//		.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Hit_AnimSocket", 0.5f) || m_bOnFloor; })
		//		.Priority(0)

		.AddState("HIT_LIGHT")
			.OnStart([&]() { CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.1f); })
			.AddTransition("HIT_LIGHT to FRONT", "HIT_LIGHT_FRONT")
			.Predicator([&]()->_bool 
			{
				_float fDot = XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_DamageDesc.m_vHitDir));
				return (fDot < 0.f) ? true : false; 
			})
			.Priority(0)

			.AddTransition("HIT_LIGHT to BACK", "HIT_LIGHT_BACK")
			.Predicator([&]()->_bool
			{
				_float fDot = XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_DamageDesc.m_vHitDir));
				return (fDot > 0.f) ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_LIGHT to NON_HIT", "NON_HIT")
			.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
			.Priority(100)

		.AddState("HIT_LIGHT_FRONT")
			.OnStart([&]() 
			{ 
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_F_Level01); 
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
				m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_LIGHT_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_LIGHT_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_bHit && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_LIGHT_BACK")
			.OnStart([&]() 
			{
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_B_Level01);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
				m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_LIGHT_BACK to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_LIGHT_BACK to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_bHit && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM")
			.OnStart([&]() { CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.2f); })
			.AddTransition("HIT_MIDIUM to FRONT", "HIT_MIDIUM_FRONT")
			.Predicator([&]()->_bool 
			{
				return m_DamageDesc.m_eHitDir == EBaseAxis::NORTH ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDIUM to BACK", "HIT_MIDIUM_BACK")
			.Predicator([&]()->_bool
			{
				return m_DamageDesc.m_eHitDir == EBaseAxis::SOUTH ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDIUM to LEFT", "HIT_MIDIUM_LEFT")
			.Predicator([&]()->_bool 
			{
				return m_DamageDesc.m_eHitDir == EBaseAxis::WEST ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDIUM to RIGHT", "HIT_MIDIUM_RIGHT")
			.Predicator([&]()->_bool
			{
				return m_DamageDesc.m_eHitDir == EBaseAxis::EAST ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDIUM to NON_HIT", "NON_HIT")
			.Predicator([&]()->_bool {return true; })
			.Priority(100)

		.AddState("HIT_MIDIUM_FRONT")
			.OnStart([&]() 
			{ 
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_F_Level02);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
				m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_MIDIUM_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_MIDIUM_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_bHit && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_BACK")
			.OnStart([&]() 
			{ 
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_B_Level02);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
				m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_MIDIUM_BACK to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_MIDIUM_BACK to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_bHit && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_LEFT")
			.OnStart([&]() 
			{ 
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_L_Level02);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
				m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_MIDIUM_LEFT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_MIDIUM_LEFT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_bHit && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_RIGHT")
			.OnStart([&]() 
			{ 
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_R_Level02);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
				m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_MIDIUM_RIGHT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_MIDIUM_RIGHT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_bHit && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("BREAKFALL")
			.OnStart([&]()
			{
				m_bAir = true;
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_BreakFall_Front);
				m_fYSpeed = 5.f;
			})
				.AddTransition("BREAKFALL to BREAKFALL_LANDING", "BREAKFALL_LANDING")
				.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Hit_AnimSocket", 0.2f) && m_bOnFloor; })
				.Priority(0)

				.AddTransition("BREAKFALL to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(1)

		.AddState("BREAKFALL_LANDING")
			.OnStart([&]()
			{
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
				m_bHit = false;
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_BreakFall_Landing);
			})

				.AddTransition("BREAKFALL_LANDING to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Hit_AnimSocket", 0.5f) && (m_bLeftClick || m_bJump || m_bDash || m_bWalk || m_bKineticG || m_bKineticRB); })
				.Priority(0)

				.AddTransition("BREAKFALL_LANDING to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Hit_AnimSocket"); })
				.Priority(1)

				.AddTransition("BREAKFALL_LANDING to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(1)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_KineticComboStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_216_AL_atk_dash1_start"));
	m_KineticCombo_Slash01.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_216_AL_atk_dash1_end"));
	m_KineticCombo_Slash01.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_217_AL_atk_dash2_start"));
	m_KineticCombo_Slash02.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_217_AL_atk_dash2_end"));
	m_KineticCombo_Slash02.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_218_AL_atk_dash3_start"));
	m_KineticCombo_Slash03.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_218_AL_atk_dash3_end"));
	m_KineticCombo_Slash03.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_219_AL_atk_dash4_start"));
	m_KineticCombo_Slash04.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_219_AL_atk_dash4_end"));
	m_KineticCombo_Slash04.push_back(pAnimation);

	// sLcLeR
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_231_AL_Cap_sLcLeR_start"));
	m_KineticCombo_sLcLeR_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_231_AL_Cap_sLcLeR_loop"));
	m_KineticCombo_sLcLeR_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_231_AL_Cap_sLcLeR_end"));
	m_KineticCombo_sLcLeR_End.push_back(pAnimation);

	// sLcLeL
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_232_AL_Cap_sLcLeL_start"));
	m_KineticCombo_sLcLeL_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_232_AL_Cap_sLcLeL_loop"));
	m_KineticCombo_sLcLeL_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_232_AL_Cap_sLcLeL_end"));
	m_KineticCombo_sLcLeL_End.push_back(pAnimation);

	// sLcReR
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_235_AL_Cap_sLcReR_start"));
	m_KineticCombo_sLcReR_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_235_AL_Cap_sLcReR_loop"));
	m_KineticCombo_sLcReR_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_235_AL_Cap_sLcReR_end"));
	m_KineticCombo_sLcReR_End.push_back(pAnimation);

	// sLcReL
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_236_AL_Cap_sLcReL_start"));
	m_KineticCombo_sLcReL_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_236_AL_Cap_sLcReL_loop"));
	m_KineticCombo_sLcReL_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_236_AL_Cap_sLcReL_end"));
	m_KineticCombo_sLcReL_End.push_back(pAnimation);
	
	// sRcLeL
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_237_AL_Cap_sRcLeL_start"));
	m_KineticCombo_sRcLeL_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_237_AL_Cap_sRcLeL_loop"));
	m_KineticCombo_sRcLeL_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_237_AL_Cap_sRcLeL_end"));
	m_KineticCombo_sRcLeL_End.push_back(pAnimation);

	// sRcLeR
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_238_AL_Cap_sRcLeR_start"));
	m_KineticCombo_sRcLeR_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_238_AL_Cap_sRcLeR_loop"));
	m_KineticCombo_sRcLeR_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_238_AL_Cap_sRcLeR_end"));
	m_KineticCombo_sRcLeR_End.push_back(pAnimation);

	// sRcReL
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_241_AL_Cap_sRcReL_start"));
	m_KineticCombo_sRcReL_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_241_AL_Cap_sRcReL_loop"));
	m_KineticCombo_sRcReL_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_241_AL_Cap_sRcReL_end"));
	m_KineticCombo_sRcReL_End.push_back(pAnimation);

	// sRcReR
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_242_AL_Cap_sRcReR_start"));
	m_KineticCombo_sRcReR_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_242_AL_Cap_sRcReR_loop"));
	m_KineticCombo_sRcReR_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_242_AL_Cap_sRcReR_end"));
	m_KineticCombo_sRcReR_End.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_245_AL_Pcon_cLeR_Lv1"));
	m_KineticCombo_Pcon_cLeR_Lv1.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_246_AL_Pcon_cLeL_Lv1"));
	m_KineticCombo_Pcon_cLeL_Lv1.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_249_AL_Pcon_cReR_Lv1"));
	m_KineticCombo_Pcon_cReR_Lv1.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_250_AL_Pcon_cReL_Lv1"));
	m_KineticCombo_Pcon_cReL_Lv1.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_252_AL_Pcon_cLeR_Lv2"));
	m_KineticCombo_Pcon_cLeR_Lv2.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_253_AL_Pcon_cLeL_Lv2"));
	m_KineticCombo_Pcon_cLeL_Lv2.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_256_AL_Pcon_cReR_Lv2"));
	m_KineticCombo_Pcon_cReR_Lv2.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_257_AL_Pcon_cReL_Lv2"));
	m_KineticCombo_Pcon_cReL_Lv2.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_259_AL_Pcon_cLeR_Lv3"));
	m_KineticCombo_Pcon_cLeR_Lv3.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_260_AL_Pcon_cLeL_Lv3"));
	m_KineticCombo_Pcon_cLeL_Lv3.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_263_AL_Pcon_cReR_Lv3"));
	m_KineticCombo_Pcon_cReR_Lv3.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_264_AL_Pcon_cReL_Lv3"));
	m_KineticCombo_Pcon_cReL_Lv3.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_266_AL_Pcon_cLeR_Lv4"));
	m_KineticCombo_Pcon_cLeR_Lv4.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_267_AL_Pcon_cLeL_Lv4"));
	m_KineticCombo_Pcon_cLeL_Lv4.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_270_AL_Pcon_cReR_Lv4"));
	m_KineticCombo_Pcon_cReR_Lv4.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_271_AL_Pcon_cReL_Lv4"));
	m_KineticCombo_Pcon_cReL_Lv4.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_273_AL_AirCap_start"));
	m_KineticCombo_AirCap.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_279_AL_AirPcon"));
	m_KineticCombo_Pcon.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_228_AL_atk_air_dash1_start"));
	m_KineticCombo_AirSlash.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_228_AL_atk_air_dash1_end"));
	m_KineticCombo_AirSlash.push_back(pAnimation);

m_pKineticComboStateMachine = CFSMComponentBuilder()

		.InitState("KINETIC_COMBO_NOUSE")

		.AddState("KINETIC_COMBO_NOUSE")
		.OnStart([&]() 
		{
			m_bKineticCombo = false;

			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 0.f;

			m_pASM->ClearAnimSocket("Kinetic_Combo_AnimSocket");

			if(!m_bAir)
				m_pASM->SetCurState("IDLE");
			else
				m_pASM->SetCurState("JUMP_FALL");

			// Î¶ºÎùº?¥Ìä∏ Ï¢ÖÎ£å
			m_pCurve = nullptr;

			for (auto& iter : m_pModel->GetMaterials())
			{
				iter->GetParam().Floats[0] = 0.f;
				iter->GetParam().Float4s[0] = { 0.f, 0.f, 0.f, 0.f };
			}

			Event_Trail(false);

			IM_LOG("Kinetic No Use");
		})
		.Tick([&](double fTimeDelta)
		{

		})
		.OnExit([&]()
		{
			m_bKineticMove = false;
			m_bSeperateAnim = false;
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_pASM->SetCurState("IDLE");
			m_pKineticStataMachine->SetState("NO_USE_KINETIC");

			for (auto& iter : m_pModel->GetMaterials())
			{
				iter->GetParam().Float4s[0] = { 0.945098f, 0.4f, 1.f, 1.f };
			}

			Event_Trail(false);
		})

			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_SLASH01", "KINETIC_COMBO_SLASH01")
			.Predicator([&]()->_bool { return !m_bHit && m_bLeftClick && (m_fKineticCombo_Kinetic > 0.f) && !m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_KINETIC01_CHARGE", "KINETIC_COMBO_KINETIC01_CHARGE")
			.Predicator([&]()->_bool { return CanKinetic(15) && !m_bHit && m_bKineticRB && (m_fKineticCombo_Slash > 0.f) && !m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_AIR_CAP", "KINETIC_COMBO_AIR_CAP")
			.Predicator([&]()->_bool 
			{ 
				if (false == CPlayerInfoManager::GetInstance()->Get_BrainMap(EBRAINMAP::BRAINMAP_KINETIC_COMBO_AIR)) return false;
				return CanKinetic(15) && !m_bHit && m_bKineticRB && (m_fKineticCombo_Slash > 0.f) && m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject());
			})
			.Priority(0)

#pragma region ?¨Îûò??ÏΩ§Î≥¥ 1

		.AddState("KINETIC_COMBO_SLASH01")
		.OnStart([&]() 
		{
			m_bActionAble = true;

			m_bSeperateAnim = false;
			m_bKineticMove = false;
			m_bKineticCombo = true;

			KineticObject_Targeting();

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 10.f;

			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				m_pTransformCom->LookAt_NonY(CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
			}

			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash01);
		})
		.Tick([&](double fTimeDelta)
		{
			KineticObject_Targeting();

			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_vector EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
					m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
			}
		})

			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return (m_bLeftClick || m_bWalk || m_bJump || m_bDash) && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_KINETIC01_CHARGE", "KINETIC_COMBO_KINETIC01_CHARGE")
			.Predicator([&]()->_bool {return CanKinetic(15) && m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

#pragma endregion ?¨Îûò??ÏΩ§Î≥¥ 1

#pragma region ?§ÎÑ§??ÏΩ§Î≥¥ 1

		.AddState("KINETIC_COMBO_KINETIC01_CHARGE")
		.OnStart([&]() 
		{
			m_bSeperateAnim = false;
			m_bKineticMove = false;
			m_bKineticCombo = true;

			KineticObject_Targeting();

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
			}

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				LookTarget();
			}

			m_fKineticCombo_Slash = 0.f;
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);

			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_245_AL_Pcon_cLeR_Lv1");
			Kinetic_Combo_MoveToKineticPoint();

			// ?ÑÏãúÎ°??§Î°ú Îπ†Ï?Í≤??òÎäî ÏΩîÎìú
			m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
		})
		.Tick([&](double fTimeDelta)
		{
			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);

				//m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);

				Kinetic_Combo_AttachLerpObject();
			}

			m_fKineticCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
		})

			.AddTransition("KINETIC_COMBO_KINETIC01_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_KINETIC01_CHARGE to KINETIC_COMBO_KINETIC01_CANCEL", "KINETIC_COMBO_KINETIC01_CANCEL")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC01_CHARGE to KINETIC_COMBO_KINETIC01_THROW", "KINETIC_COMBO_KINETIC01_THROW")
			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f); })
			.Priority(1)

		.AddState("KINETIC_COMBO_KINETIC01_CANCEL")
		.OnStart([&]()
		{
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_End);
		})
		.Tick([&](double fTimeDelta)
		{
			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}
		})

			.AddTransition("KINETIC_COMBO_KINETIC01_CANCEL to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_bWalk || m_bDash || m_bLeftClick || m_bKineticRB || m_bJump; })
			.Priority(0)

		.AddState("KINETIC_COMBO_KINETIC01_THROW")
		.OnStart([&]()
		{
			// Î¶ºÎùº?¥Ìä∏ Ïª§Î∏å ?ùÏÑ±
			Start_RimLight("Kinetic_Combo_01_RimLight");

			m_fKineticCharge = 0.f;
			m_fKineticCombo_Kinetic = 10.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv1);

			Kinetic_Combo_KineticAnimation();
		})
		.Tick([&](double fTimeDelta)
		{
			// Î¶ºÎùº?¥Ìä∏ Ïª§Î∏å ?ôÏûë (?†ÎãàÎ©îÏù¥???¨ÏÉù ?çÎèÑ??ÎßûÏ∂∞??
			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
			Tick_RimLight(fRatio);


			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}

			Kinetic_Combo_KineticAnimation();
		})
		.OnExit([&]()
		{
			m_pCurve = nullptr;

			// Î¶ºÎùº?¥Ìä∏ Ï¢ÖÎ£å
			End_RimLight();

		})

			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump || m_bKineticRB || m_bWalk); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_SLASH02", "KINETIC_COMBO_SLASH02")
			.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f) && m_bLeftClick; })
			.Priority(0)

#pragma endregion ?§ÎÑ§??ÏΩ§Î≥¥ 1

#pragma region ?¨Îûò??ÏΩ§Î≥¥ 2

		.AddState("KINETIC_COMBO_SLASH02")
		.OnStart([&]() 
		{
			m_bActionAble = true;
			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 10.f;

			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				LookTarget();
			}

			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash02);
		})
		.Tick([&](double fTimeDelta)
		{
			KineticObject_Targeting();

			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_vector EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
					m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
			}
		})

			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return (m_bLeftClick || m_bDash || m_bJump || m_bWalk) && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_KINETIC02_CHARGE", "KINETIC_COMBO_KINETIC02_CHARGE")
			.Predicator([&]()->_bool {return CanKinetic(15) && m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)


#pragma endregion ?¨Îûò??ÏΩ§Î≥¥ 2

#pragma region ?§ÎÑ§??ÏΩ§Î≥¥ 2

		.AddState("KINETIC_COMBO_KINETIC02_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
			}

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				LookTarget();
			}

			m_bKineticCombo = true;
			m_fKineticCombo_Slash = 0.f;
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);

			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_252_AL_Pcon_cLeR_Lv2");
			Kinetic_Combo_MoveToKineticPoint();

			m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
		})
		.Tick([&](double fTimeDelta)
		{
			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);

				//m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);

				Kinetic_Combo_AttachLerpObject();
			}

			m_fKineticCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
		})
			.AddTransition("KINETIC_COMBO_KINETIC02_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_KINETIC02_CHARGE to KINETIC_COMBO_KINETIC02_CANCEL", "KINETIC_COMBO_KINETIC02_CANCEL")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC02_CHARGE to KINETIC_COMBO_KINETIC02_THROW", "KINETIC_COMBO_KINETIC02_THROW")
			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f); })
			.Priority(1)

		.AddState("KINETIC_COMBO_KINETIC02_CANCEL")
		.OnStart([&]()
		{
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_End);
		})
		.Tick([&](double fTimeDelta)
		{
			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}
		})

			.AddTransition("KINETIC_COMBO_KINETIC02_CANCEL to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_bWalk || m_bDash || m_bLeftClick || m_bKineticRB || m_bJump; })
			.Priority(0)

		.AddState("KINETIC_COMBO_KINETIC02_THROW")
		.OnStart([&]()
		{
			Start_RimLight("Kinetic_Combo_02_RimLight");

			m_fKineticCharge = 0.f;
			m_fKineticCombo_Kinetic = 10.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv2);
		})
		.Tick([&](double fTimeDelta)
		{
			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
			Tick_RimLight(fRatio);

			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}

			Kinetic_Combo_KineticAnimation();
		})
		.OnExit([&]()
		{
			End_RimLight();
		})

			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump || m_bKineticRB || m_bWalk); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_SLASH03", "KINETIC_COMBO_SLASH03")
			.Predicator([&]()->_bool {return m_bLeftClick; })
			.Priority(0)

#pragma endregion ?§ÎÑ§??ÏΩ§Î≥¥ 2

#pragma region ?¨Îûò??ÏΩ§Î≥¥ 3

		.AddState("KINETIC_COMBO_SLASH03")
		.OnStart([&]() 
		{
			m_bActionAble = true;

			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 10.f;

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				LookTarget();
			}

			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash03);
		})
		.Tick([&](double fTimeDelta)
		{
			KineticObject_Targeting();

			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_vector EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
					m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
			}
		})

			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return (m_bLeftClick || m_bWalk || m_bJump || m_bDash) && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.25f); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_KINETIC03_CHARGE", "KINETIC_COMBO_KINETIC03_CHARGE")
			.Predicator([&]()->_bool {return CanKinetic(15) && m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.25f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

#pragma endregion ?¨Îûò??ÏΩ§Î≥¥ 3

#pragma region ?§ÎÑ§??ÏΩ§Î≥¥ 3

		.AddState("KINETIC_COMBO_KINETIC03_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
			}

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				LookTarget();
			}

			m_bKineticCombo = true;
			m_fKineticCombo_Slash = 0.f;
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);

			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_259_AL_Pcon_cLeR_Lv3");
			Kinetic_Combo_MoveToKineticPoint();

			m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
		})
		.Tick([&](double fTimeDelta)
		{
			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);

				//m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);

				Kinetic_Combo_AttachLerpObject();
			}

			m_fKineticCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
		})

			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_KINETIC03_CANCEL", "KINETIC_COMBO_KINETIC03_CANCEL")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_KINETIC03_THROW", "KINETIC_COMBO_KINETIC03_THROW")
			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(1)

		.AddState("KINETIC_COMBO_KINETIC03_CANCEL")
		.OnStart([&]()
		{
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_End);
		})
		.Tick([&](double fTimeDelta)
		{
			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}
		})

			.AddTransition("KINETIC_COMBO_KINETIC03_CANCEL to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_bWalk || m_bDash || m_bLeftClick || m_bKineticRB || m_bJump; })
			.Priority(0)

		.AddState("KINETIC_COMBO_KINETIC03_THROW")
		.OnStart([&]()
		{
			Start_RimLight("Kinetic_Combo_03_RimLight");

			m_fKineticCharge = 0.f;
			m_fKineticCombo_Kinetic = 10.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv3);
		})
		.Tick([&](double fTimeDelta)
		{
			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
			Tick_RimLight(fRatio);

			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}

			Kinetic_Combo_KineticAnimation();
		})
		.OnExit([&]()
		{
			End_RimLight();
		})

			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump || m_bKineticRB || m_bWalk); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_SLASH04", "KINETIC_COMBO_SLASH04")
			.Predicator([&]()->_bool 
			{
				if (false == CPlayerInfoManager::GetInstance()->Get_BrainMap(EBRAINMAP::BRAINMAP_KINETIC_COMBO_4)) return false;
				return m_bLeftClick;
			})
			.Priority(0)


#pragma endregion ?§ÎÑ§??ÏΩ§Î≥¥ 3

#pragma region ?¨Îûò??ÏΩ§Î≥¥ 4

		.AddState("KINETIC_COMBO_SLASH04")
		.OnStart([&]() 
		{
			m_bActionAble = true;

			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 10.f;

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				LookTarget();
			}

			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash04);
		})
		.Tick([&](double fTimeDelta)
		{
			KineticObject_Targeting();

			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_vector EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
					m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
			}
		})

			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return (m_bLeftClick || m_bJump || m_bDash || m_bWalk) && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_KINETIC04_CHARGE", "KINETIC_COMBO_KINETIC04_CHARGE")
			.Predicator([&]()->_bool {return CanKinetic(15) && m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

#pragma endregion ?¨Îûò??ÏΩ§Î≥¥ 4

#pragma region ?§ÎÑ§??ÏΩ§Î≥¥ 4

		.AddState("KINETIC_COMBO_KINETIC04_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
			}

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				LookTarget();
			}

			m_bKineticCombo = true;
			m_fKineticCombo_Slash = 0.f;
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);

			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_266_AL_Pcon_cLeR_Lv4");
			Kinetic_Combo_MoveToKineticPoint();

			m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
		})
		.Tick([&](double fTimeDelta)
		{
			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				//m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);

				_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);

				Kinetic_Combo_AttachLerpObject();
			}

			m_fKineticCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
		})

			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_KINETIC04_CANCEL", "KINETIC_COMBO_KINETIC04_CANCEL")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_KINETIC04_THROW", "KINETIC_COMBO_KINETIC04_THROW")
			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(1)

		.AddState("KINETIC_COMBO_KINETIC04_CANCEL")
		.OnStart([&]()
		{
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_End);
		})
		.Tick([&](double fTimeDelta)
		{
			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}
		})

			.AddTransition("KINETIC_COMBO_KINETIC04_CANCEL to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_bWalk || m_bDash || m_bLeftClick || m_bKineticRB || m_bJump; })
			.Priority(0)

		.AddState("KINETIC_COMBO_KINETIC04_THROW")
		.OnStart([&]()
		{
			Start_RimLight("Kinetic_Combo_04_RimLight");
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv4);
		})
		.Tick([&](double fTimeDelta)
		{
			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
			Tick_RimLight(fRatio);

			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}

			Kinetic_Combo_KineticAnimation();
		})
		.OnExit([&]()
		{
			End_RimLight();
		})

			.AddTransition("KINETIC_COMBO_KINETIC04_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC04_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.8f) && (m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB); })
			.Priority(0)


#pragma endregion ?§ÎÑ§??ÏΩ§Î≥¥ 4

#pragma region Í≥µÏ§ë ÏΩ§Î≥¥ ?§ÎÑ§??

			.AddState("KINETIC_COMBO_AIR_CAP")
			.OnStart([&]() 
			{
				m_bSeperateAnim = false;
				m_bKineticMove = false;
				m_bKineticCombo = true;

				SetAbleState({ false, false, false, false, true, false, true, true, true, false });

				m_pASM->SetCurState("JUMP_FALL");
				m_pKineticStataMachine->SetState("NO_USE_KINETIC");

				KineticObject_Targeting();

				if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
					Enemy_Targeting(true);

				if (CPlayerInfoManager::GetInstance()->Get_KineticObject())
				{
					m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
				}

				if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					LookTarget();
				}

				m_fKineticCombo_Slash = 0.f;
				m_fKineticCharge = 0.f;
				m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_AirCap);

				m_pKineticAnimModel->SetPlayAnimation("AS_no0000_279_AL_AirPcon");
				Kinetic_Combo_MoveToKineticPoint();

				// ?ÑÏãúÎ°??§Î°ú Îπ†Ï?Í≤??òÎäî ÏΩîÎìú
				m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
			})
			.Tick([&](double fTimeDelta)
			{
				if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

					_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

					_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
					m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);

					Kinetic_Combo_AttachLerpObject();
				}

				if (m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket") != nullptr)
					m_fKineticCharge = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
				else
					m_fKineticCharge = 0.f;

				CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
			})
			.OnExit([&]()
			{
				m_fKineticCharge = 0.f;
				CPlayerInfoManager::GetInstance()->Set_KineticCharge(m_fKineticCharge);
			})

				.AddTransition("KINETIC_COMBO_AIR_CAP to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
				.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
				.Priority(0)

				.AddTransition("KINETIC_COMBO_AIR_CAP to KINETIC_COMBO_AIR_PCON", "KINETIC_COMBO_AIR_PCON")
				.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
				.Priority(0)

			.AddState("KINETIC_COMBO_AIR_PCON")
			.OnStart([&]()
			{
				// Î¶ºÎùº?¥Ìä∏ Ïª§Î∏å ?ùÏÑ±
				Start_RimLight("Kinetic_Combo_01_RimLight");

				if (CPlayerInfoManager::GetInstance()->Get_KineticObject())
				{
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_ToAir(true);
				}

				m_fKineticCharge = 0.f;
				m_fKineticCombo_Kinetic = 0.f;
				m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon);

				Kinetic_Combo_KineticAnimation();
			})
			.Tick([&](double fTimeDelta)
			{
				// Î¶ºÎùº?¥Ìä∏ Ïª§Î∏å ?ôÏûë (?†ÎãàÎ©îÏù¥???¨ÏÉù ?çÎèÑ??ÎßûÏ∂∞??
				_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
				Tick_RimLight(fRatio);

				/*if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
						->Set_Gravity(false);
				}*/

				if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
				{
					string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
					_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
					m_pTransformCom->LocalMove(vLocal);
				}

				Kinetic_Combo_KineticAnimation();
			})
			.OnExit([&]()
			{
				m_pCurve = nullptr;

				// Î¶ºÎùº?¥Ìä∏ Ï¢ÖÎ£å
				End_RimLight();

				m_bActiveGravity = true;

			/*	if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
						->Set_Gravity(true);
				}*/
			})

			.AddTransition("KINETIC_COMBO_AIR_PCON to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_AIR_PCON to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_AIR_PCON to KINETIC_COMBO_AIRSLASH", "KINETIC_COMBO_AIRSLASH")
			.Predicator([&]()->_bool { return m_bLeftClick && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5625f); })
			.Priority(0)

			.AddState("KINETIC_COMBO_AIRSLASH")
			.OnStart([&]()
			{
				m_bActionAble = true;

				m_bActiveGravity = false;

				if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					m_pTransformCom->LookAt_NonY(CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
				}

				m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_AirSlash);

			})
			.Tick([&](double fTimeDelta)
			{
				KineticObject_Targeting();

				if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					_float4 EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

					EnemyPos.y = vMyPos.y;

					_vector vDirEnemy = XMLoadFloat4(&EnemyPos) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

					_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

					if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
						m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
				}
			})
			.OnExit([&]()
			{
				m_bActiveGravity = true;
			})

				.AddTransition("KINETIC_COMBO_AIRSLASH to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
				.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
				.Priority(0)

				.AddTransition("KINETIC_COMBO_AIRSLASH to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
				.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
				.Priority(0)

				.AddTransition("KINETIC_COMBO_AIRSLASH to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
				.Predicator([&]()->_bool { return m_bLeftClick && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7273f); })
				.Priority(0)


#pragma endregion Í≥µÏ§ë ÏΩ§Î≥¥ ?§ÎÑ§??

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_JustDodgeStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_207_AL_atk_justdodge"));
	m_JustDodgeSlash.push_back(m_pModel->Find_Animation("AS_ch0100_207_AL_atk_justdodge"));

	m_pJustDodgeStateMachine = CFSMComponentBuilder()
		.InitState("JUSTDODGE_NONUSE")

		.AddState("JUSTDODGE_NONUSE")
			.OnStart([&]()
			{
				m_pASM->ClearAnimSocket("JustDodge_AnimSocket");
				m_pASM->SetCurState("IDLE");
			})
			.OnExit([&]()
			{
			})
			.AddTransition("JUSTDODGE_NONUSE to JUSTDODGE_USABLE", "JUSTDODGE_USABLE")
			.Predicator([&]()->_bool { return m_fJustDodgeAble > 0.f; })
			.Priority(0)

		.AddState("JUSTDODGE_USABLE")
			.OnStart([&]()
			{
				CGameInstance::GetInstance()->SetTimeRatioCurve("JustDodge_Income");	// ?¨Î°ú??
			})
			.Tick([&](double fTimeDelta)
			{
				//IM_LOG("%f", m_fJustDodgeAble);
				//IM_LOG("JUSTDODGE!!");
				// ?Ä?§Ìä∏?∑Ï? Ï°∞Í±¥ Î∞úÏÉù ???åÎ¶¥ Tick?®Ïàò
			})
			.OnExit([&]()
			{
				CGameInstance::GetInstance()->ResetTimeRatio();
				m_pASM->SetCurState("IDLE");

				m_fJustDodgeAble = 0.f;
			})
			
				.AddTransition("JUSTDODGE_USABLE to JUSEDODGE_NONUSE", "JUSTDODGE_NONUSE")
				.Predicator([&]()->_bool{ return m_fJustDodgeAble <= 0.f; })
				.Priority(0)

				.AddTransition("JUSTDODGE_USABLE to JUSEDODGE_NONUSE", "JUSTDODGE_NONUSE")
				.Predicator([&]()->_bool { return m_bDash || m_bJump; })
				.Priority(0)

				.AddTransition("JUSTDODGE_USABLE to JUSTDODGE_SLASH", "JUSTDODGE_SLASH")
				.Predicator([&]()->_bool { return m_bLeftClick; })
				.Priority(0)

		.AddState("JUSTDODGE_SLASH")
			.OnStart([&]()
			{
				m_bJustDodge_Activate = true;
				if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					LookTarget();
				}

				m_fJustDodgeAble = 0.f;
				SetAbleState({ false, false, false ,false, true, true, true, true, true, false });
				m_pASM->SetCurState("IDLE");
				m_pASM->AttachAnimSocket("JustDodge_AnimSocket", m_JustDodgeSlash);
			})
			.Tick([&](double fTimeDelta)
			{
				// ?Ä?§Ìä∏?∑Ï? ?ºÎãà??Î∞úÏÉù ???åÎ¶¥ Tick?®Ïàò
			})
			.OnExit([&]()
			{
				m_bJustDodge_Activate = false;
			})

				.AddTransition("JUSTDODGE_SLASH to JUSTDODGE_NONUSE", "JUSTDODGE_NONUSE")
				.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("JustDodge_AnimSocket"); })
				.Priority(0)

				.AddTransition("JUSTDODGE_SLASH to JUSTDODGE_NONUSE", "JUSTDODGE_NONUSE")
				.Predicator([&]()->_bool { return m_pASM->isSocketPassby("JustDodge_AnimSocket", 0.5f) && (m_bLeftClick || m_bKineticRB || m_bDash || m_bWalk || m_bJump); })
				.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_BrainFieldKineticStateMachine()
{
	CAnimation* pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_646_AL_BF_throw1_move"));
	m_BF_KineticThrow_A.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_647_AL_BF_throw2_move"));
	m_BF_KineticThrow_B.push_back(pAnimation);


	m_pBrainFieldKineticStateMachine =
		CFSMComponentBuilder().InitState("BF_NO_USE_KINETIC")

		.AddState("BF_NO_USE_KINETIC")
		.OnStart([&]()
		{
			m_pASM->ClearAnimSocket("Kinetic_AnimSocket");
			m_pASM->SetCurState_BrainField(m_pASM->GetCurStateName());
			m_bKineticMove = false;
		})
		.Tick([&](double TimeDelta)
		{
			BF_Throw.Reset();
			m_bKineticMove = false;

			if (m_pBrainFieldAttackStateMachine->GetCurStateName() == "NO_USE_BRAINFIELD" &&
				m_pBrainFieldKineticStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC" &&
				m_pBrainFieldKineticComboStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_COMBO" &&
				m_pBrainFieldFallStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_FALL")
			{
				KineticObject_Targeting();
			}
		})

			.AddTransition("BF_NO_USE_KINETIC to BF_KINETIC_THROW_A", "BF_KINETIC_THROW_A")
			.Predicator([&]()->_bool 
			{
				return m_bKineticRB && 
					!m_bAir && 
					(nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) && 
					!m_bHit; 
			})
			.Priority(0)


		.AddState("BF_KINETIC_THROW_A")
		.OnStart([&]() 
		{
			Enemy_Targeting(true);
			m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_BF_KineticThrow_A);
			static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->SetParticle();
		})
		.Tick([&](double fTimeDelta)
		{
			m_bKineticMove = true;

			if (m_pASM->isSocketPassby("Kinetic_AnimSocket", 0.25f) && BF_Throw.IsNotDo())
				Event_Kinetic_Throw();

			if(m_pASM->isSocketPassby("Kinetic_AnimSocket", 0.3f))
				KineticObject_Targeting();
		})
		.OnExit([&]()
		{
			BF_Throw.Reset();
		})
			.AddTransition("BF_KINETIC_THROW_A to BF_NO_USE_KINETIC", "BF_NO_USE_KINETIC")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_AnimSocket"); })
			.Priority(0)

			.AddTransition("BF_KINETIC_THROW_A to BF_NO_USE_KINETIC", "BF_NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket"); })
			.Priority(0)
			
			.AddTransition("BF_KINETIC_THROW_A to BF_KINETIC_THROW_B", "BF_KINETIC_THROW_B")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_AnimSocket", 0.4f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

		.AddState("BF_KINETIC_THROW_B")
		.OnStart([&]() 
		{
			Enemy_Targeting(true);
			m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_BF_KineticThrow_B);
			static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->SetParticle();
		})
		.Tick([&](double fTimeDelta)
		{
			m_bKineticMove = true;

			if (m_pASM->isSocketPassby("Kinetic_AnimSocket", 0.25f) && BF_Throw.IsNotDo())
				Event_Kinetic_Throw();

			if (m_pASM->isSocketPassby("Kinetic_AnimSocket", 0.3f))
				KineticObject_Targeting();
		})
		.OnExit([&]()
		{
			BF_Throw.Reset();
		})
			.AddTransition("BF_KINETIC_THROW_B to BF_NO_USE_KINETIC", "BF_NO_USE_KINETIC")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_AnimSocket"); })
			.Priority(0)

			.AddTransition("BF_KINETIC_THROW_B to BF_NO_USE_KINETIC", "BF_NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket"); })
			.Priority(0)

			.AddTransition("BF_KINETIC_THROW_B to BF_KINETIC_THROW_A", "BF_KINETIC_THROW_A")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_AnimSocket", 0.4f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_BrainFieldKineticComboStateMachine()
{
	CAnimation* pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_611_AL_BF_swing_dash"));
	m_BF_KineticCombo_Slash.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_631_AL_BF_throw_b1"));
	m_BF_KineticCombo_Kinetic.push_back(pAnimation);

	m_pBrainFieldKineticComboStateMachine = 
		CFSMComponentBuilder().InitState("BF_NO_USE_KINETIC_COMBO")

		.AddState("BF_NO_USE_KINETIC_COMBO")
		.OnStart([&]()
		{
			m_bKineticCombo = false;
			m_pASM->ClearAnimSocket("BrainField_AnimSocket");
			m_pASM->SetCurState_BrainField("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			m_bKineticMove = false;

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
					->Set_Dynamic();
			}
		})
		.Tick([&](double TimeDelta)
		{
			if (m_pBrainFieldAttackStateMachine->GetCurStateName() == "NO_USE_BRAINFIELD" &&
				m_pBrainFieldKineticStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC" &&
				m_pBrainFieldKineticComboStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_COMBO" &&
				m_pBrainFieldFallStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_FALL")
			{
				KineticObject_Targeting();
			}
		})
		.OnExit([&]() 
		{
			m_bKineticMove = false;
			m_bSeperateAnim = false;

			m_pASM->SetCurState_BrainField("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_pBrainFieldKineticStateMachine->SetState("BF_NO_USE_KINETIC");
			m_pASM->ClearAnimSocket("Kinetic_AnimSocket");

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
					->Set_Trigger(true);
			}
		})

			.AddTransition("BF_NO_USE_KINETIC_COMBO to BF_KINETIC_COMBO_SLASH", "BF_KINETIC_COMBO_SLASH")
			.Predicator([&]()->_bool 
			{
				return m_bLeftClick &&
					!m_bAir && 
					(nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) && 
					!m_bHit &&
					!m_pASM->isSocketEmpty("Kinetic_AnimSocket");
			})
			.Priority(0)

			.AddTransition("BF_NO_USE_KINETIC_COMBO to BF_KINETIC_COMBO_KINETIC", "BF_KINETIC_COMBO_KINETIC")
			.Predicator([&]()->_bool 
			{
				return m_bKineticRB &&
					!m_bAir && 
					(nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) && 
					!m_bHit &&
					   (m_pASM->GetCurStateName_BrainField() == "ATK_A1" ||
						m_pASM->GetCurStateName_BrainField() == "ATK_A2" ||
						m_pASM->GetCurStateName_BrainField() == "ATK_A3" ||
						m_pASM->GetCurStateName_BrainField() == "ATK_AIR_LANDING");
			})
			.Priority(0)

		.AddState("BF_KINETIC_COMBO_SLASH")
			.OnStart([&]()
			{
				m_bKineticCombo = true;
				m_pASM->AttachAnimSocket("BrainField_AnimSocket", m_BF_KineticCombo_Slash);
				m_pKineticAnimModel->SetPlayAnimation("AS_ch0100_611_AL_BF_swing_dash_obj");

				if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
				{
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
						->Set_Trigger(true);
				}

				LookTarget();
			})
			.Tick([&](double TimeDelta)
			{
				if (BF_BeginTick.IsNotDo())
				{
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
						->Set_Trigger(true);
				}

				BrainField_ObjectAnimation_Socket("BrainField_AnimSocket", 5.f, 1.f);
			})
			.OnExit([&]() 
			{
				BF_BeginTick.Reset();
				m_pASM->SetCurState_BrainField("IDLE");
				SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})
				.AddTransition("BF_KINETIC_COMBO_SLASH to BF_NO_USE_KINETIC_COMBO", "BF_NO_USE_KINETIC_COMBO")
				.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("BrainField_AnimSocket"); })
				.Priority(0)

				.AddTransition("BF_KINETIC_COMBO_SLASH to BF_NO_USE_KINETIC_COMBO", "BF_NO_USE_KINETIC_COMBO")
				.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("BrainField_AnimSocket"); })
				.Priority(0)

				.AddTransition("BF_KINETIC_COMBO_SLASH to BF_NO_USE_KINETIC_COMBO", "BF_NO_USE_KINETIC_COMBO")
				.Predicator([&]()->_bool { return m_pASM->isSocketPassby("BrainField_AnimSocket", 0.5f) && (m_bLeftClick || m_bWalk || m_bDash || m_bJump); })
				.Priority(0)

				.AddTransition("BF_KINETIC_COMBO_SLASH to BF_KINETIC_COMBO_KINETIC", "BF_KINETIC_COMBO_KINETIC")
				.Predicator([&]()->_bool
				{
					return m_bKineticRB &&
						(nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) &&
						m_pASM->isSocketPassby("BrainField_AnimSocket", 0.5f);
				})
				.Priority(0)

		.AddState("BF_KINETIC_COMBO_KINETIC")
			.OnStart([&]()
			{
				m_bKineticCombo = true;
				m_pASM->AttachAnimSocket("BrainField_AnimSocket", m_BF_KineticCombo_Kinetic);
				m_pKineticAnimModel->SetPlayAnimation("AS_ch0100_631_AL_BF_throw_b1_obj");

				if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
				{
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
						->Set_Trigger(true);
				}

				LookTarget();
			})
			.Tick([&](double TimeDelta)
			{
				if (BF_BeginTick.IsNotDo())
				{
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
						->Set_Trigger(true);
				}

				if (m_pASM->isSocketPassby("BrainField_AnimSocket", 0.35f) && BF_Combo_Throw.IsNotDo())
				{
					Event_Kinetic_Throw();
				}
				else if (false == m_pASM->isSocketPassby("BrainField_AnimSocket", 0.35f))
				{
					BrainField_ObjectAnimation_Socket("BrainField_AnimSocket", 0.f, 1.f);
				}

				if (m_pASM->isSocketPassby("BrainField_AnimSocket", 0.5f) && BF_Retarget.IsNotDo())
				{
					KineticObject_Targeting();
				}
			})
			.OnExit([&]() 
			{
				BF_Retarget.Reset();
				BF_Combo_Throw.Reset();
				BF_BeginTick.Reset();
				m_pASM->SetCurState_BrainField("IDLE");
				SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})
				.AddTransition("BF_KINETIC_COMBO_KINETIC to BF_NO_USE_KINETIC_COMBO", "BF_NO_USE_KINETIC_COMBO")
				.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("BrainField_AnimSocket"); })
				.Priority(0)

				.AddTransition("BF_KINETIC_COMBO_KINETIC to BF_NO_USE_KINETIC_COMBO", "BF_NO_USE_KINETIC_COMBO")
				.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("BrainField_AnimSocket"); })
				.Priority(0)

				.AddTransition("BF_KINETIC_COMBO_KINETIC to BF_NO_USE_KINETIC_COMBO", "BF_NO_USE_KINETIC_COMBO")
				.Predicator([&]()->_bool { return m_pASM->isSocketPassby("BrainField_AnimSocket", 0.5f) && (m_bKineticRB || m_bWalk || m_bDash || m_bJump); })
				.Priority(0)

				.AddTransition("BF_KINETIC_COMBO_KINETIC to BF_KINETIC_COMBO_SLASH", "BF_KINETIC_COMBO_SLASH")
				.Predicator([&]()->_bool
				{
					return m_bLeftClick &&
						(nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) &&
						m_pASM->isSocketPassby("BrainField_AnimSocket", 0.5f);
				})
				.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_BrainFieldAttackStateMachine()
{
	// ∫Í∑π¿Œ« µÂ ∞¯∞›¿∫ ø∞∑¬√ºø° æ÷¥œ∏ﬁ¿Ãº«¿ª ≈¬øÏ¥¬ πÊΩƒ
	// «ˆ¿Á ASM¿ª πﬁæ∆º≠ ø∞∑¬√ºø° æ÷¥œ∏ﬁ¿Ãº«¿ª ¿¸¥ﬁ«œ¥¬ FSM

	m_pBrainFieldAttackStateMachine =
		CFSMComponentBuilder().InitState("NO_USE_BRAINFIELD")

		.AddState("NO_USE_BRAINFIELD")
		.OnStart([&]()
		{
			m_pKineticAnimModel->GetPlayAnimation()->Reset();

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				
				if (m_pBrainFieldKineticStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC" &&
					m_pBrainFieldKineticComboStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_COMBO" &&
					m_pBrainFieldFallStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_FALL")
				{
					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
						->Set_Dynamic();
				}
			}
		})
		.Tick([&](double TimeDelta)
		{
			if (m_pBrainFieldAttackStateMachine->GetCurStateName() == "NO_USE_BRAINFIELD" &&
				m_pBrainFieldKineticStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC" &&
				m_pBrainFieldKineticComboStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_COMBO" &&
				m_pBrainFieldFallStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_FALL")
			{
				KineticObject_Targeting();
			}
		})
		.OnExit([&]() 
		{
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
					->Set_Trigger(true);
			}
		})
			.AddTransition("NO_USE_BRAINFIELD to BRAINFIELD_ATK_A1", "BRAINFIELD_ATK_A1")
			.Predicator([&]()->_bool {return (m_pASM->GetCurStateName_BrainField() == "ATK_A1"); })
			.Priority(0)

			.AddTransition("NO_USE_BRAINFIELD to BRAINFIELD_ATK_A2", "BRAINFIELD_ATK_A2")
			.Predicator([&]()->_bool {return (m_pASM->GetCurStateName_BrainField() == "ATK_A2"); })
			.Priority(0)

			.AddTransition("NO_USE_BRAINFIELD to BRAINFIELD_ATK_A3", "BRAINFIELD_ATK_A3")
			.Predicator([&]()->_bool {return (m_pASM->GetCurStateName_BrainField() == "ATK_A3"); })
			.Priority(0)

			.AddTransition("NO_USE_BRAINFIELD to BRAINFIELD_ATK_AIR_START", "BRAINFIELD_ATK_AIR_START")
			.Predicator([&]()->_bool {return (m_pASM->GetCurStateName_BrainField() == "ATK_AIR_START"); })
			.Priority(0)

		.AddState("BRAINFIELD_ATK_A1")
		.OnStart([&]()	
		{
			m_pKineticAnimModel->SetPlayAnimation("AS_ch0100_601_AL_BF_swing_a1_obj");
		})
		.Tick([&](double TimeDelta)
		{
			// ∫¸∏£∞‘ ≈∞≥◊∆Ω ∆˜¿Œ∆Æ∑Œ ¿ßƒ° ∫∏∞£«ÿ¡÷∞Ì ø¨µø
			BrainField_ObjectAnimation(5.f, 1.f);

		})
		.OnExit([&]() 
		{

		})
			.AddTransition("BRAINFIELD_ATK_A1 to NO_USE_BRAINFIELD", "NO_USE_BRAINFIELD")
			.Predicator([&]()->_bool {return (m_pASM->GetCurStateName_BrainField() != "ATK_A1"); })
			.Priority(0)

		.AddState("BRAINFIELD_ATK_A2")
		.OnStart([&]()	
		{
			m_pKineticAnimModel->SetPlayAnimation("AS_ch0100_602_AL_BF_swing_a2_obj");
		})
		.Tick([&](double TimeDelta)
		{
			BrainField_ObjectAnimation(5.f, 1.f);
		})
		.OnExit([&]() 
		{

		})
			.AddTransition("BRAINFIELD_ATK_A2 to NO_USE_BRAINFIELD", "NO_USE_BRAINFIELD")
			.Predicator([&]()->_bool {return (m_pASM->GetCurStateName_BrainField() != "ATK_A2"); })
			.Priority(0)

		.AddState("BRAINFIELD_ATK_A3")
		.OnStart([&]()	
		{
			m_pKineticAnimModel->SetPlayAnimation("AS_ch0100_603_AL_BF_swing_a3_obj");
		})
		.Tick([&](double TimeDelta)
		{
			BrainField_ObjectAnimation(5.f, 1.f);
		})
		.OnExit([&]() 
		{

		})
			.AddTransition("BRAINFIELD_ATK_A3 to NO_USE_BRAINFIELD", "NO_USE_BRAINFIELD")
			.Predicator([&]()->_bool {return (m_pASM->GetCurStateName_BrainField() != "ATK_A3"); })
			.Priority(0)

		.AddState("BRAINFIELD_ATK_AIR_START")
		.OnStart([&]()	
		{
			m_pKineticAnimModel->SetPlayAnimation("AS_ch0100_621_AL_BF_swing_air_start_obj");
		})
		.Tick([&](double TimeDelta)
		{
			BrainField_ObjectAirSwing(5.f, 1.f);
		})
		.OnExit([&]() 
		{

		})
			.AddTransition("BRAINFIELD_ATK_AIR_START to NO_USE_BRAINFIELD", "NO_USE_BRAINFIELD")
			.Predicator([&]()->_bool {return (m_pASM->GetCurStateName_BrainField() != "ATK_AIR_START") &&
											 (m_pASM->GetCurStateName_BrainField() != "ATK_AIR_LOOP") &&
											 (m_pASM->GetCurStateName_BrainField() != "ATK_AIR_LANDING"); })
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_BrainFieldFallStateMachine()
{
	CAnimation* pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_671_AL_BF_fall_start"));
	m_BF_Fall_Charge.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_672_AL_BF_fall_loop"));
	m_BF_Fall_Charge.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_674_AL_BF_fall_cancel"));
	m_BF_Fall_Cancel.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_673_AL_BF_fall_end"));
	m_BF_Fall_Finish.push_back(pAnimation);
	
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_666_AL_BF_fall_fin_start"));
	m_BF_Fall_Fin_Charge.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_667_AL_BF_fall_fin_loop"));
	m_BF_Fall_Fin_Charge.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_669_AL_BF_fall_fin_cancel"));
	m_BF_Fall_Fin_Cancel.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_668_AL_BF_fall_fin_end"));
	m_BF_Fall_Fin_Finish.push_back(pAnimation);

	m_pBrainFieldFallStateMachine =
		CFSMComponentBuilder().InitState("BF_NO_USE_KINETIC_FALL")

		.AddState("BF_NO_USE_KINETIC_FALL")
		.OnStart([&]()
		{
			m_pASM->ClearAnimSocket("BrainField_AnimSocket");
			m_pASM->SetCurState_BrainField("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_bKineticSpecial_Activate = false;
			m_bKineticSpecial = false;
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
					->Set_Dynamic();
			}
		})
		.Tick([&](double TimeDelta)
		{
			if (m_pBrainFieldAttackStateMachine->GetCurStateName() == "NO_USE_BRAINFIELD" &&
				m_pBrainFieldKineticStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC" &&
				m_pBrainFieldKineticComboStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_COMBO" &&
				m_pBrainFieldFallStateMachine->GetCurStateName() == "BF_NO_USE_KINETIC_FALL")
			{
				KineticObject_Targeting();
			}
		})
		.OnExit([&]() 
		{
			m_bKineticMove = false;
			m_bSeperateAnim = false;

			m_pASM->SetCurState_BrainField("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_pBrainFieldKineticStateMachine->SetState("BF_NO_USE_KINETIC");
			m_pASM->ClearAnimSocket("Kinetic_AnimSocket");
		})
			
			.AddTransition("BF_NO_USE_KINETIC_FALL to BF_FALL_CHARGE", "BF_FALL_CHARGE")
			.Predicator([&]()->_bool 
			{
				return m_bKineticG &&
					!m_bAir && 
					(nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) && 
					!m_bHit;
			})
			.Priority(1)

			.AddTransition("BF_NO_USE_KINETIC_FALL to BF_FALL_FIN_CHARGE", "BF_FALL_FIN_CHARGE")
			.Predicator([&]()->_bool 
			{
				return m_pASM->GetCurStateName_BrainField() == "ATK_A3" &&
					m_bKineticG &&
					!m_bAir && 
					(nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) && 
					!m_bHit;
			})
			.Priority(0)
			

		.AddState("BF_FALL_CHARGE")
		.OnStart([&]() 
		{
			m_bKineticSpecial = true;
			m_pASM->AttachAnimSocket("BrainField_AnimSocket", m_BF_Fall_Charge);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();

			BrainField_FindGatherObjectList();
		})
		.Tick([&](double fTimeDelta)
		{
			BrainField_ObjectGather(m_fBF_Range, m_fBF_Height);
		})
		.OnExit([&]()
		{
			m_fGatherLerp = 0.f;
		})
			.AddTransition("BF_FALL_CHARGE to BF_FALL_FINISH", "BF_FALL_FINISH")
			.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("BrainField_AnimSocket"); })
			.Priority(0)

			.AddTransition("BF_FALL_CHARGE to BF_FALL_CANCEL", "BF_FALL_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)

		.AddState("BF_FALL_CANCEL")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_pASM->AttachAnimSocket("BrainField_AnimSocket", m_BF_Fall_Cancel);
			m_GatherObjectList.clear();

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
					->Set_Dynamic();
			}
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})
			.AddTransition("BF_FALL_CANCEL to BF_NO_USE_KINETIC_FALL", "BF_NO_USE_KINETIC_FALL")
			.Predicator([&]()->_bool { return (m_pASM->isSocketAlmostFinish("BrainField_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG);  })
			.Priority(0)

		.AddState("BF_FALL_FINISH")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("BrainField_AnimSocket", m_BF_Fall_Finish);

			for (auto& iter : m_GatherObjectList)
			{
				static_cast<CMapKinetic_Object*>(iter)->Set_Trigger(true);

				_float4 vIterPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_float4 vTargetPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

				_float4 vDir = vTargetPos - vIterPos;
				m_vecGatherOverlap.push_back(vDir);
			}

		})
		.Tick([&](double fTimeDelta)
		{
			if (nullptr == m_pASM->GetSocketAnimation("BrainField_AnimSocket")) return;

			_float fRatio = m_pASM->GetSocketAnimation("BrainField_AnimSocket")->GetPlayRatio();

			if (0.4f <= fRatio)
			{
				BrainField_ObjectGather_Throw();
			}
			if ((0.4f <= fRatio) && BF_Fall_Throw.IsNotDo())
			{
				Event_Kinetic_Throw();
			}
		})
		.OnExit([&]()
		{
			BF_Fall_Throw.Reset();
			m_pCenterObject = nullptr;

			for (auto& iter : m_GatherObjectList)
			{
				if (false == CGameInstance::GetInstance()->Check_ObjectAlive(iter))
					continue;

				if (auto KineticCheck = dynamic_cast<CMapKinetic_Object*>(iter))
				{
					KineticCheck->SetThrow();
					KineticCheck->Set_Dynamic();
					KineticCheck->Set_Hit();
				}
			}

			m_GatherObjectList.clear();
			m_vecGatherOverlap.clear();
		})
			.AddTransition("BF_FALL_FINISH to BF_NO_USE_KINETIC_FALL", "BF_NO_USE_KINETIC_FALL")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("BrainField_AnimSocket");
			})
			.Priority(0)

/////////////////////////////////////////////////////////////////////////////////////////

		.AddState("BF_FALL_FIN_CHARGE")
		.OnStart([&]() 
		{
			m_bKineticSpecial = true;
			m_pASM->AttachAnimSocket("BrainField_AnimSocket", m_BF_Fall_Charge);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();

			BrainField_FindGatherObjectList();
		})
		.Tick([&](double fTimeDelta)
		{
			BrainField_ObjectGather(m_fBF_Range, m_fBF_Height);
		})
		.OnExit([&]()
		{
			m_fGatherLerp = 0.f;
		})
			.AddTransition("BF_FALL_FIN_CHARGE to BF_FALL_FIN_FINISH", "BF_FALL_FIN_FINISH")
			.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("BrainField_AnimSocket"); })
			.Priority(0)

			.AddTransition("BF_FALL_FIN_CHARGE to BF_FALL_FIN_CANCEL", "BF_FALL_FIN_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)

		.AddState("BF_FALL_FIN_CANCEL")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_pASM->AttachAnimSocket("BrainField_AnimSocket", m_BF_Fall_Fin_Cancel);
			m_GatherObjectList.clear();

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())
					->Set_Dynamic();
			}
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})
			.AddTransition("BF_FALL_FIN_CANCEL to BF_NO_USE_KINETIC_FALL", "BF_NO_USE_KINETIC_FALL")
			.Predicator([&]()->_bool { return (m_pASM->isSocketAlmostFinish("BrainField_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG);  })
			.Priority(0)

		.AddState("BF_FALL_FIN_FINISH")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("BrainField_AnimSocket", m_BF_Fall_Fin_Finish);
			m_pKineticAnimModel->SetPlayAnimation("AS_ch0100_668_AL_BF_fall_fin_end_obj");

			for (auto& iter : m_GatherObjectList)
			{
				static_cast<CMapKinetic_Object*>(iter)->Set_Trigger(true);

				_float4 vIterPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_float4 vTargetPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

				_float4 vDir = vTargetPos - vIterPos;
				m_vecGatherOverlap.push_back(vDir);
			}
		})
		.Tick([&](double fTimeDelta)
		{
			if (nullptr == m_pASM->GetSocketAnimation("BrainField_AnimSocket")) return;

			_float fRatio = m_pASM->GetSocketAnimation("BrainField_AnimSocket")->GetPlayRatio();


			if (0.45 <= fRatio && BF_Fall_Throw.IsNotDo())
			{
				BrainField_ObjectGather_Throw();
				Event_Kinetic_Throw();
			}
			else if (0.45 <= fRatio)
			{
				BrainField_ObjectGather_Throw();
			}
			else
			{
				BrainField_ObjectAnimation_Socket("BrainField_AnimSocket", 0.f, 0.f);
				BrainField_ObjectGather_Throw();
			}
		})
		.OnExit([&]()
		{
			BF_Fall_Throw.Reset();

			m_pCenterObject = nullptr;

			for (auto& iter : m_GatherObjectList)
			{
				if (false == CGameInstance::GetInstance()->Check_ObjectAlive(iter))
					continue;

				if (auto KineticCheck = dynamic_cast<CMapKinetic_Object*>(iter))
				{
					KineticCheck->SetThrow();
					KineticCheck->Set_Dynamic();
					KineticCheck->Set_Hit();
				}
			}

			m_GatherObjectList.clear();
			m_vecGatherOverlap.clear();
		})
			.AddTransition("BF_FALL_FIN_FINISH to BF_NO_USE_KINETIC_FALL", "BF_NO_USE_KINETIC_FALL")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("BrainField_AnimSocket");
			})
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_Sound()
{
	m_SoundStore.CloneSound("attack_nor_1");
	m_SoundStore.CloneSound("attack_nor_2");
	m_SoundStore.CloneSound("attack_nor_3");
	m_SoundStore.CloneSound("attack_nor_4");
	m_SoundStore.CloneSound("attack_nor_5");
	m_SoundStore.CloneSound("attack_nor_charge_lv1");
	m_SoundStore.CloneSound("attack_nor_charge_lv2");
	m_SoundStore.CloneSound("attack_nor_dashattack");
	m_SoundStore.CloneSound("attack_nor_jumpattack_1");
	m_SoundStore.CloneSound("attack_nor_jumpattack_2");
	m_SoundStore.CloneSound("attack_nor_spin");
	m_SoundStore.CloneSound("attack_nor_spindown");
	m_SoundStore.CloneSound("attack_nor_upper");

	m_SoundStore.CloneSound("attack_fire_1");
	m_SoundStore.CloneSound("attack_fire_2");
	m_SoundStore.CloneSound("attack_fire_3");
	m_SoundStore.CloneSound("attack_fire_4");
	m_SoundStore.CloneSound("attack_fire_5");
	m_SoundStore.CloneSound("attack_fire_charge_lv1");
	m_SoundStore.CloneSound("attack_fire_charge_lv2");
	m_SoundStore.CloneSound("attack_fire_dashattack");
	m_SoundStore.CloneSound("attack_fire_jumpattack_1");
	m_SoundStore.CloneSound("attack_fire_jumpattack_2");
	m_SoundStore.CloneSound("attack_fire_spin");
	m_SoundStore.CloneSound("attack_fire_spindown");
	m_SoundStore.CloneSound("attack_fire_upper");

	m_SoundStore.CloneSound("move_dash");
	m_SoundStore.CloneSound("move_foot_stop");
	m_SoundStore.CloneSound("move_jump");
	m_SoundStore.CloneSound("move_run");
	m_SoundStore.CloneSound("move_walk");

	m_SoundStore.CloneSound("fx_kinetic_lifting");
	m_SoundStore.CloneSound("fx_kinetic_shot");

	m_SoundStore.CloneSound("attack_kinetic_combo_1_1");
	m_SoundStore.CloneSound("attack_kinetic_combo_1_2");

	m_SoundStore.CloneSound("attack_kinetic_combo_spin_1");
	m_SoundStore.CloneSound("attack_kinetic_combo_spin_2");

	m_SoundStore.CloneSound("fx_kinetic_air");
	m_SoundStore.CloneSound("fx_kinetic_backdash");

	m_pModel->Add_EventCaller("attack_nor_1", [this] {Event_EffectSound("attack_nor_1"); });
	m_pModel->Add_EventCaller("attack_nor_2", [this] {Event_EffectSound("attack_nor_2"); });
	m_pModel->Add_EventCaller("attack_nor_3", [this] {Event_EffectSound("attack_nor_3"); });
	m_pModel->Add_EventCaller("attack_nor_4", [this] {Event_EffectSound("attack_nor_4"); });
	m_pModel->Add_EventCaller("attack_nor_5", [this] {Event_EffectSound("attack_nor_5"); });
	m_pModel->Add_EventCaller("attack_nor_charge_lv1", [this] {Event_EffectSound("attack_nor_charge_lv1"); });
	m_pModel->Add_EventCaller("attack_nor_charge_lv2", [this] {Event_EffectSound("attack_nor_charge_lv2"); });
	m_pModel->Add_EventCaller("attack_nor_dashattack", [this] {Event_EffectSound("attack_nor_dashattack"); });

	m_pModel->Add_EventCaller("attack_nor_jumpattack_1", [this] {Event_EffectSound("attack_nor_jumpattack_1"); });
	m_pModel->Add_EventCaller("attack_nor_jumpattack_2", [this] {Event_EffectSound("attack_nor_jumpattack_2"); });

	m_pModel->Add_EventCaller("attack_fire_1", [this] {Event_EffectSound("attack_fire_1"); });
	m_pModel->Add_EventCaller("attack_fire_2", [this] {Event_EffectSound("attack_fire_2"); });
	m_pModel->Add_EventCaller("attack_fire_3", [this] {Event_EffectSound("attack_fire_3"); });
	m_pModel->Add_EventCaller("attack_fire_4", [this] {Event_EffectSound("attack_fire_4"); });
	m_pModel->Add_EventCaller("attack_fire_5", [this] {Event_EffectSound("attack_fire_5"); });
	m_pModel->Add_EventCaller("attack_fire_charge_lv1", [this] {Event_EffectSound("attack_fire_charge_lv1"); });
	m_pModel->Add_EventCaller("attack_fire_charge_lv2", [this] {Event_EffectSound("attack_fire_charge_lv2"); });
	m_pModel->Add_EventCaller("attack_fire_dashattack", [this] {Event_EffectSound("attack_fire_dashattack"); });

	m_pModel->Add_EventCaller("attack_fire_jumpattack_1", [this] {Event_EffectSound("attack_fire_jumpattack_1"); });
	m_pModel->Add_EventCaller("attack_fire_jumpattack_2", [this] {Event_EffectSound("attack_fire_jumpattack_2"); });

	m_pModel->Add_EventCaller("attack_fire_spin", [this] {Event_EffectSound("attack_fire_spin"); });
	m_pModel->Add_EventCaller("attack_fire_spindown", [this] {Event_EffectSound("attack_fire_spindown"); });

	m_pModel->Add_EventCaller("attack_fire_upper", [this] {Event_EffectSound("attack_fire_upper"); });

	m_pModel->Add_EventCaller("attack_nor_spin", [this] {Event_EffectSound("attack_nor_spin"); });
	m_pModel->Add_EventCaller("attack_nor_spindown", [this] {Event_EffectSound("attack_nor_spindown"); });

	m_pModel->Add_EventCaller("attack_nor_upper", [this] {Event_EffectSound("attack_nor_upper"); });

	m_pModel->Add_EventCaller("move_dash", [this] {m_SoundStore.PlaySound("move_dash", m_pTransformCom); });
	m_pModel->Add_EventCaller("move_foot_stop", [this] {m_SoundStore.PlaySound("move_foot_stop", m_pTransformCom); });
	m_pModel->Add_EventCaller("move_jump", [this] {m_SoundStore.PlaySound("move_jump", m_pTransformCom); });
	m_pModel->Add_EventCaller("move_run", [this] {m_SoundStore.PlaySound("move_run", m_pTransformCom); });
	m_pModel->Add_EventCaller("move_walk", [this] {m_SoundStore.PlaySound("move_walk", m_pTransformCom); });

	m_pModel->Add_EventCaller("fx_kinetic_lifting", [this] {m_SoundStore.PlaySound("fx_kinetic_lifting", m_pTransformCom); });
	m_pModel->Add_EventCaller("fx_kinetic_shot", [this] {m_SoundStore.PlaySound("fx_kinetic_shot", m_pTransformCom); });

	m_pModel->Add_EventCaller("attack_kinetic_combo_1_1", [this] {m_SoundStore.PlaySound("attack_kinetic_combo_1_1", m_pTransformCom); });
	m_pModel->Add_EventCaller("attack_kinetic_combo_1_2", [this] {m_SoundStore.PlaySound("attack_kinetic_combo_1_2", m_pTransformCom); });
	m_pModel->Add_EventCaller("attack_kinetic_combo_spin_1", [this] {m_SoundStore.PlaySound("attack_kinetic_combo_1_2", m_pTransformCom); });
	m_pModel->Add_EventCaller("attack_kinetic_combo_spin_2", [this] {m_SoundStore.PlaySound("attack_kinetic_combo_1_2", m_pTransformCom); });

	m_pModel->Add_EventCaller("fx_kinetic_air", [this] {m_SoundStore.PlaySound("fx_kinetic_air", m_pTransformCom); });
	m_pModel->Add_EventCaller("fx_kinetic_backdash", [this] {m_SoundStore.PlaySound("fx_kinetic_backdash", m_pTransformCom); });

	return S_OK;
}

void CPlayer::Update_NoticeNeon()
{
	_float4x4	NoticeNeonPivot = XMMatrixTranslation(0.f, 2.5f, 0.f);

	if (m_pNoticeNeon.first != nullptr && m_pNoticeNeon.second != nullptr)
	{
		m_pNoticeNeon.first->SetDelete();
		m_pNoticeNeon.second->Delete_Particles();

		Safe_Release(m_pNoticeNeon.first);
		Safe_Release(m_pNoticeNeon.second);

		m_pNoticeNeon.first = nullptr;
		m_pNoticeNeon.second = nullptr;
	}

	switch (this->GetDeBuffType())
	{
	case Client::EDeBuffType::DEBUFF_FIRE:
		m_pNoticeNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_fire");
		m_pNoticeNeon.second = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_HIT, L"NoticeNeon_Fire_Particle");
		break;

	case Client::EDeBuffType::DEBUFF_OIL:
		m_pNoticeNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_oil");
		m_pNoticeNeon.second = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_HIT, L"Notice_Neon_Oil_Particle");
		break;

	case Client::EDeBuffType::DEBUFF_THUNDER:
		m_pNoticeNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_thunder");
		m_pNoticeNeon.second = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_HIT, L"Notice_Neon_Thunder_Particle");
		break;

	case Client::EDeBuffType::DEBUFF_WATER:
		m_pNoticeNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_water");
		m_pNoticeNeon.second = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_HIT, L"Notice_Neon_Water_Particle");
		break;

	case Client::EDeBuffType::DEBUFF_END:
		break;

	default:
		break;
	}

	if (m_pNoticeNeon.first != nullptr && m_pNoticeNeon.second != nullptr)
	{
		m_pNoticeNeon.first->Start_AttachPivot(this, NoticeNeonPivot, "Reference", true, true);
		m_pNoticeNeon.second->Start_AttachPivot(this, NoticeNeonPivot, "Reference", true, true);
		Safe_AddRef(m_pNoticeNeon.first);
		Safe_AddRef(m_pNoticeNeon.second);
	}
}

void CPlayer::Update_CautionNeon()
{
	_float4x4	NoticeNeonPivot = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.1f, 0.f, 0.f);

	_float fCurHP = (_float)CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP;
	_float fMaxHP = (_float)CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP;

	if (0.2f >= fCurHP / fMaxHP)
	{
		if (!CGameInstance::GetInstance()->Check_ObjectAlive(m_pCautionNeon.first))
		{
			m_pCautionNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_HP");
			m_pCautionNeon.first->Start_AttachPivot(this, NoticeNeonPivot, "String2", true, true);
		}
	}
	else
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCautionNeon.first))
		{
			m_pCautionNeon.first->SetDelete();
			Safe_Release(m_pNoticeNeon.first);
			m_pCautionNeon.first = nullptr;
		}
	}
}

HRESULT CPlayer::SetUp_DeBuffStateMachine()
{
	// ªÛ≈¬¿ÃªÛø° ∞…∑»¿ª ∂ß µø¿€«“ FSM, æ÷¥œ∏ﬁ¿Ãº«ø° øµ¡l¿ª ¡÷¡ˆ æ ¿Ω

	m_ElecShock_Start.push_back(m_pModel->Find_Animation("AS_ch0100_421_AL_down_start"));
	m_ElecShock_Start.push_back(m_pModel->Find_Animation("AS_ch0100_481_AL_down_B_shock"));
	m_ElecShock_WakeUp.push_back(m_pModel->Find_Animation("AS_ch0100_445_AL_wakeup_B"));

		m_pDeBuffStateMachine =
		CFSMComponentBuilder().InitState("DEBUFF_END")

		.AddState("DEBUFF_END")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) 
		{
			
		})
		.OnExit([&]() {})

			.AddTransition("DEBUFF_END to DEBUFF_FIRE", "DEBUFF_FIRE")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_FIRE; })
			.Priority(0)

			.AddTransition("DEBUFF_END to DEBUFF_WATER", "DEBUFF_WATER")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_WATER; })
			.Priority(1)

			.AddTransition("DEBUFF_END to DEBUFF_OIL", "DEBUFF_OIL")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_OIL; })
			.Priority(2)

			.AddTransition("DEBUFF_END to DEBUFF_ELEC", "DEBUFF_ELEC")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_THUNDER; })
			.Priority(3)


		.AddState("DEBUFF_FIRE")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) 
		{
			m_NoticeTick.Tick(fTimeDelta);

			if (m_NoticeTick.Use())
			{
				DAMAGE_PARAM tParam;
				ZeroMemory(&tParam, sizeof(DAMAGE_DESC));
				tParam.eAttackType = EAttackType::ATK_END;
				tParam.eDeBuff = EDeBuffType::DEBUFF_FIRE;
				tParam.iDamage = 1;
				tParam.pCauser = this;
				TakeDamage(tParam);
			}
		})
		.OnExit([&]() {})
			
			/*.AddTransition("DEBUFF_FIRE to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(0)*/

			.AddTransition("DEBUFF_FIRE to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(100)

		.AddState("DEBUFF_WATER")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) { })
		.OnExit([&]() {})

			.AddTransition("DEBUFF_WATER to DEBUFF_ELEC", "DEBUFF_ELEC")
			.Predicator([&]()->_bool {return m_eDeBuff == EDeBuffType::DEBUFF_THUNDER; })
			.Priority(0)

			.AddTransition("DEBUFF_WATER to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(100)

		.AddState("DEBUFF_OIL")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) { })
		.OnExit([&]() {})

			.AddTransition("DEBUFF_OIL to DEBUFF_FIRE", "DEBUFF_FIRE")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_FIRE; })
			.Priority(0)

			.AddTransition("DEBUFF_OIL to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(100)

				.AddState("DEBUFF_ELEC")
		.OnStart([&]() 
		{
			m_pASM->SetCurState("IDLE");
			m_pASM->SetCurState_BrainField("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_pASM->AttachAnimSocket("Hit_AnimSocket", m_ElecShock_Start);
		})
		.Tick([&](double fTimeDelta) { })
		.OnExit([&]() {})

			/*.AddTransition("DEBUFF_OIL to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(0)*/

			.AddTransition("DEBUFF_ELEC to DEBUFF_ELEC_END", "DEBUFF_ELEC_END")
			.Predicator([&]()->_bool {return GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(100)

		.AddState("DEBUFF_ELEC_END")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Hit_AnimSocket", m_ElecShock_WakeUp);
		})
		.Tick([&](double fTimeDelta) { })
		.OnExit([&]() { m_pASM->ClearAnimSocket("Hit_AnimSocket"); })

			.AddTransition("DEBUFF_ELEC_END to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool 
			{
				return m_pModel->Find_Animation("AS_ch0100_445_AL_wakeup_B")->GetPlayRatio() > 0.8f &&
					(m_bLeftClick || m_bWalk || m_bDash || m_bJump);
			})
			.Priority(0)

			.AddTransition("DEBUFF_ELEC_END to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool 
			{
				return m_pASM->isSocketEmpty("Hit_AnimSocket");
			})
			.Priority(100)

		.Build();

	return S_OK;
}

void CPlayer::EnemyReportCheck()
{
	list<ENEMY_DAMAGE_REPORT> DamageReportList = CGameManager::GetInstance()->GetEnemyReport();

	EAttackType eAtkType = EAttackType::ATK_END;

	for (auto& iter : DamageReportList)
	{
		eAtkType = iter.eAttackType;

		// ∞‘¿Ã¡ˆ »∏∫π ∞¸∑√
		if (iter.eAttackType == EAttackType::ATK_LIGHT || iter.eAttackType == EAttackType::ATK_MIDDLE)
		{
			KineticGuage_Recover();
		}
		else if (iter.eAttackType == EAttackType::ATK_HEAVY)
		{
			CPlayerInfoManager::GetInstance()->Change_DriveEnergy(CHANGE_INCREASE, 2.f);
			IM_LOG("DRIVE : %f", CPlayerInfoManager::GetInstance()->Get_PlayerStat().fDriveEnergy);
		}
		else if (iter.eAttackType == EAttackType::ATK_SPECIAL_LOOP)
		{
			CPlayerInfoManager::GetInstance()->Change_DriveEnergy(CHANGE_INCREASE, 1.f);
			IM_LOG("DRIVE : %f", CPlayerInfoManager::GetInstance()->Get_PlayerStat().fDriveEnergy);
		}
		else if (iter.eAttackType == EAttackType::ATK_SPECIAL_END)
		{
			CPlayerInfoManager::GetInstance()->Change_DriveEnergy(CHANGE_INCREASE, 3.f);
			IM_LOG("DRIVE : %f", CPlayerInfoManager::GetInstance()->Get_PlayerStat().fDriveEnergy);
		}

		// »˜∆Æ∑¢ ∞¸∑√
		/*if (iter.eAttackType == EAttackType::ATK_MIDDLE)
		{
			CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack");
		}
		else if (iter.eAttackType == EAttackType::ATK_HEAVY)
		{
			CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack");
		}
		else if (iter.eAttackType == EAttackType::ATK_SPECIAL_END)
		{
			CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack");
		}*/
	}

	if (m_bActionAble && !DamageReportList.empty())
	{
		if (eAtkType == EAttackType::ATK_LIGHT)
		{
			CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.1f);
		}
		else if (eAtkType == EAttackType::ATK_MIDDLE)
		{
			CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.3f);
			CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack_Middle");
		}

		m_bActionAble = false;
	}

	CGameManager::GetInstance()->ResetEnemyReport();
}

void CPlayer::KineticGuage_Recover()
{
	_uint iRecovery = 0;

	// ASM∞¯∞›
	if (m_pASM->GetCurStateName() == "ATTACK_NONCHARGE" ||
		m_pASM->GetCurStateName() == "CHARGE_ATTACK_01" ||
		m_pASM->GetCurStateName() == "CHARGE_ATTACK_02" || 
		m_pASM->GetCurStateName() == "CHARGE_ATTACK_03")
	{
		iRecovery = 20;
	}
	else
	{
		iRecovery = 10;
	}

	// FSM∞¯∞›? ±∏∫–«ÿæﬂ«œ≥™?
	//if (m_pASM->GetCurSocketAnimName() == "AS_ch0100_207_AL_atk_justdodge")
	//{
	//	
	//}

	CPlayerInfoManager::GetInstance()->Change_PlayerKineticEnergy(CHANGE_INCREASE, iRecovery);
}

_bool CPlayer::CanKinetic(_uint iUseGuage)
{
	_uint	iCurKineticEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergy;
	_uint	iMinKineticEnergy = 20;
	_uint	iUseKineticEnergy = iUseGuage;

	if (iCurKineticEnergy < iMinKineticEnergy)
		return false;

	if (iCurKineticEnergy < iUseKineticEnergy)
		return false;

	return true;
}

HRESULT CPlayer::SetUp_ProductionEvent()
{
	m_pModel->Add_EventCaller("Hood_Active_On", [this]
	{
		m_pModel->FindMaterial(L"MI_ch0100_HOOD_0")->SetActive(true);
		m_pModel->FindMaterial(L"MI_ch0100_BODY_0")->GetParam().Ints[1] = 1;
	});

	m_pModel->Add_EventCaller("Hood_Active_Off", [this]
	{
		m_pModel->FindMaterial(L"MI_ch0100_HOOD_0")->SetActive(false);
		m_pModel->FindMaterial(L"MI_ch0100_BODY_0")->GetParam().Ints[1] = 0;
	});

	m_pModel->Add_EventCaller("Mask_Dissolve_On", [this]
	{
		m_bDriveMask = true;
	});

	m_pModel->Add_EventCaller("Mask_Dissolve_Off", [this]
	{
		m_bDriveMask = false;
	});

	m_pModel->Add_EventCaller("Hair_Active_On", [this]
	{
		m_pModel->FindMaterial(L"MI_ch0100_HAIR_0")->SetActive(true);
		m_pModel->FindMaterial(L"MI_ch0100_HAIR_1")->SetActive(true);
	});

	m_pModel->Add_EventCaller("Hair_Active_Off", [this]
	{
		m_pModel->FindMaterial(L"MI_ch0100_HAIR_0")->SetActive(false);
		m_pModel->FindMaterial(L"MI_ch0100_HAIR_1")->SetActive(false);
	});

	m_pModel->Add_EventCaller("DriveMode_Effect", [this]
	{
		_matrix matEffect = XMMatrixScaling(0.4f, 0.4f, 0.4f) * XMMatrixTranslation(0.f, 0.01f, -0.04f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Sas_DriveMode_Effect")->Start_AttachPivot(this, matEffect, "Mask", true);
	});
	m_pModel->Add_EventCaller("DriveMode_Particle", [this]
	{
		_matrix matParticle = XMMatrixScaling(1.5f, 1.f, 2.f) * XMMatrixRotationX(XMConvertToRadians(-5.f)) * XMMatrixTranslation(0.f, -0.275f, 1.f);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Sas_DriveMode_Particle")->Start_AttachPivot(this, matParticle, "Mask", true, false, true);
	});

	return S_OK;
}

void CPlayer::Production_Tick(_double TimeDelta)
{
	DriveMaskDissolve(TimeDelta);

	//CamZoom(TimeDelta);
}

void CPlayer::DriveMaskDissolve(_double TimeDelta)
{
	if (m_bDriveMask)
	{
		if (1.f >= m_fMaskDissolve)
		{
			m_fMaskDissolve = min(m_fMaskDissolve + (TimeDelta / 0.8f), 1.f);
			m_pModel->FindMaterial(L"MI_ch0100_MASK_0")->GetParam().Floats[3] = m_fMaskDissolve;
		}
	}
	else
	{
		if (0.f < m_fMaskDissolve)
		{
			m_fMaskDissolve = max(m_fMaskDissolve - (TimeDelta / 0.8f), 0.f);
			m_pModel->FindMaterial(L"MI_ch0100_MASK_0")->GetParam().Floats[3] = m_fMaskDissolve;
		}
	}
}

HRESULT CPlayer::SetUp_AttackDesc()
{
	m_mapCollisionEvent.emplace("ATK_A1", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A2", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A3", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A4", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A5", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATTACK_DASH_END", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATTACK_NONCHARGE", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_01", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_02", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_03", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR1", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR2", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR_NONCHARGE", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR_CHARGE_START", [this]()
	{
		if (m_pModel->GetPlayAnimation()->GetPlayRatio() <= 0.75f)
		{
			m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
			m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
			m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
			m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
			m_AttackDesc.pCauser = this;
			m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		}
		else
		{
			m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
			m_AttackDesc.eAttackType = EAttackType::ATK_DOWN;
			m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
			m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
			m_AttackDesc.pCauser = this;
			m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		}
	});
	m_mapCollisionEvent.emplace("ATK_AIR_CHARGE_FALL", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_DOWN;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATTACK_UPPER_START", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_TO_AIR;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_207_AL_atk_justdodge", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_216_AL_atk_dash1_start", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_217_AL_atk_dash2_end", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_218_AL_atk_dash3_end", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_219_AL_atk_dash4_end", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_214_AL_sas_dash_end_Telepo", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_222_AL_atk_air2", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_228_AL_atk_air_dash1_end", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_TO_AIR;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 1.f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	return S_OK;
}

HRESULT CPlayer::SetUp_TeleportStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_214_AL_sas_dash_start_Telepo"));
	m_Teleport_FloorAttack_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_214_AL_sas_dash_end_Telepo"));
	m_Teleport_FloorAttack_End.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_230_AL_atk_air_dash_start"));
	m_Teleport_AirAttack_Start.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_222_AL_atk_air2"));
	m_Teleport_AirAttack_End.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_141_AL_jump_fall"));
	m_Teleport_AirAttack_Fall.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_141_AL_jump_landing"));
	m_Teleport_AirAttack_Landing.push_back(pAnimation);

	m_pTeleportStateMachine = 
		CFSMComponentBuilder()
		.InitState("TELEPORTATTACK_NOUSE")

		.AddState("TELEPORTATTACK_NOUSE")
		.OnStart([&]() 
		{
			m_pASM->ClearAnimSocket("SAS_Special_AnimSocket");

			m_bVisible = true;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(true);
			for (auto& iter : m_vecSheath)
				iter->SetVisible(true);

			m_pSAS_Cable->SetVisible(true);

			m_fTeleportAttack_GC = 0.f;
		})
		.Tick([&](double fTimeDelta) 
		{
			m_fTeleportAttack_GC += (_float)fTimeDelta;
		})
		.OnExit([&]() 
		{
			TeleportEffectMaker();
		})
		.AddTransition("TELEPORTATTACK_NOUSE to TELEPORTATTACK_FLOOR_START", "TELEPORTATTACK_FLOOR_START")
		.Predicator([&]()->_bool 
		{
			_bool bResult = m_bLeftClick && isPlayerNonAttack() && (m_fTeleportAttack_GC > 0.1f) && !m_bAir;
			return bResult;
		})
		.Priority(0)

		.AddTransition("TELEPORTATTACK_NOUSE to TELEPORTATTACK_AIR_START", "TELEPORTATTACK_AIR_START")
		.Predicator([&]()->_bool 
		{
			_bool bResult = m_bLeftClick && isPlayerNonAttack() && (m_fTeleportAttack_GC > 0.1f) && m_bAir;
			return bResult;
		})
		.Priority(0)

		// ÏßÄ??/////////////////////////////
		.AddState("TELEPORTATTACK_FLOOR_START")
		.OnStart([&]() 
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_pASM->AttachAnimSocket("SAS_Special_AnimSocket", m_Teleport_FloorAttack_Start);

			m_bVisible = false;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(false);
			for (auto& iter : m_vecSheath)
				iter->SetVisible(false);

			m_pSAS_Cable->SetVisible(false);

			// ?úÍ∞Ñ?¥Îèô ?ÑÏπò ?°Í∏∞
			m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				physx::PxRaycastHit hitBuffer[1];
				physx::PxRaycastBuffer rayOut(hitBuffer, 1);

				RayCastParams param;
				param.rayOut = &rayOut;
				param.vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				param.vDir = XMLoadFloat4(&static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetColliderPosition())
							- XMLoadFloat4(&param.vOrigin);
				param.fDistance = 20.f;
				param.iTargetType = CTB_STATIC | CTB_MONSTER | CTB_MONSTER_PART;
				param.bSingle = true;
				param.fVisibleTime = 1.f;

				if (CGameInstance::GetInstance()->RayCast(param))
				{
					for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
					{
						auto pHit = rayOut.getAnyHit(i);

						_float4 vTargetOriginPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

						m_vTeleportPos = {pHit.position.x, vTargetOriginPos.y, pHit.position.z, 1.f};
						m_pTransformCom->LookAt_NonY(m_vTeleportPos);
					}
				}
				else
				{
					m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f);
				}
			}
			else
			{
				m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f);
			}

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_vTeleportPos);

		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]() 
		{

		})
		.AddTransition("TELEPORTATTACK_FLOOR_START to TELEPORTATTACK_FLOOR_START_END", "TELEPORTATTACK_FLOOR_START_END")
		.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("SAS_Special_AnimSocket"); })
		.Priority(0)

		.AddState("TELEPORTATTACK_FLOOR_START_END")
		.OnStart([&]() 
		{
			m_pASM->InputAnimSocket("SAS_Special_AnimSocket", m_Teleport_FloorAttack_End);

			m_bVisible = true;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(true);
			for (auto& iter : m_vecSheath)
				iter->SetVisible(true);

			m_pSAS_Cable->SetVisible(true);

			TeleportEffectMaker();
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]() 
		{
			m_bVisible = true;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(true);
			for (auto& iter : m_vecSheath)
				iter->SetVisible(false);

			m_pSAS_Cable->SetVisible(true);
		})
		.AddTransition("TELEPORTATTACK_FLOOR_START_END to TELEPORTATTACK_NOUSE", "TELEPORTATTACK_NOUSE")
		.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("SAS_Special_AnimSocket"); })
		.Priority(0)

		.AddTransition("TELEPORTATTACK_FLOOR_START_END to TELEPORTATTACK_NOUSE", "TELEPORTATTACK_NOUSE")
		.Predicator([&]()->_bool { return m_pASM->isSocketPassby("SAS_Special_AnimSocket", 0.1f) && (m_bLeftClick); })
		.Priority(0)

		.AddTransition("TELEPORTATTACK_FLOOR_START_END to TELEPORTATTACK_NOUSE", "TELEPORTATTACK_NOUSE")
		.Predicator([&]()->_bool { return m_pASM->isSocketPassby("SAS_Special_AnimSocket", 0.2f) && (m_bWalk || m_bDash || m_bJump || m_bKineticRB || m_bKineticG); })
		.Priority(0)

		// Í≥µÏ§ë /////////////////////////////
		.AddState("TELEPORTATTACK_AIR_START")
		.OnStart([&]() 
		{
			m_pASM->SetCurState("JUMP_FALL");
			SetAbleState({ false, false, false, false, true, false, true, true, true, false });

			m_pASM->AttachAnimSocket("SAS_Special_AnimSocket", m_Teleport_AirAttack_Start);

			m_bVisible = false;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(false);
			for (auto& iter : m_vecSheath)
				iter->SetVisible(false);

			m_pSAS_Cable->SetVisible(false);

			// ?úÍ∞Ñ?¥Îèô ?ÑÏπò ?°Í∏∞
			m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				physx::PxRaycastHit hitBuffer[1];
				physx::PxRaycastBuffer rayOut(hitBuffer, 1);

				RayCastParams param;
				param.rayOut = &rayOut;
				param.vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				param.vDir = XMLoadFloat4(&static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetColliderPosition())
					- XMLoadFloat4(&param.vOrigin);
				param.fDistance = 20.f;
				param.iTargetType = CTB_STATIC | CTB_MONSTER | CTB_MONSTER_PART;
				param.bSingle = true;
				param.fVisibleTime = 1.f;

				if (CGameInstance::GetInstance()->RayCast(param))
				{
					for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
					{
						auto pHit = rayOut.getAnyHit(i);

						_float4 vTargetOriginPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

						m_vTeleportPos = { pHit.position.x, vTargetOriginPos.y + 0.2f, pHit.position.z, 1.f };
						m_pTransformCom->LookAt_NonY(m_vTeleportPos);
					}
				}
				else
				{
					m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f);
				}
			}
			else
			{
				m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f);
			}

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_vTeleportPos);

		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]() 
		{
			m_Teleport_AirAttack_End.front()->Reset();
		})
		.AddTransition("TELEPORTATTACK_AIR_START to TELEPORTATTACK_AIR_END", "TELEPORTATTACK_AIR_END")
		.Predicator([&]()->_bool 
		{
			return m_pASM->isSocketAlmostFinish("SAS_Special_AnimSocket");
		})
		.Priority(0)

		.AddState("TELEPORTATTACK_AIR_END")
		.OnStart([&]() 
		{
			m_pASM->InputAnimSocket("SAS_Special_AnimSocket", m_Teleport_AirAttack_End);

			m_bVisible = true;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(true);
			for (auto& iter : m_vecSheath)
				iter->SetVisible(true);

			m_pSAS_Cable->SetVisible(true);

			TeleportEffectMaker();
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]() 
		{
			m_bActiveGravity = true;
		})
		.AddTransition("TELEPORTATTACK_AIR_END to TELEPORTATTACK_AIR_LANDING_START", "TELEPORTATTACK_AIR_LANDING_START")
		.Predicator([&]()->_bool 
		{
			return m_pASM->isSocketAlmostFinish("SAS_Special_AnimSocket");
		})
		.Priority(0)
		.AddTransition("TELEPORTATTACK_AIR_END to TELEPORTATTACK_NOUSE", "TELEPORTATTACK_NOUSE")
		.Predicator([&]()->_bool 
		{
			return m_pASM->isSocketPassby("SAS_Special_AnimSocket", 0.35f) && m_bLeftClick;
		})
		.Priority(0)

		.AddState("TELEPORTATTACK_AIR_LANDING_START")
		.OnStart([&]()
		{
			m_bVisible = false;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(false);
			for (auto& iter : m_vecSheath)
				iter->SetVisible(false);

			m_pSAS_Cable->SetVisible(false);

			TeleportEffectMaker();

		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]() 
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_fYSpeed = 0.f;
			m_bVisible = true;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(true);
			for (auto& iter : m_vecSheath)
				iter->SetVisible(true);

			m_pSAS_Cable->SetVisible(true);

			TeleportEffectMaker();
		})
		.AddTransition("TELEPORTATTACK_AIR_LANDING_START to TELEPORTATTACK_NOUSE", "TELEPORTATTACK_NOUSE")
		.Predicator([&]()->_bool 
		{
			return m_bOnFloor;
		})
		.Priority(0)

	.Build();

	return S_OK;
}

void CPlayer::TeleportEffectMaker()
{
	_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName, LAYER_PLAYEREFFECT)->Start_Attach(this, "Sheath", true);

	_matrix matTex = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixTranslation(0.f, 0.f, 0.f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Sas_Teleport_Attach_Player_Tex", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matTex, "Sheath", true, true);
}

HRESULT CPlayer::SetUp_TrainStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	// Ï¢åÏ∏° Ï∞®Ï?
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_372_AL_Cap_sLcLeL_start"));
	m_Train_Charge_L.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_372_AL_Cap_sLcLeL_loop"));
	m_Train_Charge_L.push_back(pAnimation);
	// Ï¢åÏ∏° Ï∫îÏä¨
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_372_AL_Cap_sLcLeL_end"));
	m_Train_Cancel_L.push_back(pAnimation);
	// Ï¢åÏ∏° ?òÏ?Í∏?
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_372_AL_mg_train_L"));
	m_Train_Throw_L.push_back(pAnimation);

	m_pTrainStateMachine_Left = 
		CFSMComponentBuilder()
		.InitState("TRAIN_LEFT_NOUSE")

		.AddState("TRAIN_LEFT_NOUSE")
		.OnStart([&]() 
		{
			m_bKineticSpecial_Activate = false;

			m_bKineticSpecial = false;
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

		})
		.Tick([&](double fTimeDelta)
		{
			m_bKineticSpecial = false;
		})
		.OnExit([&]()
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})

			.AddTransition("TRAIN_LEFT_NOUSE to TRAIN_LEFT_CHARGE", "TRAIN_LEFT_CHARGE")
			.Predicator([&]()->_bool 
			{
				_bool bResult = (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject());
				return m_bKineticG && bResult;
			})
			.Priority(0)


		.AddState("TRAIN_LEFT_CHARGE")
		.OnStart([&]() 
		{
			m_bKineticSpecial = true;
			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_Train_Charge_L);
			//static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();

			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Train_Set_Animation("AS_mg02_372_train_cap_L");
			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Activate_Animation(true);
		})
		.Tick([&](double fTimeDelta)
		{
			m_fSpecialCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge * 0.5f);

		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge);
		})

			.AddTransition("TRAIN_LEFT_CHARGE to TRAIN_LEFT_THROW", "TRAIN_LEFT_THROW")
			.Predicator([&]()->_bool { return m_fSpecialCharge >= 2.f; })
			.Priority(0)

			.AddTransition("TRAIN_LEFT_CHARGE to TRAIN_LEFT_CANCEL", "TRAIN_LEFT_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)

		.AddState("TRAIN_LEFT_CANCEL")
		.OnStart([&]()
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_Train_Cancel_L);

			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Train_Set_Animation("AS_mg02_372_train_cancel_L");
			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Activate_Animation(true);

			//static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})

			.AddTransition("TRAIN_LEFT_CANCEL to TRAIN_LEFT_NOUSE", "TRAIN_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG); 
			})
			.Priority(0)

		.AddState("TRAIN_LEFT_THROW")
		.OnStart([&]() 
		{
			m_bKineticSpecial_Activate = true;

			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Set_Used();

			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Train_Set_Animation("AS_mg02_372_train_start_L");
			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Activate_Animation(true);
			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->SpecialRimLightFix(true);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();


			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_Train_Throw_L);
		})
		.Tick([&](double fTimeDelta)
		{
		})
		.OnExit([&]()
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_fSpecialCharge = 0.f;
			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Train_SetDeadTimer();
		})

			.AddTransition("TRAIN_LEFT_THROW to TRAIN_LEFT_NOUSE", "TRAIN_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || 
					( ((m_pASM->isSocketPassby("Kinetic_Special_AnimSocket"), 0.8f) && (m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB))));
			})
			.Priority(0)


		.Build();

	// Ï∞®Ï? / Ï∫îÏä¨ / ?ôÏûë 3Í∞úÎ°ú ?§ÌÖå?¥Ìä∏ Î∂ÑÌï†

	return S_OK;
}

HRESULT CPlayer::SetUp_TelephonePoleStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_start0"));
	m_TelephonePole_Charge_L.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_loop0"));
	m_TelephonePole_Charge_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_end0"));
	m_TelephonePole_Cancel_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LL_start"));
	m_TelephonePole_Start_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LR_start"));
	m_TelephonePole_Throw_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LR_loop"));
	m_TelephonePole_Wait_L.push_back(pAnimation);
	 
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LR_end"));
	m_TelephonePole_End_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_331_AL_swing_L"));
	m_TelephonePole_Swing_L.push_back(pAnimation);


	m_pTelephonePoleStateMachine_Left =
		CFSMComponentBuilder()
		.InitState("TELEPHONEPOLE_LEFT_NOUSE")

		.AddState("TELEPHONEPOLE_LEFT_NOUSE")
		.OnStart([&]()
		{
			m_bKineticSpecial_Activate = false;

			m_bKineticSpecial = false;
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			m_bKineticSpecial = false;
		})
		.OnExit([&]()
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
			.AddTransition("TELEPHONEPOLE_LEFT_NOUSE to TELEPHONEPOLE_LEFT_CHARGE", "TELEPHONEPOLE_LEFT_CHARGE")
			.Predicator([&]()->_bool 
			{
				_bool bResult = (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject());
				return m_bKineticG && bResult;
			})
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_CHARGE")
		.OnStart([&]() 
		{
			m_bKineticSpecial = true;
			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_Charge_L);
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_fSpecialCharge > 1.f)
			{
				static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
					->TelephonePole_Bend(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), CMathUtils::RandomFloat(0.f, 0.5f));
			}

			m_fSpecialCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge * 0.5f);
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge);
		})
			.AddTransition("TELEPHONEPOLE_LEFT_CHARGE to TELEPHONEPOLE_LEFT_START", "TELEPHONEPOLE_LEFT_START")
			.Predicator([&]()->_bool { return m_fSpecialCharge >= 2.f; })
			.Priority(0)

			.AddTransition("TELEPHONEPOLE_LEFT_CHARGE to TELEPHONEPOLE_LEFT_CANCEL", "TELEPHONEPOLE_LEFT_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_CANCEL")
		.OnStart([&]()
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_Cancel_L);
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})

			.AddTransition("TELEPHONEPOLE_LEFT_CANCEL to TELEPHONEPOLE_LEFT_NOUSE", "TELEPHONEPOLE_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG); 
			})
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_START")
		.OnStart([&]() 
		{
			m_bKineticSpecial_Activate = true;
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_Start_L);
			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->SpecialRimLightFix(true);

			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Set_Used();
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
				->TelephonePole_PullOut(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f - m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio());
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("TELEPHONEPOLE_LEFT_START to TELEPHONEPOLE_LEFT_START", "TELEPHONEPOLE_LEFT_THROW")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket")); 
			})
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_THROW")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_Throw_L);
			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_pole_swing_R");

			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
				->SetCatchPoint();
		})
		.Tick([&](double fTimeDelta)
		{
			//_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio();

			_float fPlayTime = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayTime();
			_float fDuration = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetDuration();

			if (0.5f >= (fPlayTime / fDuration))
			{
				static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
					->TelephonePole_AttachLerp(m_pKineticAnimModel, m_pTransformCom, (fPlayTime / fDuration) * 2.f);
			}
			else
			{
				static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
					->TelephonePole_Swing(m_pKineticAnimModel, m_pTransformCom, fPlayTime - (fDuration * 0.5f));
			}

			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->TelephonePole_Collision_On();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->TelephonePole_Collision_Off();
		})
			.AddTransition("TELEPHONEPOLE_LEFT_THROW to TELEPHONEPOLE_LEFT_WAIT", "TELEPHONEPOLE_LEFT_WAIT")
			.Predicator([&]()->_bool 
			{ 
				_bool bAddAble = static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_AddAble();
				return bAddAble && (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
			})
			.Priority(0)

			.AddTransition("TELEPHONEPOLE_LEFT_THROW to TELEPHONEPOLE_LEFT_END", "TELEPHONEPOLE_LEFT_END")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
			})
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_WAIT")
		.OnStart([&]() 
		{
			CGameInstance::GetInstance()->SetTimeRatioCurve("KineticSpecialWaiting");

			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_Swing_L);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();

			CreateSpecialUI(SPECIAL_TELEPHONEPOLE);
		})
		.Tick([&](double fTimeDelta)
		{
			m_fSpecialWaiting += g_fTimeDelta;
		})
		.OnExit([&]()
		{
			CGameInstance::GetInstance()->ResetTimeRatio();
			m_fSpecialCharge = 0.f;
			m_fSpecialWaiting = 0.f;
		})
			.AddTransition("TELEPHONEPOLE_LEFT_WAIT to TELEPHONEPOLE_LEFT_SWING", "TELEPHONEPOLE_LEFT_SWING")
			.Predicator([&]()->_bool 
			{ 
				return m_bLeftClick; 
			})
			.Priority(0)

			.AddTransition("TELEPHONEPOLE_LEFT_WAIT to TELEPHONEPOLE_LEFT_END", "TELEPHONEPOLE_LEFT_END")
			.Predicator([&]()->_bool 
			{ 
				return 5.f <= m_fSpecialWaiting;
			})
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_END")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_End_L);

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->TelephonePole_SetDeadTimer();
		})
			.AddTransition("TELEPHONEPOLE_LEFT_END to TELEPHONEPOLE_LEFT_NOUSE", "TELEPHONEPOLE_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
			})
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_SWING")
		.OnStart([&]() 
		{
			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_pole_swing_L");

			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
				->SetCatchPoint();
		})
		.Tick([&](double fTimeDelta)
		{
			_float fPlayTime = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayTime();
			_float fDuration = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetDuration();

			if (0.1f >= (fPlayTime / fDuration))
			{
				static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
					->TelephonePole_AttachLerp(m_pKineticAnimModel, m_pTransformCom, (fPlayTime / fDuration) * 10.f);
			}
			else
			{
				static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
					->TelephonePole_Swing(m_pKineticAnimModel, m_pTransformCom, fPlayTime - (fDuration * 0.1f));
			}

			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->TelephonePole_Collision_On();
		})
		.OnExit([&]()
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_fSpecialCharge = 0.f;
			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->TelephonePole_Collision_Off();
			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->TelephonePole_SetDeadTimer();
		})
			.AddTransition("TELEPHONEPOLE_LEFT_SWING to TELEPHONEPOLE_LEFT_NOUSE", "TELEPHONEPOLE_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket")); 
			})
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_BrainCrashStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_BC_em_common_ch0100_end"));
	m_BrainCrash_Activate.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_BrainCrash_Cutin"));
	m_BrainCrash_CutScene.push_back(pAnimation);


	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_BC_em0100m_ch0100"));
	m_BrainCrash_em0100.push_back(pAnimation);


	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_BC_em0200m_ch0100"));
	m_BrandCrash_em0200.push_back(pAnimation);
	//NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_BC_em0200m_ch0100_end"));
	//m_BrandCrash_em0200.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_BC_em0700m_ch0100"));
	m_BrainCrash_em0700.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_BC_em0800m_ch0100"));
	m_BrainCrash_em0800.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_BC_em1100m_ch0100"));
	m_BrainCrash_em1100.push_back(pAnimation);

	m_pBrainCrashStateMachine = CFSMComponentBuilder()
		.InitState("BRAINCRASH_NOUSE")

		.AddState("BRAINCRASH_NOUSE")
		.OnStart([&]()
		{
			m_pASM->ClearAnimSocket("BrainCrash_AnimSocket");
			m_bBrainCrash = false;
		})
		.Tick([&](double fTimeDelta) 
		{
			m_bBrainCrash = false;
		})
		.OnExit([&]()
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
			.AddTransition("BRAINCRASH_NOUSE to BRAINCRASH_CUTSCENE", "BRAINCRASH_CUTSCENE")
			.Predicator([&]()->_bool 
			{ 
				if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) return false;
				return m_bBrainCrashInput && static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->CanBC();
			})
			.Priority(0)

		.AddState("BRAINCRASH_CUTSCENE")
		.OnStart([&]()
		{
			m_bBrainCrash = true;
			m_pSasPortrait->Start_SAS(ESASType::SAS_NOT);
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]()
		{
				//static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();
		})
		.AddTransition("BRAINCRASH_CUTSCENE to BRAINCRASH_ACTIVATE", "BRAINCRASH_ACTIVATE")
			.Predicator([&]()->_bool { return m_pSasPortrait->isFinish(); })
			.Priority(0)

		.AddState("BRAINCRASH_ACTIVATE")
		.OnStart([&]()
		{
			CGameObject* pTarget = nullptr;

			if (pTarget = CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_float4 vTargetPos = pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_float4 vDistance = XMLoadFloat4(&vTargetPos) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = vDistance.Length();

				if (pTarget->GetPrototypeTag() == L"Monster_em210")
				{
					m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_BrandCrash_em0200);
					static_cast<CEM0210*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					if (5.f >= fDistance)
					{
						auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em0210_brainCrash");
						m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayerCam, m_pTransformCom, 0.f, 0.f);
						//m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), m_pTransformCom, 0.f, 0.f);

						_vector BC_Pos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f);
						_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
						pTarget->GetTransform()->LookAt_NonY(vPlayerPos);
						pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
					}
					else
					{
						auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("BrainCrush01");
						m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayerCam, m_pTransformCom, 0.f, 0.f);
						m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_BrainCrash_Activate);
					}
				}
				else if (pTarget->GetPrototypeTag() == L"Monster_em700")
				{
					m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_BrainCrash_em0700);
					static_cast<CEM0700*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					if (5.f >= fDistance)
					{
						_vector BC_Pos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f);
						_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
						pTarget->GetTransform()->LookAt_NonY(BC_Pos);
						pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
					}
					else
					{
						auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("BrainCrush01");
						m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayerCam, m_pTransformCom, 0.f, 0.f);
						m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_BrainCrash_Activate);
					}
				}
				else if (pTarget->GetPrototypeTag() == L"Monster_em800")
				{
					m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_BrainCrash_em0800);
					static_cast<CEM0800*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					_vector BC_Pos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 8.5f);
					_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					pTarget->GetTransform()->LookAt_NonY(vPlayerPos);
					pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
				}

				else if (pTarget->GetPrototypeTag() == L"Monster_em1100")
				{
					m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_BrainCrash_em1100);
					static_cast<CEM0800*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					_vector BC_Pos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 8.f);
					BC_Pos += (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * 3.f);
					_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					pTarget->GetTransform()->LookAt_NonY(vPlayerPos);
					pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
				}
			}
			else
			{
				//static_cast<CEM0210*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

				/*auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("BrainCrush01");
				m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, m_pPlayer->m_pTransformCom, 0.f, 0.f);
				m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_Activate);*/

				//m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_BrainCrash_em1100);
			}
		})
		.Tick([&](double fTimeDelta)
		{
			/*string szCurAnimName = m_pASM->GetSocketAnimation("BrainCrash_AnimSocket")->GetName();
			_float4 vLocal =m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
			m_pTransformCom->LocalMove({ vLocal.x, vLocal.y, vLocal.z });*/

			/*if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetPrototypeTag() == L"Monster_em800")
			{
			
			}*/
		})
		.OnExit([&]()
		{

		})
			.AddTransition("BRAINCRASH_ACTIVATE to BRAINCRASH_NOUSE", "BRAINCRASH_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("BrainCrash_AnimSocket"); })
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_HBeamStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_start0"));
	m_HBeam_Charge_L.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_loop0"));
	m_HBeam_Charge_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_end0"));
	m_HBeam_Cancel_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LR_start"));
	m_HBeam_Throw_L.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LR_loop"));
	m_HBeam_Throw_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_333_AL_rotation_start"));
	m_HBeam_Rotation_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_333_AL_rotation_loop"));
	m_HBeam_Rotation_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_333_AL_rotation_cancel"));
	m_HBeam_End_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_333_AL_rotation_end"));
	m_HBeam_Finish_L.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LR_end"));
	m_HBeam_Fail_L.push_back(pAnimation);

	m_pHBeamStateMachine_Left =
		CFSMComponentBuilder()
		.InitState("HBEAM_LEFT_NOUSE")

		.AddState("HBEAM_LEFT_NOUSE")
		.OnStart([&]()
		{
			m_bKineticSpecial_Activate = false;
			m_bKineticSpecial = false;
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			m_bKineticSpecial = false;
		})
		.OnExit([&]()
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
			.AddTransition("HBEAM_LEFT_NOUSE to HBEAM_LEFT_CHARGE", "HBEAM_LEFT_CHARGE")
			.Predicator([&]()->_bool 
			{
				_bool bResult = (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject());
				return CanKinetic(50) && m_bKineticG && bResult;
			})
			.Priority(0)


		.AddState("HBEAM_LEFT_CHARGE")
		.OnStart([&]() 
		{
			m_bKineticSpecial = true;
			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_Charge_L);
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Drift();

			m_fSpecialCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge * 0.5f);
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge);
		})
			.AddTransition("HBEAM_LEFT_CHARGE to HBEAM_LEFT_THROW", "HBEAM_LEFT_THROW")
			.Predicator([&]()->_bool { return m_fSpecialCharge >= 2.f; })
			.Priority(0)

			.AddTransition("HBEAM_LEFT_CHARGE to HBEAM_LEFT_CANCEL", "HBEAM_LEFT_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)


		.AddState("HBEAM_LEFT_CANCEL")
		.OnStart([&]()
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_Cancel_L);
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})

			.AddTransition("HBEAM_LEFT_CANCEL to HBEAM_LEFT_CANCEL", "HBEAM_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG); 
			})
			.Priority(0)


		.AddState("HBEAM_LEFT_THROW")
		.OnStart([&]() 
		{
			m_bKineticSpecial_Activate = true;
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_Throw_L);
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->SpecialRimLightFix(true);
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Set_Used();

		})
		.Tick([&](double fTimeDelta)
		{
			if (2 == m_pASM->GetSocketSize("Kinetic_Special_AnimSocket") &&
				0.8f <= m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio())
			{

				if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					_float4 vObjPos = CPlayerInfoManager::GetInstance()->Get_SpecialObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					_float4 vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetColliderPosition();
					_vector vDir = XMLoadFloat4(&vTargetPos) - XMLoadFloat4(&vObjPos);
					XMVectorSetW(vDir, 0.f);
					vDir = XMVector3Normalize(vDir);
					static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Throw(vDir);
				}
				else
				{
					_float4 vCamLook = CGameInstance::GetInstance()->Get_CamLook();
					vCamLook.Normalize();
					static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Throw(vCamLook);
				}
			}
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("HBEAM_LEFT_THROW to HBEAM_LEFT_WAIT", "HBEAM_LEFT_WAIT")
			.Predicator([&]()->_bool 
			{
				_bool bAddAble = static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_AddAble();
				return bAddAble && (false == static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_isDecomposed()) && (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.05f));
			})
			.Priority(0)

			.AddTransition("HBEAM_LEFT_THROW to HBEAM_LEFT_FAIL", "HBEAM_LEFT_FAIL")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.2f));
			})
			.Priority(0)

		.AddState("HBEAM_LEFT_FAIL")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_Fail_L);
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_SetTrigger(true);
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_SetDeadTimer();

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("HBEAM_LEFT_FAIL to HBEAM_LEFT_NOUSE", "HBEAM_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
			})
			.Priority(0)

		.AddState("HBEAM_LEFT_WAIT")
		.OnStart([&]() 
		{
			CGameInstance::GetInstance()->SetTimeRatioCurve("KineticSpecialWaiting");

			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_Rotation_L);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();

			CreateSpecialUI(SPECIAL_HBEAM_BUNDLE);
		})
		.Tick([&](double fTimeDelta)
		{
			m_fSpecialWaiting += g_fTimeDelta;
		})
		.OnExit([&]()
		{
			CGameInstance::GetInstance()->ResetTimeRatio();
			m_fSpecialCharge = 0.f;
			m_fSpecialWaiting = 0.f;
		})
			.AddTransition("HBEAM_LEFT_WAIT to HBEAM_LEFT_ROTATION", "HBEAM_LEFT_ROTATION")
			.Predicator([&]()->_bool 
			{ 
				return m_bLeftClick; 
			})
			.Priority(0)

			.AddTransition("HBEAM_LEFT_WAIT to HBEAM_LEFT_END", "HBEAM_LEFT_END")
			.Predicator([&]()->_bool 
			{ 
				return (5.f <= m_fSpecialWaiting);
			})
			.Priority(0)


		.AddState("HBEAM_LEFT_END")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_End_L);
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_SetTrigger(true);
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_SetDeadTimer();

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("HBEAM_LEFT_END to HBEAM_LEFT_NOUSE", "HBEAM_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
			})
			.Priority(0)

		.AddState("HBEAM_LEFT_ROTATION")
		.OnStart([&]() 
		{
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_Catch();
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_Turn();

			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Collision();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("HBEAM_LEFT_ROTATION to HBEAM_LEFT_FINISH", "HBEAM_LEFT_FINISH")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket")); 
			})
			.Priority(0)

		.AddState("HBEAM_LEFT_FINISH")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_Finish_L);
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_Turn();

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.2f) && HBeam.IsNotDo())
				static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_Finish();
			else
				static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_Turn();
		})
		.OnExit([&]()
		{
			HBeam.Reset();
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("HBEAM_LEFT_FINISH to HBEAM_LEFT_NOUSE", "HBEAM_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket")); 
			})
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_DropObjectStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_334_AL_fall_start"));
	m_DropObject_Charge.push_back(m_pModel->Find_Animation("AS_ch0100_334_AL_fall_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_334_AL_fall_loop"));
	m_DropObject_Charge.push_back(m_pModel->Find_Animation("AS_ch0100_334_AL_fall_loop"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_334_AL_fall_cancel"));
	m_DropObject_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_334_AL_fall_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_334_AL_fall_end"));
	m_DropObject_Drop.push_back(m_pModel->Find_Animation("AS_ch0100_334_AL_fall_end"));

	m_pDropObjectStateMachine =
		CFSMComponentBuilder().InitState("DROP_NOUSE")

		.AddState("DROP_NOUSE")
		.OnStart([&]()
		{
			m_bKineticSpecial_Activate = false;
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_bKineticSpecial = false;
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
		.Tick([&](double fTimeDelta)
		{
			m_bKineticSpecial = false;
		})
		.OnExit([&]()
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
			.AddTransition("DROP_NOUSE to DROP_CHARGE", "DROP_CHARGE")
			.Predicator([&]()->_bool 
			{
				_bool bResult = (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject());
				return CanKinetic(50) && m_bKineticG && bResult;
			})
			.Priority(0)

		.AddState("DROP_CHARGE")
		.OnStart([&]() 
		{
			m_bKineticSpecial = true;
			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_DropObject_Charge);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
		})
		.Tick([&](double fTimeDelta)
		{
			m_fSpecialCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge * 0.5f);
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge);
		})
			.AddTransition("DROP_CHARGE to DROP_THROW", "DROP_THROW")
			.Predicator([&]()->_bool { return m_fSpecialCharge >= 2.f; })
			.Priority(0)

			.AddTransition("DROP_CHARGE to DROP_CANCEL", "DROP_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)

		.AddState("DROP_CANCEL")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_DropObject_Cancel);
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})
			.AddTransition("DROP_CANCEL to DROP_NOUSE", "DROP_NOUSE")
			.Predicator([&]()->_bool { return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG);  })
			.Priority(0)

		.AddState("DROP_THROW")
		.OnStart([&]() 
		{
			m_bKineticSpecial_Activate = true;
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_DropObject_Drop);

			static_cast<CSpecial_DropObject_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Set_Used();

			static_cast<CSpecial_DropObject_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->SpecialRimLightFix(true);
			static_cast<CSpecial_DropObject_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Set_Trigger(false);
			static_cast<CSpecial_DropObject_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->DropObject_Floating();
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.24f) && DropObject.IsNotDo())
			{
				static_cast<CSpecial_DropObject_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->DropObject_Drop();
			}
		})
		.OnExit([&]()
		{
			DropObject.Reset();
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("DROP_THROW to DROP_NOUSE", "DROP_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket");
			})
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_TankLorryStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_375_AL_Cap_start"));
	m_TankLorry_Start.push_back(m_pModel->Find_Animation("AS_ch0100_375_AL_Cap_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_375_AL_Cap_loop"));
	m_TankLorry_Start.push_back(m_pModel->Find_Animation("AS_ch0100_375_AL_Cap_loop"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_375_AL_Cap_cansel"));
	m_TankLorry_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_375_AL_Cap_cansel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_375_AL_Cap_end"));
	m_TankLorry_Finish.push_back(m_pModel->Find_Animation("AS_ch0100_375_AL_Cap_end"));

	m_pTankLorryStateMachine = 
		CFSMComponentBuilder().InitState("TANKLORRY_NOUSE")

		.AddState("TANKLORRY_NOUSE")
		.OnStart([&]()
		{
			m_bKineticSpecial_Activate = false;
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_bKineticSpecial = false;
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_bKineticSpecial = false;
		})
		.OnExit([&]()
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
		.AddTransition("TANKLORRY_NOUSE to TANKLORRY_CHARGE", "TANKLORRY_CHARGE")
		.Predicator([&]()->_bool
		{
			_bool bResult = (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject());
			return m_bKineticG && bResult;
		})
		.Priority(0)

		.AddState("TANKLORRY_CHARGE")
		.OnStart([&]()
		{
			m_bKineticSpecial = true;
			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_TankLorry_Start);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject())
			{
				static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
					->TankLorry_Activate();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject())
			{
				static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
					->TankLorry_Shake(10.f * m_fSpecialCharge);

				if (1.f <= m_fSpecialCharge && TankLorry.IsNotDo())
				{
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Bounce(100.f);
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Cheage_TankIndex(2);
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Create_OilParticle();
				}
			}

			m_fSpecialCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge * 0.5f);
		})
		.OnExit([&]()
		{
			TankLorry.Reset();
			m_fSpecialCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge);
		})
			.AddTransition("TANKLORRY_CHARGE to TANKLORRY_FINISH", "TANKLORRY_FINISH")
			.Predicator([&]()->_bool { return m_fSpecialCharge >= 2.f; })
			.Priority(0)

			.AddTransition("TANKLORRY_CHARGE to TANKLORRY_CANCEL", "TANKLORRY_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)

		.AddState("TANKLORRY_CANCEL")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TankLorry_Cancel);
			static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				TankLorry_Release_Particle();
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})
			.AddTransition("TANKLORRY_CANCEL to TANKLORRY_NOUSE", "TANKLORRY_NOUSE")
			.Predicator([&]()->_bool { return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG);  })
			.Priority(0)

		.AddState("TANKLORRY_FINISH")
		.OnStart([&]() 
		{
			static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
				->Set_Used();


			m_bKineticSpecial_Activate = true;
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TankLorry_Finish);
			static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
				->SpecialRimLightFix(true);
		})
		.Tick([&](double fTimeDelta)
		{
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject())
			{

				if (!m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.38f))
				{
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Shake(20.f);
				}

				if (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.17f) && TankLorry.IsNotDo())
				{
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Bounce(100.f);
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Cheage_TankIndex(3);
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Release_OilParticle();
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Explosion_Effect();
				}

				if (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.35f) && TankLorry_Effect.IsNotDo())
				{
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Explosion_Particle();
				}

				if (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.38f) && TankLorry_Exploision.IsNotDo())
				{
					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Cheage_TankIndex(4);

					static_cast<CSpecial_TankLorry*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
						->TankLorry_Explosion();
				}
			}
		})
		.OnExit([&]()
		{
			TankLorry.Reset();
			TankLorry_Exploision.Reset();
			TankLorry_Effect.Reset();
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("TANKLORRY_FINISH to TANKLORRY_NOUSE", "TANKLORRY_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket");
			})
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_IronBarsStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_start0"));
	m_IronBars_Charge.push_back(m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_start0"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_loop0"));
	m_IronBars_Charge.push_back(m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_loop0"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_end0"));
	m_IronBars_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_321_AL_cap_L_end0"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LL_start"));
	m_IronBars_Decompose.push_back(m_pModel->Find_Animation("AS_ch0100_325_AL_throw_LL_start"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_329_AL_throw_LR_start"));
	m_IronBars_Start.push_back(m_pModel->Find_Animation("AS_ch0100_329_AL_throw_LR_start"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_329_AL_throw_LR_end"));
	m_IronBars_End.push_back(m_pModel->Find_Animation("AS_ch0100_329_AL_throw_LR_end"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_pull_start"));
	m_IronBars_Charge_Ex.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_pull_start"));
	
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_pull_loop"));
	m_IronBars_Wait_Ex.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_pull_loop"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_pull_cansel"));
	m_IronBars_Cancel_Ex.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_pull_cansel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_throw1_start"));
	m_IronBars_Throw_01.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_throw1_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_throw1_loop"));
	m_IronBars_Throw_01.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_throw1_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_throw2_start"));
	m_IronBars_Throw_02.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_throw2_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_throw3_start"));
	m_IronBars_Throw_03.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_throw3_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_throw4_start"));
	m_IronBars_Throw_04.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_throw4_start"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_348_AL_throw5_start"));
	m_IronBars_Finish.push_back(m_pModel->Find_Animation("AS_ch0100_348_AL_throw5_start"));

	m_pIronBarsStateMachine = 
		CFSMComponentBuilder()
		.InitState("IRONBARS_NOUSE")

		.AddState("IRONBARS_NOUSE")
		.OnStart([&]()
		{
			m_bKineticSpecial_Activate = false;
			m_bKineticSpecial = false;
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			m_bKineticSpecial = false;
		})
		.OnExit([&]()
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})

			.AddTransition("IRONBARS_NOUSE to IRONBARS_CHARGE", "IRONBARS_CHARGE")
			.Predicator([&]()->_bool 
			{
				_bool bResult = (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject());
				return CanKinetic(50) && m_bKineticG && bResult;
			})
			.Priority(0)

		.AddState("IRONBARS_CHARGE")
		.OnStart([&]() 
		{
			m_bKineticSpecial = true;
			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Charge);
		})
		.Tick([&](double fTimeDelta)
		{
			// ??≤®?òÍ????†ÎãàÎ©îÏù¥???ôÏûë
			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_AnimActive(true);

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SingleBars_Particle();

			m_fSpecialCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge * 0.5f);
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge);
		})
			.AddTransition("IRONBARS_CHARGE to IRONBARS_DECOMPOSE", "IRONBARS_DECOMPOSE")
			.Predicator([&]()->_bool { return m_fSpecialCharge >= 2.f; })
			.Priority(0)

			.AddTransition("IRONBARS_CHARGE to IRONBARS_CANCEL", "IRONBARS_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)

		.AddState("IRONBARS_CANCEL")
		.OnStart([&]()
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Cancel);

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_AnimActive(false);
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})

			.AddTransition("IRONBARS_CANCEL to IRONBARS_NOUSE", "IRONBARS_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG); 
			})
			.Priority(0)

		.AddState("IRONBARS_DECOMPOSE")
		.OnStart([&]() 
		{
			m_bKineticSpecial_Activate = true;
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Decompose);

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				SpecialRimLightFix(true);

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				Set_Used();
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.2f) && IronBarsDecomposeEffect.IsNotDo())
			{
				static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
					IronBars_DecomposeEffect();
			}

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SingleBars_Particle();
		})
		.OnExit([&]()
		{
			IronBarsDecomposeEffect.Reset();

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_Decompose(true);

			m_fSpecialCharge = 0.f;
		})

			.AddTransition("IRONBARS_DECOMPOSE to IRONBARS_START", "IRONBARS_START")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"); 
			})
			.Priority(0)

		.AddState("IRONBARS_START")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Start);
		})
		.Tick([&](double fTimeDelta)
		{
			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio();
			
			if (0.5f >= fRatio)
			{
				if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				{
					_float4 vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetColliderPosition();
					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_LookAtTarget(vTargetPos, fRatio * 2.f);
				}
				// Î∞©Ìñ• Î≥¥Í∞Ñ
			}
			else
			{
				if (IronBars.IsNotDo())
				{
					if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
					{
						_float4 vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetColliderPosition();
						static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
							IronBars_Shooting_All(vTargetPos);
					}
					else
					{
						static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
							IronBars_Shooting_All(XMVectorSet(0.f, 0.f, 0.f, 0.f));
					}
				}
				// ?†ÏïÑÍ∞?
			}

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SingleBars_Particle();

			// ?§Ïù¥?òÎ??ºÎ°ú Î≥ÄÍ≤ΩÌïòÍ≥??†ÏïÑÍ∞Ä Î∂Ä?™ÌûàÍ∏?
		})
		.OnExit([&]()
		{
			IronBars.Reset();
		})
			
			.AddTransition("IRONBARS_START to IRONBARS_CHARGE_EX", "IRONBARS_CHARGE_EX")
			.Predicator([&]()->_bool 
			{ 
				_bool m_bAddAble = static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_AddAble();
				return m_bAddAble && m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket");
			})
			.Priority(0)

			.AddTransition("IRONBARS_START to IRONBARS_END", "IRONBARS_END")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket");
			})
			.Priority(0)

		.AddState("IRONBARS_CHARGE_EX")
		.OnStart([&]() 
		{
			CGameInstance::GetInstance()->SetTimeRatioCurve("KineticSpecialWaiting");
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Charge_Ex);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();

			CreateSpecialUI(SPECIAL_IRONBARS);
		})
		.Tick([&](double fTimeDelta)
		{
			// Î™¨Ïä§?∞Ï? ?†Ï∞Ω??Ï∂©Îèå ??Ï∂îÍ??Ä ?ÄÍ∏∞Ìï® -> ?¨Í∏¥?®Ïù¥?ÖÏûÑ
			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SingleBars_Particle();

			m_fSpecialWaiting += g_fTimeDelta;
		})
		.OnExit([&]()
		{
			CGameInstance::GetInstance()->ResetTimeRatio();
			m_fSpecialCharge = 0.f;
			m_fSpecialWaiting = 0.f;
		})

			.AddTransition("IRONBARS_CHARGE_EX to IRONBARS_END", "IRONBARS_END")
			.Predicator([&]()->_bool 
			{ 
				return 5.f <= m_fSpecialWaiting;
			})
			.Priority(0)

			.AddTransition("IRONBARS_CHARGE_EX to IRONBARS_RELOAD_EX", "IRONBARS_RELOAD_EX")
			.Predicator([&]()->_bool 
			{ 
				return m_bLeftClick; 
			})
			.Priority(0)


		.AddState("IRONBARS_END")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_End);

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SetDead();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})

			.AddTransition("IRONBARS_END to IRONBARS_NOUSE", "IRONBARS_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG); 
			})
			.Priority(0)

		.AddState("IRONBARS_RELOAD_EX")
		.OnStart([&]() 
		{
			
		})
		.Tick([&](double fTimeDelta)
		{
			// Î™¨Ïä§??Î∞©Ìñ•?ºÎ°ú, ?åÎ†à?¥Ïñ¥ ?ÑÏπò?êÏÑú 8Í∞?Î∞©Ìñ•?ºÎ°ú ?†Ï∞Ω??Î≥¥Í∞Ñ ?ïÎ†¨
			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio();
			
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_float4 vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetColliderPosition();
				_float4 vDestPos = GetColliderPosition() + m_vIronBars_ThrowPoins;
				static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
					IronBars_Reload(vDestPos, vTargetPos, fRatio);
			}

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SingleBars_Particle();
			
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})

			.AddTransition("IRONBARS_RELOAD_EX to IRONBARS_WAIT_EX", "IRONBARS_WAIT_EX")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"); 
			})
			.Priority(0)


		.AddState("IRONBARS_WAIT_EX")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Wait_Ex);
			CreateSpecialUI(SPECIAL_IRONBARS, true);
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SingleBars_Particle();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})

			.AddTransition("IRONBARS_WAIT_EX to IRONBARS_CANCEL_EX", "IRONBARS_CANCEL_EX")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket");
			})
			.Priority(0)

			.AddTransition("IRONBARS_WAIT_EX to IRONBARS_START_EX", "IRONBARS_START_EX")
			.Predicator([&]()->_bool 
			{ 
				return m_bLeftClick;
			})
			.Priority(0)


		.AddState("IRONBARS_CANCEL_EX")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Cancel_Ex);

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SetDead();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})

			.AddTransition("IRONBARS_CANCEL_EX to IRONBARS_NOUSE", "IRONBARS_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG); 
			})
			.Priority(0)


		.AddState("IRONBARS_START_EX")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Throw_01);
		})
		.Tick([&](double fTimeDelta)
		{
			// ?†Ï∞Ω???§Ïù¥?òÎ??ºÎ°ú Î≥ÄÍ≤ΩÌïòÍ≥?Î™¨Ïä§??Î∞©Ìñ•?ºÎ°ú Î∞úÏÇ¨, Ï∂©Îèå 0.5Ï¥????§ÎÑ§?±ÏúºÎ°?Î≥ÄÍ≤ΩÌïò???°Ïùå
			_float4 vTargetPos = { 0.f, 0.f, 0.f, 0.f };
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetColliderPosition();
			}

			if (m_bLeftClick && m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio() >= 0.2f)
			{
				if (m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetName() == "AS_ch0100_348_AL_throw1_loop")
				{
					m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Throw_02);

					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_Shooting_Single(vTargetPos, 0);
					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_Shooting_Single(vTargetPos, 1);
				}
				else if (m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetName() == "AS_ch0100_348_AL_throw2_start")
				{
					m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Throw_03);

					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_Shooting_Single(vTargetPos, 2);
					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_Shooting_Single(vTargetPos, 3);
				}
				else if (m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetName() == "AS_ch0100_348_AL_throw3_start")
				{
					m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Throw_04);

					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_Shooting_Single(vTargetPos, 4);
					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_Shooting_Single(vTargetPos, 5);
					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_Shooting_Single(vTargetPos, 6);
				}
			}

			static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				IronBars_SingleBars_Particle();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})

			.AddTransition("IRONBARS_START_EX to IRONBARS_FINISH_EX", "IRONBARS_FINISH_EX")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"); 
			})
			.Priority(0)

		.AddState("IRONBARS_FINISH_EX")
		.OnStart([&]() 
		{
			m_pKineticAnimModel->SetPlayAnimation("AS_ch0100_348_AL_obj_rod");
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_IronBars_Finish);
		})
		.Tick([&](double fTimeDelta)
		{
			if (nullptr != m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket"))
			{
				_float fPlayTime = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayTime();

				m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio_NonLocalLock(fPlayTime);
				m_pKineticAnimModel->Compute_CombindTransformationMatrix();

				_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio();

				if (0.3f > fRatio)
				{
					_float fDuration = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetCurDuration();
					m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio_NonLocalLock(fDuration * 0.2f);
					m_pKineticAnimModel->Compute_CombindTransformationMatrix();
				}


				if (0.3f > fRatio)
				{
					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_LerpAnim(m_pKineticAnimModel, m_pTransformCom, m_vIronBars_ThrowPoins, fRatio / 0.2f);
				}
				else if (0.3f <= fRatio && 0.6 > fRatio)
				{
					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_ChangeToBundle();

					static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						IronBars_AttachAnim_MulitBars(m_pKineticAnimModel, m_pTransformCom, m_vIronBars_ThrowPoins);
				}
				else if (0.6f <= fRatio && IronBars.IsNotDo())
				{
					if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
					{
						_float4 vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetColliderPosition();
						static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
							IronBars_Shooting_Finish(vTargetPos);
					}
					else
					{
						_float4 vTargetPos = {0.f, 0.f, 0.f, 0.f};
						static_cast<CSpecial_IronBars*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
							IronBars_Shooting_Finish(vTargetPos);
					}
				}
			}
		})
		.OnExit([&]()
		{
			IronBars.Reset();
			SyncEffectLocalMove("Kinetic_Special_AnimSocket");
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("IRONBARS_FINISH_EX to IRONBARS_NOUSE", "IRONBARS_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket");
			})
			.Priority(0)

	.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_ContainerStateMachine()
{
	CAnimation* pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_322_AL_cap_R_start0"));
	m_Container_Charge.push_back(m_pModel->Find_Animation("AS_ch0100_322_AL_cap_R_start0"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_322_AL_cap_R_loop0"));
	m_Container_Charge.push_back(m_pModel->Find_Animation("AS_ch0100_322_AL_cap_R_loop0"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_322_AL_cap_R_end0"));
	m_Container_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_322_AL_cap_R_end0"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_330_AL_fall_start"));
	m_Container_Start.push_back(m_pModel->Find_Animation("AS_ch0100_330_AL_fall_start"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_337_AL_press_start"));
	m_Container_Press.push_back(m_pModel->Find_Animation("AS_ch0100_337_AL_press_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_337_AL_press_loop"));
	m_Container_Press.push_back(m_pModel->Find_Animation("AS_ch0100_337_AL_press_loop"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_337_AL_press_cancel"));
	m_Container_End.push_back(m_pModel->Find_Animation("AS_ch0100_337_AL_press_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_337_AL_press_end"));
	m_Container_Finish.push_back(m_pModel->Find_Animation("AS_ch0100_337_AL_press_end"));


	m_pContainerStateMachine =
		CFSMComponentBuilder()
		.InitState("CONTAINER_NOUSE")

		.AddState("CONTAINER_NOUSE")
		.OnStart([&]()
		{
			m_bKineticSpecial_Activate = false;

			m_bKineticSpecial = false;
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}
		})
		.Tick([&](double fTimeDelta)
		{
			m_bKineticSpecial = false;
		})
		.OnExit([&]()
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
			.AddTransition("CONTAINER_NOUSE to CONTAINER_CHARGE", "CONTAINER_CHARGE")
			.Predicator([&]()->_bool 
			{
				_bool bResult = (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject());
				return CanKinetic(50) && m_bKineticG && bResult;
			})
			.Priority(0)

		.AddState("CONTAINER_CHARGE")
		.OnStart([&]() 
		{
			m_bKineticSpecial = true;
			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_Container_Charge);
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->CreateKineticParticle();
			m_fSpecialCharge += (_float)fTimeDelta;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge * 0.5f);
		})
		.OnExit([&]()
		{
				m_fSpecialCharge = 0.f;
			CPlayerInfoManager::GetInstance()->Set_SpecialCharge(m_fSpecialCharge);
		})
			.AddTransition("CONTAINER_CHARGE to CONTAINER_START", "CONTAINER_START")
			.Predicator([&]()->_bool { return m_fSpecialCharge >= 2.f; })
			.Priority(0)

			.AddTransition("CONTAINER_CHARGE to CONTAINER_CANCEL", "CONTAINER_CANCEL")
			.Predicator([&]()->_bool { return !m_bKineticG; })
			.Priority(0)

		.AddState("CONTAINER_CANCEL")
		.OnStart([&]()
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_Container_Cancel);
		})
		.OnExit([&]()
		{
			m_bKineticSpecial = false;
		})

			.AddTransition("CONTAINER_CANCEL to CONTAINER_NOUSE", "CONTAINER_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB || m_bKineticG); 
			})
			.Priority(0)

		.AddState("CONTAINER_START")
		.OnStart([&]() 
		{
			m_bKineticSpecial_Activate = true;
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_Container_Start);

			static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				Set_Used();

			static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				SpecialRimLightFix(true);
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pASM->isSocketEmpty("Kinetic_Special_AnimSocket")) return;

			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio();

			if (0.5f >= fRatio)
			{
				if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster() && nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject())
				{
					_float4 vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
						->GetColliderPosition();

					static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
						Lerp_to_TargetPoint(vTargetPos, (m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio() / 0.5f));
				}
			}
			else
			{
				static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
					Container_Press();
			}

			static_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->CreateKineticParticle();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})

			.AddTransition("CONTAINER_START to CONTAINER_WAIT", "CONTAINER_WAIT")
			.Predicator([&]()->_bool 
			{ 
				_bool bAddAble = static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_AddAble();
				return bAddAble && (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
			})
			.Priority(1)

			.AddTransition("CONTAINER_START to CONTAINER_END", "CONTAINER_END")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
			})
			.Priority(2)

		.AddState("CONTAINER_WAIT")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_Container_Press);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			CGameInstance::GetInstance()->SetTimeRatioCurve("KineticSpecialWaiting");

			CreateSpecialUI(SPECAIL_CONTAINER);
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->CreateKineticParticle();
			m_fSpecialWaiting += g_fTimeDelta;
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
			m_fSpecialWaiting = 0.f;
			CGameInstance::GetInstance()->ResetTimeRatio();
		})
			.AddTransition("CONTAINER_WAIT to CONTAINER_PRESS", "CONTAINER_PRESS")
			.Predicator([&]()->_bool
				{
					return m_bLeftClick;
				})
			.Priority(0)

			.AddTransition("CONTAINER_WAIT to CONTAINER_END", "CONTAINER_END")
			.Predicator([&]()->_bool
				{
					return 5.f <= m_fSpecialWaiting;
				})
			.Priority(0)


		.AddState("CONTAINER_PRESS")
		.OnStart([&]() 
		{
			CreateSpecialUI(SPECAIL_CONTAINER, true);
		})
		.Tick([&](double fTimeDelta)
		{
			if (nullptr == m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")) return;
			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) return;

			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio();

			if (m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetName() == "AS_ch0100_337_AL_press_start")
			{
				if (0.9f >= fRatio)
				{
					static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Container_Reposition(m_pTransformCom, fRatio / 0.9f, (1.f - (fRatio / 0.9f)));
				}
				else
				{
					_float4 vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
						->GetColliderPosition();

					static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Container_Press_Maintain(vTargetPos, fTimeDelta);
				}
			}
			else
			{
				_float4 vTargetPos = static_cast<CScarletCharacter*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
					->GetColliderPosition();

				static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Container_Press_Maintain(vTargetPos, fTimeDelta);

				if (0.f < fRatio && 0.3f >= fRatio)		static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Container_ChangeIndex(2);
				if (0.3f < fRatio && 0.6f >= fRatio)	static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Container_ChangeIndex(3);
				if (0.6f < fRatio && 0.9f >= fRatio)	static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Container_ChangeIndex(4);
			}

			static_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->CreateKineticParticle();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("CONTAINER_PRESS to CONTAINER_FINISH", "CONTAINER_FINISH")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket")); 
			})
			.Priority(0)

			.AddTransition("CONTAINER_PRESS to CONTAINER_FINISH", "CONTAINER_FINISH")
			.Predicator([&]()->_bool 
			{ 
				return (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster());
			})
			.Priority(1)

		.AddState("CONTAINER_END")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_Container_End);
			static_cast<CCamSpot*>(m_pCamSpot)->Reset_CamMod();

			if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialUI))
			{
				m_pSpecialUI->SetDelete();
			}

			static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				SpecialRimLightFix(false);
		})
		.Tick([&](double fTimeDelta)
		{
				static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
					Container_SetDead();
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("CONTAINER_END to CONTAINER_NOUSE", "CONTAINER_NOUSE")
			.Predicator([&]()->_bool
				{
					return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
				})
			.Priority(0)

		.AddState("CONTAINER_FINISH")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_Container_Finish);

			static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->
				SpecialRimLightFix(false);
		})
		.Tick([&](double fTimeDelta)
		{
			if (nullptr == m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")) return;
			//if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) return;

			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio();

			if (0.15f >= fRatio)
			{
				if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) return;
				static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Container_Reposition(m_pTransformCom, fRatio / 0.15f, (1.f - (fRatio / 0.15f)));
			}
			else
			{
				static_cast<CSpecial_Container*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Container_Press_Finish();
			}
		})
		.OnExit([&]()
		{
			m_fSpecialCharge = 0.f;
		})
			.AddTransition("CONTAINER_FINISH to CONTAINER_NOUSE", "CONTAINER_NOUSE")
			.Predicator([&]()->_bool
				{
					return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
				})
			.Priority(0)

		.Build();

	return S_OK;
}

HRESULT CPlayer::Setup_AnimSocket()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_104_Up_trans_neutral"));
	m_TransNeutralSocket.push_back(m_pModel->Find_Animation("AS_ch0100_104_Up_trans_neutral"));

	return S_OK;
}

_bool CPlayer::isPlayerAttack(void)
{
	string szCurAnim = "";

	if (nullptr != m_pModel->GetPlayAnimation())
		szCurAnim = m_pModel->GetPlayAnimation()->GetName();

	if (Detector())
	{
		m_bOnBattle = true;
		return true;
	}

	else if (m_bBrainField)
	{
		m_bOnBattle = true;
		return true;
	}

	else if (szCurAnim.find("AS_ch0100_2") != string::npos)
	{
		m_bOnBattle = true;
		return true;
	}

	else if (szCurAnim.find("AS_ch0100_3") != string::npos)
	{
		m_bOnBattle = true;
		return true;
	}

	else if (szCurAnim.find("AS_ch0100_4") != string::npos)
	{
		m_bOnBattle = true;
		return true;
	}

	else if (szCurAnim.find("AS_ch0100_5") != string::npos)
	{
		m_bOnBattle = true;
		return true;
	}

	else if (!CPlayerInfoManager::GetInstance()->Get_PlayerSasList().empty())
	{
		m_bOnBattle = true;
		return true;
	}

	else if (m_bHit)
	{
		m_bOnBattle = true;
		return true;
	}

	return false;
}

_bool CPlayer::isPlayerNonAttack(void)
{
	_bool bResult = 
		(m_pASM->GetCurStateName() == "IDLE") ||
		(m_pASM->GetCurStateName() == "WALK") ||
		(m_pASM->GetCurStateName() == "WALK_START_FRONT") ||
		(m_pASM->GetCurStateName() == "WALK_START_LEFT") ||
		(m_pASM->GetCurStateName() == "WALK_START_BACK_LEFT") ||
		(m_pASM->GetCurStateName() == "WALK_START_BACK_RIGHT") ||
		(m_pASM->GetCurStateName() == "WALK_LOOP") ||
		(m_pASM->GetCurStateName() == "WALK_END") ||
	/*	(m_pASM->GetCurStateName() == "DASH") ||
		(m_pASM->GetCurStateName() == "DASH_START_FRONT") ||
		(m_pASM->GetCurStateName() == "DASH_END_FRONT") ||
		(m_pASM->GetCurStateName() == "DASH_START_LEFT") ||
		(m_pASM->GetCurStateName() == "DASH_END_LEFT") ||
		(m_pASM->GetCurStateName() == "DASH_START_RIGHT") ||
		(m_pASM->GetCurStateName() == "DASH_END_RIGHT") ||
		(m_pASM->GetCurStateName() == "DASH_START_BACK") ||
		(m_pASM->GetCurStateName() == "DASH_END_BACK") ||*/
		(m_pASM->GetCurStateName() == "RUN_FRONT") ||
		(m_pASM->GetCurStateName() == "RUN_END") || 
		
		(m_pASM->GetCurStateName() == "JUMP_START") ||
		(m_pASM->GetCurStateName() == "JUMP_RISE") ||
		(m_pASM->GetCurStateName() == "JUMP_FALL") ||
		(m_pASM->GetCurStateName() == "JUMP_LANDING") ||
		(m_pASM->GetCurStateName() == "DOUBLE_JUMP_RISE") ||
		
		(m_pASM->GetCurStateName() == "RUNJUMP_START") ||
		(m_pASM->GetCurStateName() == "RUNJUMP_RISE") ||
		(m_pASM->GetCurStateName() == "RUNJUMP_FALL") ||
		(m_pASM->GetCurStateName() == "RUNJUMP_LANDING") ||
		(m_pASM->GetCurStateName() == "DOUBLE_RUNJUMP_RISE") ||

		(m_pASM->GetCurStateName() == "DASHJUMP_START") ||
		(m_pASM->GetCurStateName() == "DASHJUMP_RISE") ||
		(m_pASM->GetCurStateName() == "DASHJUMP_FALL") ||
		(m_pASM->GetCurStateName() == "DASHJUMP_LANDING") ||
		(m_pASM->GetCurStateName() == "DOUBLE_DASHJUMP_RISE")
		
		;

	return bResult;
}

_bool CPlayer::BeforeCharge(_float fBeforeCharge)
{
	if (fBeforeCharge > m_fBefCharge)
	{
		m_fBefCharge += g_fTimeDelta;
		return false;
	}
	else
	{
		m_fBefCharge = 0.f;
		return true;
	}
}

_bool CPlayer::Charge(_uint iNum, _float fCharge)
{
	if (fCharge > m_fCharge[iNum])
	{
		m_fCharge[iNum] += g_fTimeDelta;

		if ((0 == iNum) && fCharge <= m_fCharge[iNum])
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Default_Attack_Charge_Effect_01"), LAYER_PLAYEREFFECT)->Start_Attach(this, "RightWeapon");
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Charge_White_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "RightWeapon");

		/*	if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
			{
				_matrix EffectPivot = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Default_Charge_1_MeshParticle"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivot, "RightWeapon", true, false);
			}
			else if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			{
				_matrix EffectPivot = XMMatrixScaling(5.f, 5.f, 5.f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Fire_Charge_1_MeshParticle"), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, EffectPivot, "RightWeapon", true, false);
			}*/

		}
		if ((1 == iNum) && fCharge <= m_fCharge[iNum])
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Default_Attack_Charge_Effect_02"), LAYER_PLAYEREFFECT)->Start_Attach(this, "RightWeapon");
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Charge_White_Particle"), LAYER_PLAYEREFFECT)->Start_Attach(this, "RightWeapon");
			
			if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
			{
				_matrix EffectPivot = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(-20.f)) * XMMatrixTranslation(0.f, 0.4f, -0.5f);
				m_pChargeParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Default_Charge_2_MeshParticle"), LAYER_PLAYEREFFECT);
				m_pChargeParticle->Start_AttachPivot(this, EffectPivot, "RightWeapon", true, false);
			}
			else if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			{
				_matrix EffectPivot = XMMatrixScaling(5.f, 5.f, 5.f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
				m_pChargeParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Fire_Charge_2_MeshParticle"), LAYER_PLAYEREFFECT);
				m_pChargeParticle->Start_AttachPivot(this, EffectPivot, "RightWeapon", true, false);
			}
		}


		return false;
	}
	else
	{
		return true;
	}
}

CPlayer & CPlayer::SetAbleState(REMOTE tagRemote)
{
	m_bCanTurn = tagRemote.bCanTurn;
	m_bCanMove = tagRemote.bCanMove;
	m_bCanRun = tagRemote.bCanRun;
	m_bCanTurn_Attack = tagRemote.bAttackTurn;

	m_bAir = tagRemote.bOnAir;
	m_bActiveGravity = tagRemote.bGravity;

	if (tagRemote.bMoveLimitReset) { MoveLimitReset(); }
	if (tagRemote.bAttackLimitReset) { AttackLimitReset(); }
	if (tagRemote.bChargeReset) { Reset_Charge(); }

	return *this;
}

void CPlayer::Event_Effect(string szEffectName, _float fSize, string szBoneName)
{
	wstring EffectName = s2ws(szEffectName);

	_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vRight.Normalize();
	_float4 vLeft = vRight * -1.f;

	switch (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type)
	{
	case ESASType::SAS_NOT:
		if (szEffectName.find("Fire") != string::npos || szEffectName.find("Elec") != string::npos)
			break;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach(this, szBoneName);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, szBoneName, false);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, szBoneName, false);
		}
		break;
	case ESASType::SAS_FIRE:
		if (szEffectName.find("Fire") == string::npos)
			break;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach(this, szBoneName);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, szBoneName, false);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, szBoneName, false);
		}
		break;
	case ESASType::SAS_ELETRIC:
		if (szEffectName.find("Elec") == string::npos)
			break;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach(this, szBoneName);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, szBoneName, false);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, szBoneName, false);
		}
		break;
	}
}

void CPlayer::Event_EffectSound(const string& strSoundName)
{
	switch (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type)
	{
	case ESASType::SAS_NOT:
		if (strSoundName.find("nor") != string::npos)
			m_SoundStore.PlaySound(strSoundName, m_pTransformCom);
			break;
		break;
	case ESASType::SAS_FIRE:
		if (strSoundName.find("fire") != string::npos)
			m_SoundStore.PlaySound(strSoundName, m_pTransformCom);
			break;
		break;
	}
}

void CPlayer::Event_ElecEffect(string szEffectName)
{
	if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type != ESASType::SAS_ELETRIC)
		return;

	wstring EffectName = s2ws(szEffectName);

	if (szEffectName == "Elec_Attack_1")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(1.f, 0.f, 1.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_1_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(1.f, 0.f, 1.5f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_1", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_2")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f) * 
			XMMatrixRotationX(XMConvertToRadians(70.f)) *
			XMMatrixRotationY(XMConvertToRadians(-90.f)) *
			XMMatrixRotationZ(XMConvertToRadians(90.f)) *
			XMMatrixTranslation(0.f, 0.5f, -2.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_2_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f) *
			XMMatrixRotationX(XMConvertToRadians(70.f)) *
			XMMatrixRotationY(XMConvertToRadians(-90.f)) *
			XMMatrixRotationZ(XMConvertToRadians(90.f)) *
			XMMatrixTranslation(0.f, 0.5f, -2.5f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_2", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_3")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f) *
			XMMatrixTranslation(0.f, 0.f, 1.f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_3_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f) *
			XMMatrixTranslation(0.f, 0.f, 1.f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_3", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_4_002")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_4_002_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_4_002", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_4_003")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	if (szEffectName == "Elec_Attack_4_003_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_4_003", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_4_004")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_4_004_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.6f, 0.6f, 0.6f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_4_004", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_5")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.f, 0.f, -1.f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_5_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.f, 0.f, -1.f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_5", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_Air_1")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_1_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_Air_1", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_Air_2")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_2_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_Air_2", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);

		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_00")
	{	
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_01")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_02")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_03")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_04")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_05")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_Landing")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Charge_01")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Charge_02")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Charge_03")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_1")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_2_001")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_2_002")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_3")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_Hold_0")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.f, 0.5f, 0.f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_Hold_1")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.f, 0.5f, 0.f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_Hold_2")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.f, 0.5f, 0.f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_Hold_2_Shoot")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.f, 0.5f, 0.f);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_Dash_Hold_2", LAYER_PLAYEREFFECT)->Start_AttachPivotMove(this, matEffect, "Eff01", vLook, true);
		ElecSweep();
	}
	else if (szEffectName == "Elec_Attack_Upper_0")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Upper_1")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_1_001")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) 
			* XMMatrixRotationY(XMConvertToRadians(90.f))
			* XMMatrixTranslation(5.f, -2.f, -1.25f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}

	else if (szEffectName == "Elec_Attack_Justdodge_0")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_1")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_2")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_3")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_4")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_5")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}

	else if (szEffectName == "Elec_Attack_Dash_End")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.f, -1.f, -1.f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_Air_Dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_Air_Dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, L"Elec_Attack_Air_Dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Dash_0")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Dash_1")
	{
		_matrix matEffect = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matEffect, "Eff01", true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matEffect, "Eff01", true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName, LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matEffect, "Eff01", true);
		}
	}

}

void CPlayer::Event_ElecParticle(string szEffectName)
{
	if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type != ESASType::SAS_ELETRIC)
		return;

	if (szEffectName == "Elec_Attack_1_Particle")
	{
		_matrix matParticle = XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(1.f, 0.f, 1.5f);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_1", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matParticle, "Eff01", true, false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_1"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matParticle, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_1"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matParticle, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_2_Particle")
	{
		_matrix matParticle = XMMatrixRotationX(XMConvertToRadians(70.f)) *
			XMMatrixRotationY(XMConvertToRadians(-90.f)) *
			XMMatrixRotationZ(XMConvertToRadians(90.f)) *
			XMMatrixTranslation(0.f, 0.5f, -2.5f);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_4", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matParticle, "Eff01", true, false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_4"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matParticle, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_4"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matParticle, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_3_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_3", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_3"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_3"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_4_003_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_4_003", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_4_003"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_4_003"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_4_004_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_4_004", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_4_004"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_4_004"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_5_Particle")
	{
		_matrix matParticle = XMMatrixTranslation(0.f, 0.f, -1.f);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_5", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matParticle, "Eff01", true, false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_5"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matParticle, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_5"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matParticle, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_1_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_1", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_2_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_2", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_2"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_2"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_00_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Hold_00", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_00"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_00"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_01_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Hold_01", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_01"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_01"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_02_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Hold_02", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_02"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_02"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_03_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Hold_03", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_03"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_03"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_04_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Hold_04", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_04"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_04"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_05_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Hold_05", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_05"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_05"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Hold_Landing_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Hold_Landing", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_Landing"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Hold_Landing"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Charge_01_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Charge_01", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Charge_01"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Charge_01"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Charge_02_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Charge_02", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Charge_02"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Charge_02"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Charge_03_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Charge_03", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Charge_03"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Charge_03"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_1_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Dash_1", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_2_001_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Dash_2_001", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_2_001"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_2_001"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_2_002_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Dash_2_002", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_2_002"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_2_002"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_3_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Dash_3", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_3"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_3"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_Hold_0_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Dash_Hold_0", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_Hold_0"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_Hold_0"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_Hold_1_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Dash_Hold_1", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_Hold_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_Hold_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_Hold_2_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Dash_Hold_2", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_Hold_2"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Dash_Hold_2"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Upper_0_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Upper_0", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Upper_0"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Upper_0"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Upper_1_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Upper_1", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Upper_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Upper_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_0_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Justdodge_0", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_0"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_0"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_1_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Justdodge_1", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_2_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Justdodge_2", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_2"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_2"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_3_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Justdodge_3", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_3"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_3"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_4_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Justdodge_4", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_4"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_4"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Justdodge_5_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Justdodge_5", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_5"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Justdodge_5"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Dash_End_Particle")
	{
		_matrix matParticle = XMMatrixTranslation(0.f, -1.f, -1.f);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Dash_0", LAYER_PLAYEREFFECT)->Start_AttachPivot(this, matParticle, "Eff01", true, false, true);
	
		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Dash_0"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vLeft, matParticle, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Dash_0"), LAYER_PLAYEREFFECT)->Start_AttachPivot_Vector(this, vRight, matParticle, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Dash_0_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Dash_0", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Dash_0"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Dash_0"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
	else if (szEffectName == "Elec_Attack_Air_Dash_1_Particle")
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Elec_Attack_Air_Dash_1", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", false, true);

		if (CPlayerInfoManager::GetInstance()->Get_Copy())
		{
			_float4 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vRight.Normalize();
			_float4 vLeft = vRight * -1.f;

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Dash_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vLeft, "Eff01", true);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, TEXT("Elec_Attack_Air_Dash_1"), LAYER_PLAYEREFFECT)->Start_Attach_Vector(this, vRight, "Eff01", true);
		}
	}
}

void CPlayer::Event_CollisionStart()
{
	m_bAttackEnable = true;

	if (m_pASM->isSocketExactlyEmpty())
	{
		string szCurAttackState = m_pASM->GetCurStateName();

		const auto iter = m_mapCollisionEvent.find(szCurAttackState);

		if (iter == m_mapCollisionEvent.end())
			return;

		iter->second();
	}
	else
	{
		string szCurSocketAnim = m_pASM->GetCurSocketAnimName();

		const auto iter = m_mapCollisionEvent.find(szCurSocketAnim);

		if (iter == m_mapCollisionEvent.end())
			return;

		iter->second();
	}
}

void CPlayer::Event_collisionEnd()
{
	m_bAttackEnable = false;
}

void CPlayer::Event_CamMod()
{
	if (nullptr != m_pCamSpot)
		m_pCamSpot->Switch_CamMod();
}

void CPlayer::Event_FinishFovActionCam()
{
	CGameInstance::GetInstance()->ReleaseCameraFovCurve();
}

void CPlayer::Event_Kinetic_Throw()
{
	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject() && (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster()))
	{
		_vector vKineticPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Throw_Kinetic_Distortion"), LAYER_PLAYEREFFECT)->Start_AttachOnlyPos(vKineticPos);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Kinetic_Shoot_Smoke"), LAYER_PLAYEREFFECT)->Start_AttachPosition(this, vKineticPos, _float4(0.f, 1.f, 0.f, 0.f));

		//Vector4 vTargetPos = static_cast<CMonster*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetKineticTargetPos();
		Vector4 vTargetPos = static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetKineticTargetPos();
		Vector4 vMyPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		Vector4 vDir = vTargetPos - vMyPos;
		vDir.w = 0.f;
		vDir = XMVector3Normalize(vDir);

		_float3 vForce = { vDir.x, vDir.y, vDir.z };
		vForce *= m_fThrowPower;
		static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Add_Physical(vForce);
		static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->SetThrow();

		m_SoundStore.PlaySound("fx_kinetic_shot", m_pTransformCom);
	}
	else if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		_vector vKineticPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Throw_Kinetic_Distortion"), LAYER_PLAYEREFFECT)->Start_AttachOnlyPos(vKineticPos);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Kinetic_Shoot_Smoke"), LAYER_PLAYEREFFECT)->Start_AttachPosition(this, vKineticPos, _float4(0.f, 1.f, 0.f, 0.f));

		_float3 vForce = { XMVectorGetX(m_vCamLook), XMVectorGetY(m_vCamLook), XMVectorGetZ(m_vCamLook) };
		vForce = XMVector3Normalize(vForce);
		vForce *= m_fThrowPower;
		static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Add_Physical(vForce);
		static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->SetThrow();

		m_SoundStore.PlaySound("fx_kinetic_shot", m_pTransformCom);
	}

	CPlayerInfoManager::GetInstance()->Change_PlayerKineticEnergy(CHANGE_DECREASE, 15);
}

void CPlayer::Event_KineticSlowAction()
{
	CGameInstance::GetInstance()->SetTimeRatioCurve("Kinetic_Combo_Slow");
}

void CPlayer::Event_Trail(_bool bTrail)
{
	if (CPlayerInfoManager::GetInstance()->Get_Copy())
	{
		for (auto& iter : m_vecWeapon)
		{
			static_cast<CScarletWeapon*>(iter)->Trail_Setting(bTrail);
		}
	}
	else
	{
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(bTrail);
	}

	if (CPlayerInfoManager::GetInstance()->Get_Copy())
	{
		for (auto& iter : m_vecWeapon)
		{
			static_cast<CScarletWeapon*>(iter)->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, bTrail);
		}
	}
	else
	{
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, bTrail);
	}
}

void CPlayer::Event_Dust()
{
	//CVFX_Manager::GetInstance()->GetParticle(PARTICLE::)
}

void CPlayer::Event_KineticCircleEffect()
{
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Kinetic_BaseCircle", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01");
	//_matrix MatScale = XMMatrixIdentity() * XMMatrixScaling(10.f, 10.f, 10.f);
	//CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, L"Player_Kinetic_Particle")->Start_AttachPivot(this, MatScale, "Reference", true, false);
	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, L"Player_Kinetic_Particle")->Start_Attach(this, "Reference");
}

void CPlayer::Event_KineticCircleEffect_Attach()
{
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Kinetic_BaseCircle", LAYER_PLAYEREFFECT)->Start_Attach(this, "Eff01", true);
}


void CPlayer::Reset_Charge()
{
	m_fBefCharge = 0.f;
	for (auto& iter : m_fCharge)
		iter = 0.f;
}

void CPlayer::Load_Effect(const char * pEffectDir)
{
	if (strstr(pEffectDir, "Default_Attack") != nullptr)
	{
		m_mapDefaultEffect.clear();

		CGameUtils::ListFiles(pEffectDir, [this](const string& pEffectPath)
		{
			Json jsonEffect = CJsonStorage::GetInstance()->FindOrLoadJson(pEffectPath);
			auto pEffectGroup = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectFolder", L"ProtoVFX_EffectGroup", &jsonEffect));

			m_mapDefaultEffect.emplace(pEffectGroup->GetObjectTag(), pEffectPath);
			pEffectGroup->SetDelete();
		});
	}

	else if (strstr(pEffectDir, "Fire_Attack") != nullptr)
	{
		m_mapFireEffect.clear();

		CGameUtils::ListFiles(pEffectDir, [this](const string& pEffectPath)
		{
			Json jsonEffect = CJsonStorage::GetInstance()->FindOrLoadJson(pEffectPath);
			auto pEffectGroup = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectFolder", L"ProtoVFX_EffectGroup", &jsonEffect));

			m_mapFireEffect.emplace(pEffectGroup->GetObjectTag(), pEffectPath);
			pEffectGroup->SetDelete();
		});
	}
}

_bool CPlayer::UseAttackCnt(_uint eType)
{
	switch (eType)
	{
	case LIMIT_NONCHARGE_FLOOR:
		if (0 < m_AttackLimit.m_iNonChargeAttack_Floor) { m_AttackLimit.m_iNonChargeAttack_Floor -= 1; return true; }
		break;
	case LIMIT_NONCHARGE_AIR:
		if (0 < m_AttackLimit.m_iNonChargeAttack_Air) { m_AttackLimit.m_iNonChargeAttack_Air -= 1; return true; }
		break;
	case LIMIT_AIRATK01:
		if (0 < m_AttackLimit.m_iAttack_Air01) { m_AttackLimit.m_iAttack_Air01 -= 1; return true; }
		break;
	case LIMIT_AIRATK02:
		if (0 < m_AttackLimit.m_iAttack_Air02) { m_AttackLimit.m_iAttack_Air02 -= 1; return true; }
		break;
	case LIMIT_AIRDODGEATK:
		if (0 < m_AttackLimit.m_iAttack_AirDodge) { m_AttackLimit.m_iAttack_AirDodge -= 1; return true; }
		break;
	default:
		break;
	}
	return false;
}

_bool CPlayer::UseMoveCnt(_uint eType)
{
	switch (eType)
	{
	case LIMIT_DOUBLEJUMP:
		if (0 < m_MoveLimit.m_iDoubleJump) { m_MoveLimit.m_iDoubleJump -= 1; return true; }
		break;
	case LIMIT_AIRDODGE:
		if (0 < m_MoveLimit.m_iAirDodge) { m_MoveLimit.m_iAirDodge -= 1; return true; }
		break;
	default:
		break;
	}
	return false;
}

void CPlayer::AttackLimitReset(void)
{
	m_AttackLimit.m_iNonChargeAttack_Floor = m_AttackLimit.MAX_iNonChargeAttack_Floor;
	m_AttackLimit.m_iNonChargeAttack_Air = m_AttackLimit.MAX_iNonChargeAttack_Air;
	m_AttackLimit.m_iAttack_Air01 = m_AttackLimit.MAX_iAttack_Air01;
	m_AttackLimit.m_iAttack_Air02 = m_AttackLimit.MAX_iAttack_Air02;
	m_AttackLimit.m_iAttack_AirDodge = m_AttackLimit.MAX_iAttack_AirDodge;
}

void CPlayer::MoveLimitReset(void)
{
	m_MoveLimit.m_iDoubleJump = m_MoveLimit.MAX_iDoubleJump;
	m_MoveLimit.m_iAirDodge = m_MoveLimit.MAX_iAirDodge;
}

void CPlayer::Jump()
{
	m_fYSpeed = m_fJumpPower;
}

void CPlayer::SmoothTurn_Attack(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPlayerPos.w = 1.f;

	Vector4 vCamLook = pGameInstance->Get_CamLook();
	vCamLook.y = 0.f;
	vCamLook.w = 0.f;
	vCamLook.Normalize();

	Vector4 vTarget = vPlayerPos + vCamLook;

	m_pTransformCom->LookAt_Smooth(vTarget, TimeDelta * 0.1f);
}

void CPlayer::BehaviorCheck(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_fPlayRatio = 0.f;
	m_bLeftClick = false;
	
	if (nullptr != m_pModel->GetPlayAnimation())
		m_fPlayRatio = m_pModel->GetPlayAnimation()->GetPlayRatio();

	m_bBreakFall = m_pController->KeyDown(CController::SHIFT);

	if (m_bHit || m_eDeBuff == EDeBuffType::DEBUFF_THUNDER)
	{
		m_bLeftClick = false;
		m_bDash = false;

		m_bNonCharge = false;
		m_bCharge = false;

		m_fBefCharge = 0.f;

		m_bJump = false;

		m_bKineticRB = false;
		m_bKineticG = false;
		m_bUpper = false;

		m_bBrainCrashInput = false;
		m_bDirveModeStart = false;
		m_bBrainFieldStart = false;
	}
	else
	{
		m_bLeftClick = m_pController->KeyDown(CController::MOUSE_LB);
		m_bDash = m_pController->KeyDown(CController::SHIFT);

		m_bNonCharge = m_pController->KeyUp(CController::C);
		m_bCharge = m_pController->KeyPress(CController::C);
		
		if (!m_bCharge) m_fBefCharge = 0.f;

		m_bJump = m_pController->KeyDown(CController::SPACE);
		m_bUpper = m_pController->KeyPress(CController::SPACE);

		m_bBrainCrashInput = m_pController->KeyDown(CController::R);

		m_bDirveModeStart = m_pController->KeyDown(CController::M);
		m_bBrainFieldStart = m_pController->KeyDown(CController::B);
		//if (m_bCanTurn_Attack)
		//{
		//	SmoothTurn_Attack(TimeDelta);
		//}

		m_bKineticRB = m_pController->KeyPress(CController::MOUSE_RB);
		m_bKineticG = m_pController->KeyPress(CController::G);

		if ((false == m_pASM->isSocketEmpty("Netual_Saperate_Animation")) &&
			(m_bDash || m_bJump || m_bLeftClick || m_bNonCharge || m_bCharge || m_bKineticRB))
		{
			m_pASM->ClearAnimSocket("Netual_Saperate_Animation");
		}

		if (m_pASM->GetCurStateName().find("WALK_START") != string::npos)
		{
			m_pASM->ClearAnimSocket("Netual_Saperate_Animation");
		}

		if (m_pASM->GetCurStateName().find("END") != string::npos)
		{
			m_pASM->ClearAnimSocket("Netual_Saperate_Animation");
		}

		//IM_LOG(m_pASM->GetCurAnimName().c_str());
	}

	//if (m_bLeftClick)
	//	m_fKineticCombo_Slash = 10.f;

	if (m_pASM->GetCurStateName().find("ATTACK_CHARGE_LOOP") == string::npos)
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pChargeParticle))
		{
			m_pChargeParticle->Delete_Particles();
			m_pChargeParticle = nullptr;
		}
	}

	// æ∆øÙ∂Û¿Œ √º≈©
	if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().bDriveMode || CPlayerInfoManager::GetInstance()->Get_PlayerStat().bBrainField || m_pKineticStataMachine->GetCurStateName() != "NO_USE_KINETIC")
	{
		SetOutLine(true);
	}
	else
	{
		SetOutLine(false);
	}

	m_vCamLook = pGameInstance->Get_CamLook();

	CPlayerInfoManager::GetInstance()->Set_Air(m_bAir);
}

void CPlayer::MoveStateCheck(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_bWalk = false;
	m_eMoveDir = DIR_END;

	if (m_bHit)
	{
		m_vMoveDir = Vector3::Zero;
		m_bCanTurn = false;
		m_bAir = false;
	}
	else
	{
		m_vMoveDir = m_pController->GetMoveAxis();

		Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vPlayerPos.w = 1.f;

		m_vLastDir = vPlayerPos - m_vBefPos;
		m_vLastDir.w = 0.f;
		m_vBefPos = vPlayerPos;

		if (0.f != XMVectorGetX(XMVector3Length(m_vMoveDir)))
		{
			m_bWalk = true;

			Vector3 vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vPlayerLook.y = 0.f;
			vPlayerLook.Normalize();

			Vector3 vPlayerRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vPlayerRight.y = 0.f;
			vPlayerRight.Normalize();

			Matrix matCam = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
			_vector vScale, vRotate, vTrans;
			XMMatrixDecompose(&vScale, &vRotate, &vTrans, matCam);
			Matrix matCamRot = XMMatrixRotationQuaternion(vRotate);
			m_vMoveDir = XMVector3TransformNormal(m_vMoveDir, matCamRot);
			m_vMoveDir.y = 0.f;
			m_vMoveDir.Normalize();

			_float fAxis_Look = vPlayerLook.Dot(m_vMoveDir);
			_float fAxis_Right = vPlayerRight.Dot(m_vMoveDir);

			if (fabs(fAxis_Look) > fabs(fAxis_Right))
			{
				if (0.f < fAxis_Look) { m_eMoveDir = DIR_F; }
				else { m_eMoveDir = DIR_B; }
			}
			else
			{
				if (0.f < fAxis_Right) { m_eMoveDir = DIR_R; }
				else { m_eMoveDir = DIR_L; }
			}

			if (m_bCanTurn)
				m_pTransformCom->LookAt_Smooth(vPlayerPos + m_vMoveDir, TimeDelta);

			if (m_bAir || m_bHit)	// Í≥µÏ§ë ?ÅÌÉú?êÏÑ† ?åÏ†Ñ??Î®πÏ? ?äÏùå
				m_vMoveDir = vPlayerLook;
		}
	}
}

void CPlayer::SeperateCheck()
{
	m_bSeperateAnim = true;

	if (m_pModel->GetPlayAnimation() != nullptr)
	{
		m_bSeperateAnim = (m_pModel->GetPlayAnimation()->GetName() != "AS_ch0100_002_AL_wait02") &&
			(m_bKineticMove || 
			(!m_pASM->isSocketEmpty("Upper_Saperate_Animation")) || 
				(!m_pASM->isSocketEmpty("Netual_Saperate_Animation")));
	}
}

void CPlayer::HitCheck()
{
	if (m_bHit)		// ÎßûÏúºÎ©??ÑÎ¨¥Í≤ÉÎèÑ Î™ªÌïòÍ≤??¥Ïïº?úÎã§
	{
		m_bMove = false;
		m_bWalk = false;
		m_bLeftClick = false;
		m_bDash = false;
		m_bJump = false;
		m_bSeperateAnim = false;
		m_bKineticRB = false;
		m_bKineticG = false;
		m_bKineticMove = false;
		m_bNonCharge = false;
		m_bCharge = false;
		m_bCanTurn = false;
		m_bCanMove = false;
		m_bCanRun = false;
		m_bCanTurn_Attack = false;
		m_bKineticCombo = false;
		Event_Trail(false);
		Event_collisionEnd();
		m_bTeleport = false;
		Event_FinishFovActionCam();
	}

	if (!m_pASM->isSocketEmpty("Hit_AnimSocket"))
	{
		string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
		_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		m_pTransformCom->LocalMove(vLocal);
	}
}

void CPlayer::SocketLocalMoveCheck()
{
	if (!m_pASM->isSocketEmpty("Common_AnimSocket"))
	{
		string szCurAnimName = m_pASM->GetSocketAnimation("Common_AnimSocket")->GetName();
		_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		m_pTransformCom->LocalMove(vLocal);
	}

	// ???òÎÇò?¥Í±∞
	else if (!m_pASM->isSocketEmpty("Kinetic_Special_AnimSocket"))
	{
		string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetName();
		_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		//IM_LOG("%f %f %f", XMVectorGetX(vLocal), XMVectorGetY(vLocal), XMVectorGetZ(vLocal));
		m_pTransformCom->LocalMove(vLocal);
	}

	else if (!m_pASM->isSocketEmpty("SAS_Special_AnimSocket"))
	{
		string szCurAnimName = m_pASM->GetSocketAnimation("SAS_Special_AnimSocket")->GetName();
		_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		m_pTransformCom->LocalMove(vLocal);

		_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		m_pTransformCom->LocalMove(vOpTest);
	}

	else if (!m_pASM->isSocketEmpty("BrainField_AnimSocket"))
	{
		string szCurAnimName = m_pASM->GetSocketAnimation("BrainField_AnimSocket")->GetName();
		_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		m_pTransformCom->LocalMove(vLocal);
	}

	//Kinetic_AnimSocket
}

void CPlayer::SyncEffectLocalMove(const string& szSocketName)
{
	if (szSocketName == "")
	{
		_vector vEffLocal = m_pModel->Sync_EffectLocalMove();

		_vector vScale, vRotation, vTrans;
		XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_pTransformCom->Get_WorldMatrix());
		_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

		XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_pModel->GetPivotMatrix());
		_matrix PivotRotation = XMMatrixRotationQuaternion(vRotation);
		vEffLocal = XMVector3TransformNormal(vEffLocal, PivotRotation);

		//_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
		//vEffLocal = XMVector3TransformNormal(vEffLocal, ModifyRotation);

		vEffLocal = XMVector3TransformNormal(vEffLocal, WorldRotation);

		m_vSyncEffectLocal = vEffLocal;
	}
	else
	{
		if (nullptr != m_pASM->GetSocketAnimation(szSocketName))
		{
			_vector vEffLocal = m_pASM->GetSocketAnimation(szSocketName)->GetEffectLocalMove();

			_vector vScale, vRotation, vTrans;
			XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_pTransformCom->Get_WorldMatrix());
			_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

			XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_pModel->GetPivotMatrix());
			_matrix PivotRotation = XMMatrixRotationQuaternion(vRotation);
			vEffLocal = XMVector3TransformNormal(vEffLocal, PivotRotation);

			//_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
			//vEffLocal = XMVector3TransformNormal(vEffLocal, ModifyRotation);

			vEffLocal = XMVector3TransformNormal(vEffLocal, WorldRotation);

			m_vSyncEffectLocal = vEffLocal;
		}
	}
}

void CPlayer::Update_NotiveNeon()
{	
	// Ï≤¥Î†•??10% ?¥Ìïò?ºÎïå ?ùÏÑ±
	static _bool bCreate = false;
	if (false == bCreate && 0.1f > _float(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP) / CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP)
	{
		bCreate = true;

		Json json;
		json["NoticeNeon"] = "NoticeNeon_HP";
		m_pNeonUI = dynamic_cast<CNoticeNeonUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_NoticeNeonUI"), &json));
		//m_pNeonUI->SetOwner(this);
	}

	// ??†ú
	if (nullptr != m_pNeonUI)
	{
		if (true == m_pNeonUI->doKill())
		{
			m_pNeonUI->SetDelete();
		}
	}

	// Î¶¨ÏÖã
	if (0.1f < _float(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP) / CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP)
	{
		if (nullptr != m_pNeonUI)
		{
			m_pNeonUI->SetDelete();
		}
		bCreate = false;
	}
}

void CPlayer::Update_TargetUI()
{
	CEnemy* pTarget = dynamic_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster());

	if (pTarget != nullptr && pTarget->Exclude() == true)
		pTarget = nullptr;

	if (m_pSettedTarget != pTarget)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		//ø¯∑° ≈∏∞Ÿ¿Ã æ¯¥Ÿ∞° ª˝±‰ ∞ÊøÏ
		if (m_pSettedTarget == nullptr && pTarget != nullptr)
		{
			m_pUI_LockOn = dynamic_cast<CMonsterLockonUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterLockon")));
			assert(m_pUI_LockOn != nullptr);
			m_pUI_LockOn->Set_Owner(pTarget);
			m_pUI_LockOn->Set_UIPivotMatrix(pTarget->GetBoneMatrix("Target"));

		}

		//ø¯∑° ≈∏∞Ÿ¿Ã ¿÷æ˙¥¬µ• ªÁ∂Û¡¯ ∞ÊøÏ
		else if (m_pSettedTarget != nullptr && pTarget == nullptr)
		{
			m_pUI_LockOn->SetDelete();
		}

		else if (m_pSettedTarget != nullptr && pTarget != nullptr)
		{
			m_pUI_LockOn->SetDelete();

			m_pUI_LockOn = dynamic_cast<CMonsterLockonUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterLockon")));
			assert(m_pUI_LockOn != nullptr);
			m_pUI_LockOn->Set_Owner(pTarget);
			m_pUI_LockOn->Set_UIPivotMatrix(pTarget->GetBoneMatrix("Target"));
		}

		//info bar º≥¡§
		if (pTarget != nullptr)
			pTarget->GetEnemyUI()->Create_UIInfo();

		//Create_TargetInfoBar(pTarget);

		m_pSettedTarget = pTarget;
	}

}

//void CPlayer::Create_TargetInfoBar(CGameObject* pTarget)
//{
//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
//
//	CEnemy* pEnemy = dynamic_cast<CEnemy*>(pTarget);
//	if (pEnemy == nullptr) return;
//	
//	if (pEnemy->GetHasName() == true)
//		return;
//
//	if (pEnemy->HasCrushGauge() == true)
//	{
//		CMonsterShildUI* pUI_Shild = nullptr;
//		pUI_Shild = dynamic_cast<CMonsterShildUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterShield")));
//
//		assert(pUI_Shild != nullptr);
//		pUI_Shild->Set_Owner(pTarget);
//
//		_float4x4 PivotMatrix = pEnemy->Get_UIPivotMatrix(ENEMY_INFOBAR);
//		pUI_Shild->SetPivotMatrix(PivotMatrix);
//
//		_int iLevel = pEnemy->Get_EnemyLevel();
//		_int iName = pEnemy->Get_EnemyName();
//		pUI_Shild->Set_MonsterInfo(iLevel, iName);
//	}
//	else
//	{
//		CMonsterHpUI* pUI_HP = nullptr;
//		pUI_HP = dynamic_cast<CMonsterHpUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterHP")));
//
//		assert(pUI_HP != nullptr);
//		pUI_HP->Set_Owner(pTarget);
//
//		_float4x4 PivotMatrix = pEnemy->Get_UIPivotMatrix(ENEMY_INFOBAR);
//		pUI_HP->SetPivotMatrix(PivotMatrix);
//
//		_int iLevel = pEnemy->Get_EnemyLevel();
//		_int iName = pEnemy->Get_EnemyName();
//		pUI_HP->Set_MonsterInfo(iLevel, iName);
//	}
//	
//	pEnemy->Set_HasName();
//}

void CPlayer::NetualChecker(_double TimeDelta)
{
	if ((false == isPlayerAttack()) && m_bOnBattle && m_pASM->isSocketExactlyEmpty())
	{
		m_fNetualTimer += TimeDelta;
	}
	else
	{
		m_fNetualTimer = 0.f;
	}

	if (m_bOnBattle)	// ?ÑÌà¨ Ï§??åÌã∞???ùÏÑ±
	{
		m_fBattleParticleTime += (_float)TimeDelta;

		if (1.f < m_fBattleParticleTime)
		{
			m_fBattleParticleTime = 0.f;
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Text_Particle_NoLoop"), LAYER_PLAYEREFFECT)
				->Start_Attach(this, "BackHair");
		}
	}

	if (m_bOnBattle && (m_fNetualTimer >= 10.f))
	{
		m_fBattleParticleTime = 0.f;
		m_bOnBattle = false;
		m_pASM->InputAnimSocket("Netual_Saperate_Animation", m_TransNeutralSocket);
	}
}

_bool CPlayer::Detector(_bool bComplusion)
{
	// ≈Ω¡ˆ±‚ ≥ª∫Œø° «—π¯ µÈæÓø¬ ∏ÛΩ∫≈Õ∞° ∞Ëº” ¡∏¿Á«œ¥¬¡ˆ ∆«¥‹
	// bComplusion¿Ã true¿Ã∏È ≥ª∫Œ ƒ µµ¡ﬂø°µµ µø¿€«œ∞‘ «‘

	if (false == m_DetectTimer.Use() && false == bComplusion) return false;

	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	for (auto& iter = m_DetectList.begin(); iter != m_DetectList.end();)
	{
		if (false == CGameInstance::GetInstance()->Check_ObjectAlive(*iter))
		{
			iter = m_DetectList.erase(iter);
			continue;
		}

		if ((*iter)->IsDead())
		{
			iter = m_DetectList.erase(iter);
			continue;
		}

		if ((*iter)->IsDeleted())
		{
			iter = m_DetectList.erase(iter);
			continue;
		}

		_float4 vEnemyPos = (*iter)->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		_float fDistance = (vEnemyPos - vMyPos).Length();

		if (25.f < fDistance)
		{
			iter = m_DetectList.erase(iter);
			continue;
		}
		else
		{
			return true;
		}
	}

	return false;
}

void CPlayer::LookAtDir(Vector3 Vector)
{
	m_pTransformCom->LookAt(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(Vector.x, 0.f, Vector.z, 0.f));
}

void CPlayer::LookTarget()
{
	//while (m_pTargetedEnemy != nullptr && static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
	//{
	//	Enemy_Targeting(true);
	//}

	if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
	{
		Enemy_Targeting(true);
	}

	if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
	{
		_vector vTargetPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		m_pTransformCom->LookAt_NonY(vTargetPos);
	}
}

HRESULT CPlayer::Setup_Parts()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

//	Json Weapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/wp0190.json");
//	Weapon["Model"] = "../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model";
//
//	CGameObject*	pGameObject = nullptr;
//
//	WEAPON_DESC		Desc;
//	ZeroMemory(&Desc, sizeof(WEAPON_DESC));
//	Desc.m_pJson = &Weapon;
//
////	pGameInstance->Clone_GameObject(TEXT("Layer_Player"), TEXT("PlayerWeapon"), &Desc);
//
//	pGameObject = pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("PlayerWeapon"), &Desc);
//	m_vecWeapon.push_back(pGameObject);

	Json Weapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/wp0190.json");

	WEAPON_DESC		Desc;
	ZeroMemory(&Desc, sizeof(WEAPON_DESC));
	Desc.m_pJson = &Weapon;

	CGameObject* pGameObject = nullptr;
	pGameObject = pGameInstance->Clone_GameObject_Get(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("PlayerWeapon"), &Desc);
	static_cast<CWeapon_Player*>(pGameObject)->Set_Coltype(ECOPYCOLTYPE::COPYCOL_MAIN);
	m_vecWeapon.push_back(pGameObject);

	pGameObject = pGameInstance->Clone_GameObject_Get(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("PlayerWeapon"), &Desc);
	static_cast<CWeapon_Player*>(pGameObject)->Set_Coltype(ECOPYCOLTYPE::COPYCOL_LEFT);
	m_vecWeapon.push_back(pGameObject);

	pGameObject = pGameInstance->Clone_GameObject_Get(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("PlayerWeapon"), &Desc);
	static_cast<CWeapon_Player*>(pGameObject)->Set_Coltype(ECOPYCOLTYPE::COPYCOL_RIGHT);
	m_vecWeapon.push_back(pGameObject);

	CSheath_Player* pSheath = nullptr;

	pSheath = static_cast<CSheath_Player*>(pGameInstance->Clone_GameObject_Get(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("PlayerSheath"), &Desc));
	m_vecSheath.push_back(pSheath);

	pSheath = static_cast<CSheath_Player*>(pGameInstance->Clone_GameObject_Get(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("PlayerSheath"), &Desc));
	m_vecSheath.push_back(pSheath);

	pSheath = static_cast<CSheath_Player*>(pGameInstance->Clone_GameObject_Get(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("PlayerSheath"), &Desc));
	m_vecSheath.push_back(pSheath);

	return S_OK;
}

void CPlayer::Check_Parts()
{
	_uint iWeaponType = CPlayerInfoManager::GetInstance()->Get_PlayerWeapon();
	_uint iCurWeaponType = static_cast<CWeapon_Player*>(m_vecWeapon.front())->Get_WeaponType();

	if (iWeaponType != iCurWeaponType)
	{
		for (auto& iter : m_vecWeapon)
		{
			static_cast<CWeapon_Player*>(iter)->Change_Weapon(static_cast<WEAPONTYPE>(iWeaponType));
		}
		for (auto& iter : m_vecSheath)
		{
			static_cast<CSheath_Player*>(iter)->Change_Sheath(static_cast<WEAPONTYPE>(iWeaponType));
		}
	}
}

void CPlayer::Search_Usable_KineticObject()
{
	//CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	//m_pKineticObject = nullptr;
	//m_pTargetedEnemy = nullptr;

	//if (pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject") == nullptr
	//	|| pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects().empty())
	//{
	//	m_pKineticObject = nullptr;
	//}
	//else
	//{
	//	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
	//	{
	//		if (false == static_cast<CMapKinetic_Object*>(iter)->Usable())
	//			continue;

	//		//static _float fDistance = 10.f;

	//	//	_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	//	//	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//	//	_float fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

	//	//	if (fDistance > fCurDistance)
	//	//	{
	//			m_pKineticObject = iter;
	//	//	}
	//	}
	//}

	//if (pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster") == nullptr ||
	//	pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects().empty())
	//{
	//	m_pTargetedEnemy = nullptr;
	//}
	//else
	//{
	//	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
	//	{
	//		static _float fDistance = 1000.f;

	//		_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	//		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//		_float fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

	//		if (fDistance > fCurDistance)
	//		{
	//			m_pTargetedEnemy = iter;
	//		}
	//	}
	//}
}

void CPlayer::Enemy_Targeting(_bool bNear)
{
	// true?¥Î©¥ Î¨¥Ï°∞Í±?Í∞ÄÍπåÏù¥ ?àÎäî ?ÄÍ≤?
	// false?¥Î©¥ ?§Ïùå ?ÄÍ≤?ÏßÄ??

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	static _uint iTargetCount = 0;
	CGameObject* pBeforeEnemy = CPlayerInfoManager::GetInstance()->Get_TargetedMonster();

	CPlayerInfoManager::GetInstance()->Set_TargetedMonster(nullptr);

	if (pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster") == nullptr ||
		pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects().empty())
	{
		return;
	}
	else
	{
		_float fDistance = 20.f;
		list<pair<CGameObject*, _float>>	DistanceList;
		for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
		{
			//if ((!static_cast<CEnemy*>(iter)->IsDead()))
			if ((!static_cast<CEnemy*>(iter)->IsDead()) && (!static_cast<CEnemy*>(iter)->Exclude()))
			{
				_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float	fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

				if (fDistance > fCurDistance)
				{
					DistanceList.push_back({ iter, fCurDistance });
				}
			}
		}

		DistanceList.sort([&](pair<CGameObject*, _float> pSour, pair<CGameObject*, _float> pDest)->_bool
		{
			return pSour.second < pDest.second;
		});

		if (bNear)
		{
			if (DistanceList.empty())
				return;

			CPlayerInfoManager::GetInstance()->Set_TargetedMonster(DistanceList.front().first);
		}
		else if (!bNear)
		{
			if (DistanceList.empty())
				return;

			if (nullptr == pBeforeEnemy)
			{
				CPlayerInfoManager::GetInstance()->Set_TargetedMonster(DistanceList.front().first);
			}
			else
			{
				_bool bChange = false;
				for (auto& iter : DistanceList)
				{
					if (bChange)
					{
						CPlayerInfoManager::GetInstance()->Set_TargetedMonster(iter.first);
						return;
					}
					else
					{
						if (iter.first == pBeforeEnemy)
						{
							bChange = true;
						}
						if (iter == DistanceList.back())
						{
							CPlayerInfoManager::GetInstance()->Set_TargetedMonster(DistanceList.front().first);
							return;
						}
					}
				}
			}
		}

	}
}

void CPlayer::KineticObject_Targeting()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CPlayerInfoManager::GetInstance()->Set_KineticObject(nullptr);

	_uint iCount = 0;
	_uint iTest = 0;

	if (pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject") == nullptr
		|| pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects().empty())
	{
		return;
	}
	else
	{
		_float fDistance = 20.f;

		for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
		{
			if (false == static_cast<CMapKinetic_Object*>(iter)->GetThrow() && true == static_cast<CMapKinetic_Object*>(iter)->Usable())
			{
				_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

				if (fDistance > fCurDistance)
				{
					fDistance = fCurDistance;
					CPlayerInfoManager::GetInstance()->Set_KineticObject(iter);
				}
			}
		}
	}
}


void CPlayer::KineticObject_OutLineCheck()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
	{

		if (nullptr == CPlayerInfoManager::GetInstance()->Get_KineticObject())
		{
			static_cast<CMapKinetic_Object*>(iter)->SetOutline(false);
		}
		else
		{
			if (iter == CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				static_cast<CMapKinetic_Object*>(iter)->SetOutline(true);
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_CameRange(true);

			}
			else
			{
				static_cast<CMapKinetic_Object*>(iter)->SetOutline(false);
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_CameRange(false);

			}
		}
	}
}

void CPlayer::SpecialObject_Targeting()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CPlayerInfoManager::GetInstance()->Set_SpecialObject(nullptr);

	if (pGameInstance->GetLayer(LEVEL_NOW, LAYER_KINETIC) == nullptr
		|| pGameInstance->GetLayer(LEVEL_NOW, LAYER_KINETIC)->GetGameObjects().empty())
	{
		return;
	}
	else
	{
		_float fDistance = 30.f;

		for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, LAYER_KINETIC)->GetGameObjects())
		{
			_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			_float fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

			if (fDistance > fCurDistance)
			{
				fDistance = fCurDistance;
				CPlayerInfoManager::GetInstance()->Set_SpecialObject(iter);
			}
		}
	}
}

void CPlayer::SpecialObject_OutLineCheck()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, LAYER_KINETIC)->GetGameObjects())
	{
		if (nullptr == CPlayerInfoManager::GetInstance()->Get_SpecialObject())
		{
			static_cast<CSpecialObject*>(iter)->SetOutline(false);
		}
		else
		{
			if (iter == CPlayerInfoManager::GetInstance()->Get_SpecialObject())
			{
				static_cast<CSpecialObject*>(iter)->SetOutline(true);
				//static_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_CameRange(true);
			}
			else
			{
				static_cast<CSpecialObject*>(iter)->SetOutline(false);
				//static_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_CameRange(false);
			}
		}
	}
}

void CPlayer::Spline_Kinetic(_double TimeDelta)
{
	//static _double fLerpTime = 0.f;
	//static _vector vSplinePoint_01, vSplinePoint_02, vSplinePoint_03, vSplinePoint_04;
	//static _vector vBeforePos;
	//
	//if (nullptr == m_pTargetedEnemy) { fLerpTime = 0.f; return; }
	//if (nullptr == m_pKineticObject) { fLerpTime = 0.f; return; }

	//if (0.f == fLerpTime)
	//{
	//	vSplinePoint_01 = XMLoadFloat4(&m_vSplinePoint_01);
	//	vSplinePoint_02 = XMLoadFloat4(&m_vSplinePoint_02);
	//	vSplinePoint_03 = XMLoadFloat4(&m_vSplinePoint_03);
	//	vSplinePoint_04 = XMLoadFloat4(&m_vSplinePoint_04);
	//}

	//if (2.f >= fLerpTime)
	//{
	//	_vector vCurPos = XMVectorCatmullRom(vSplinePoint_01, vSplinePoint_02, vSplinePoint_03, vSplinePoint_04, fLerpTime * 0.5f);
	//	_vector vDir = vCurPos - vBeforePos;
	//	_float3 vVelocity = _float3(XMVectorGetX(vDir) * 10.f, XMVectorGetY(vDir) * 10.f, XMVectorGetZ(vDir) * 10.f);
	//	m_pKineticObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vCurPos);
	//	vBeforePos = vCurPos;

	//	fLerpTime += TimeDelta;
	//}

	//if (2.f < fLerpTime)
	//{
	//	fLerpTime = 0.f;
	//	vSplinePoint_01 = XMLoadFloat4(&m_vSplinePoint_01);
	//	vSplinePoint_02 = XMLoadFloat4(&m_vSplinePoint_02);
	//	vSplinePoint_03 = XMLoadFloat4(&m_vSplinePoint_03);
	//	vSplinePoint_04 = XMLoadFloat4(&m_vSplinePoint_04);
	//}
}

void CPlayer::Kinetic_Test(_float fRatio)
{
	//if (0.f == m_fSwingLerpTimer)
	//	m_vKineticInitLook = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_LOOK);

	//m_fSwingLerpTimer += g_fTimeDelta * 0.5f;

	//if (1.f > m_fSwingLerpTimer)	// Îπ??åÎ©¥???òÌïú?åÍπåÏßÄ ?åÏñ¥?§Îäî Í≥ºÏ†ï, 1Ï¥??àÏóê ?¥Î§ÑÏß?
	//{
	//	static _vector vBeforePos;

	//	_float fScalar = XMVectorGetX(XMVector3Length(m_vToKineticObj)) - XMVectorGetX(XMVector3Length(m_vKineticPoint));
	//	fScalar *= (1.f - m_fSwingLerpTimer);

	//	_float fAngle = XMConvertToRadians(180.f);

	//	_vector vVector = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	//	// Ï¢åÏö∞ Ï≤¥ÌÅ¨
	//	_vector vMyright = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	//	if (0.f > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyright), XMVector3Normalize(m_vToKineticObj))))
	//	{
	//		// ?ºÏ™Ω
	//		vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
	//		vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(-(fAngle * m_fSwingLerpTimer)));
	//		m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
	//	}
	//	else
	//	{
	//		// ?§Î•∏Ï™?
	//		vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
	//		vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(fAngle * m_fSwingLerpTimer));
	//		m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
	//	}

	//	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//	m_pKineticObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vMyPos + vVector);
	////	m_pKineticObject->GetTransform()->LookAt_Lerp(m_vKineticInitLook, vMyPos, m_fSwingLerpTimer);
	////	m_pKineticObject->GetTransform()->LookAt_Lerp_Test(vMyPos, m_fSwingLerpTimer);

	//	if(!XMVector3Equal((vMyPos + vVector), vBeforePos))
	//		m_vKineticOrbit = (vMyPos + vVector) - vBeforePos;

	//	_vector vKineticRight = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_RIGHT);
	//	if (0.f < XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_vKineticOrbit), XMVector3Normalize(vKineticRight))))
	//		m_bRight = true;
	//	else
	//		m_bRight = false;

	//	vBeforePos = vMyPos + vVector;
	//}
	//else // ?òÌïú?åÍπåÏßÄ ?§Í≥† ?òÏÑú Î∂Ä?∞Ïùò ÎπôÎπô ?åÏ†ÑÍ≥??ÅÏúºÎ°??†ÏïÑÍ∞?
	//{
	//	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//	_vector vMonsterPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	//	_vector vKineticPos = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	//	_vector vKineticToMonster = vMonsterPos - vKineticPos;
	//	_vector vKineticRight = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_RIGHT);

	//	_vector vOrbitLook = vMyPos - vKineticPos;
	//	_vector vMove = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vOrbitLook));

	////	m_pKineticObject->GetTransform()->LookAt_Lerp(m_vKineticInitLook, vMyPos, m_fSwingLerpTimer);
	////	m_pKineticObject->GetTransform()->LookAt_Lerp_Test(vMyPos, m_fSwingLerpTimer);

	//	_float fOrbitSpeed = XMVectorGetX(XMVector3Length(m_vToKineticObj));

	//	if (m_bRight)
	//	{
	//		if (0.9f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKineticToMonster), XMVector3Normalize(-vMove))))
	//		{
	//			//m_vThrow = XMVector3Normalize(vKineticToMonster);
	//			//m_vThrow = vKineticToMonster;
	//			//m_bSwingKineticThrow = true;
	//			//m_fSwingLerpTimer = 0.f;
	//		}
	//		else
	//		{
	//			// Î∞îÎùºÎ≥¥Î©∞ ?∞Ï∏°?ºÎ°ú ?¥Îèô
	//			//m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, vKineticRight);
	//			m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
	//			m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, -vMove);
	//		}
	//	}
	//	else
	//	{
	//		if (0.9f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKineticToMonster), XMVector3Normalize(vMove))))
	//		{
	//			//m_vThrow = XMVector3Normalize(vKineticToMonster);
	//			//m_vThrow = vKineticToMonster;
	//			//m_bSwingKineticThrow = true;
	//			//m_fSwingLerpTimer = 0.f;
	//		}
	//		else
	//		{
	//			// Î∞îÎùºÎ≥¥Î©∞ Ï¢åÏ∏°?ºÎ°ú ?¥Îèô
	//			//m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, -vKineticRight);
	//			m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
	//			m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, vMove);
	//		}
	//	}
	//}
}

void CPlayer::Kinetic_ByTurn()
{
	//if (nullptr == m_pTargetedEnemy) { m_fSwingLerpTimer = 0.f; return; }
	//if (nullptr == m_pKineticObject) { m_fSwingLerpTimer = 0.f; return; }

	//m_fSwingLerpTimer += g_fTimeDelta;

	//if (1.f > m_fSwingLerpTimer)
	//{
	//	static _vector vBeforePos;

	//	_float fScalar = XMVectorGetX(XMVector3Length(m_vToKineticObj)) - XMVectorGetX(XMVector3Length(m_vKineticPoint));
	//	fScalar *= (1.f - m_fSwingLerpTimer);

	//	_float fAngle = XMConvertToRadians(180.f);

	//	_vector vVector = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	//	// Ï¢åÏö∞ Ï≤¥ÌÅ¨
	//	_vector vMyright = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	//	if (0.f > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyright), XMVector3Normalize(m_vToKineticObj))))
	//	{
	//		// ?ºÏ™Ω
	//		vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
	//		vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(-(fAngle * m_fSwingLerpTimer)));
	//		m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
	//		m_bRight = true;	// ?åÎ†à?¥Ïñ¥ Î∞îÎùºÎ≥¥Îäî Ï∂?Í∏∞Ï? ?§Î•∏Ï™?Ï∂ïÏúºÎ°??ÑÎäîÍ∞Ä?
	//	}
	//	else
	//	{
	//		// ?§Î•∏Ï™?
	//		vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
	//		vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(fAngle * m_fSwingLerpTimer));
	//		m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
	//		m_bRight = false;
	//	}

	//	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//	m_pKineticObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vMyPos + vVector);

	//}
	//else // ?òÌïú?åÍπåÏßÄ ?§Í≥† ?òÏÑú Î∂Ä?∞Ïùò ÎπôÎπô ?åÏ†ÑÍ≥??ÅÏúºÎ°??†ÏïÑÍ∞?
	//{
	//	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//	_vector vMonsterPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	//	_vector vKineticPos = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	//	_vector vKineticToMonster = vMonsterPos - vKineticPos;

	//	_vector vOrbitLook = vMyPos - vKineticPos;
	//	_vector vMove = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vOrbitLook));

	//	_float fOrbitSpeed = XMVectorGetX(XMVector3Length(m_vToKineticObj));

	//	if (m_bRight)
	//	{
	//		if (0.f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKineticToMonster), XMVector3Normalize(vMove))))
	//		{
	//			m_vThrow = vKineticToMonster;
	//			m_bSwingKineticThrow = true;
	//			m_fSwingLerpTimer = 0.f;
	//			m_fKineticCombo_Kinetic = 10.f;
	//		}
	//		else
	//		{
	//			m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
	//			m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, vMove);
	//		}
	//	}
	//	else
	//	{
	//		if (0.f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKineticToMonster), XMVector3Normalize(-vMove))))
	//		{
	//			m_vThrow = vKineticToMonster;
	//			m_bSwingKineticThrow = true;
	//			m_fSwingLerpTimer = 0.f;
	//			m_fKineticCombo_Kinetic = 10.f;
	//		}
	//		else
	//		{
	//			m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
	//			m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, -vMove);
	//		}
	//	}
	//}
}

void CPlayer::Kinetic_Combo_KineticAnimation()
{
	if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster() &&
		nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject() &&
		!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
	{
		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio(m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayTime());
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Set_WorldMatrix(SocketMatrix);
	}
}

void CPlayer::Kinetic_Combo_MoveToKineticPoint()
{
	// Î™¨Ïä§?∞Î? Î∞îÎùºÎ≥¥Îäî ?ëÏóÖ?Ä ?¨Í∏∞?êÏÑú ?ºÏñ¥?òÎ©∞, ?§Ï†ú ?¥Îèô?¥Ïïº ?òÎäî ?ÑÏπò???ÑÏπòÎ≤°ÌÑ∞Î•??ùÏÑ±?úÎã§.

	if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster() && 
		nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones(0.f, EAnimUpdateType::NORMAL);
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();

		_vector vDiff = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - SocketMatrix.r[3];	// ?åÏ†Ñ?òÍ∏∞ ??Î¨ºÏ≤¥ Î≥∏Í≥º ?§Ï†ú Î¨ºÏ≤¥ ?¨Ïù¥??Î∞©Ìñ•Î≤°ÌÑ∞
		_vector vRePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + vDiff; // Î¨ºÏ≤¥ ?¨Ïù¥??Í±∞Î¶¨ÎßåÌÅº ?¥Îèô???ÑÏπò
		_vector vTmpLookAtMonster = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - vRePos; // ?àÎπÑ ?¥Îèô ?ÑÏπò?êÏÑú Î™¨Ïä§?∞Î? Î∞îÎùºÎ≥?Î∞©Ìñ•Î≤°ÌÑ∞

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector vLookPoint = vMyPos + vTmpLookAtMonster;
		vLookPoint = XMVector3Normalize(vLookPoint);
		vLookPoint = XMVectorSetW(vLookPoint, 1.f);
//		m_pTransformCom->LookAt_NonY(vLookPoint);	// ?àÎπÑ LookÎ≤°ÌÑ∞Î°?Î∞îÎùºÎ≥¥Îäî Î∞©Ìñ•??Î∞îÍøà

		_matrix ModifiedMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();
		_vector vRenewalDiff = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - ModifiedMatrix.r[3];
		_vector vRenewalRePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + vRenewalDiff;	// ?¥Îèô?¥Ïïº ?òÎäî ÏµúÏ¢Ö ?ÑÏπò

		m_vKineticComboRefPoint = vRenewalRePos;
	}
}

void CPlayer::Kinetic_Combo_AttachLerpObject()
{
	if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster() &&
		nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject() &&
		!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
	{
		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio(0.f);
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();

		_float fScale_Right = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[0]));
		_float fScale_Up = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[1]));
		_float fScale_Look = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[2]));

		_float fRatio = m_fKineticCharge;

		if (1.f < fRatio)
		{
			fRatio = 1.f;
		}

		_vector vLerpLook = XMVectorLerp(m_KineticObjectOrigionPos.r[2], SocketMatrix.r[2], fRatio);
		_vector vLerpRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook);
		_vector vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);

		_vector vLerpPos = XMVectorLerp(m_KineticObjectOrigionPos.r[3], SocketMatrix.r[3], fRatio);

		vLerpRight = XMVector3Normalize(vLerpRight) * fScale_Right;
		vLerpUp = XMVector3Normalize(vLerpUp) * fScale_Up;
		vLerpLook = XMVector3Normalize(vLerpLook) * fScale_Look;

		_matrix LerpMatrix = { vLerpRight, vLerpUp, vLerpLook, vLerpPos };

		CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Set_WorldMatrix(LerpMatrix);
	}
}

void CPlayer::BrainField_ObjectAnimation(_float fLook, _float fUp)
{
	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		_float fPlayTime = m_pModel->GetPlayAnimation()->GetPlayTime();

		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio(fPlayTime);
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix WaistMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist");
		_vector vLook = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fLook;
		_vector vUp = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_UP)) * fUp;

		_matrix	SocketMatrix = WaistMatrix * m_pTransformCom->Get_WorldMatrix();
		SocketMatrix.r[3] += vLook + vUp;

		_matrix KineticObjectPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();

		_float fScale_Right = XMVectorGetX(XMVector3Length(KineticObjectPos.r[0]));
		_float fScale_Up = XMVectorGetX(XMVector3Length(KineticObjectPos.r[1]));
		_float fScale_Look = XMVectorGetX(XMVector3Length(KineticObjectPos.r[2]));

		_float fRatio = m_pModel->GetPlayAnimation()->GetPlayRatio() * 10.f;

		if (1.f < fRatio)
		{
			fRatio = 1.f;
		}

		_vector vLerpLook = XMVectorLerp(KineticObjectPos.r[2], SocketMatrix.r[2], fRatio);
		_vector vLerpRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook);
		_vector vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);
		_vector vLerpPos = XMVectorLerp(KineticObjectPos.r[3], SocketMatrix.r[3], fRatio);

		vLerpRight = XMVector3Normalize(vLerpRight) * fScale_Right;
		vLerpUp = XMVector3Normalize(vLerpUp) * fScale_Up;
		vLerpLook = XMVector3Normalize(vLerpLook) * fScale_Look;

		_matrix LerpMatrix = { vLerpRight, vLerpUp, vLerpLook, vLerpPos };

		CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Set_WorldMatrix(LerpMatrix);
	}
}

void CPlayer::BrainField_ObjectAirSwing(_float fLook, _float fUp)
{
	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		_float fPlayTime = m_pModel->GetPlayAnimation()->GetPlayTime();

		if (m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_621_AL_BF_swing_air_start")
		{
			fPlayTime = fPlayTime;// / fDurationSum;
		}
		else if (m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_621_AL_BF_swing_air_loop")
		{
			fPlayTime = (fPlayTime + (m_pModel->Find_Animation("AS_ch0100_621_AL_BF_swing_air_start")->GetCurDuration()));// / fDurationSum;
		}
		else if (m_pModel->GetPlayAnimation()->GetName() == "AS_ch0100_621_AL_BF_swing_air_landing")
		{
			fPlayTime = (fPlayTime
				+ (m_pModel->Find_Animation("AS_ch0100_621_AL_BF_swing_air_start")->GetCurDuration())
				+ m_pModel->Find_Animation("AS_ch0100_621_AL_BF_swing_air_loop")->GetCurDuration());// / fDurationSum;
		}

		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio(fPlayTime);
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix WaistMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist");
		_vector vLook = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fLook;
		_vector vUp = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_UP)) * fUp;

		_matrix	SocketMatrix = WaistMatrix * m_pTransformCom->Get_WorldMatrix();
		SocketMatrix.r[3] += vLook + vUp;

		_matrix KineticObjectPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();

		_float fScale_Right = XMVectorGetX(XMVector3Length(KineticObjectPos.r[0]));
		_float fScale_Up = XMVectorGetX(XMVector3Length(KineticObjectPos.r[1]));
		_float fScale_Look = XMVectorGetX(XMVector3Length(KineticObjectPos.r[2]));

		_float fRatio = m_pModel->GetPlayAnimation()->GetPlayRatio() * 10.f;

		if (1.f < fRatio)
		{
			fRatio = 1.f;
		}

		_vector vLerpLook = XMVectorLerp(KineticObjectPos.r[2], SocketMatrix.r[2], fRatio);
		_vector vLerpRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook);
		_vector vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);
		_vector vLerpPos = XMVectorLerp(KineticObjectPos.r[3], SocketMatrix.r[3], fRatio);

		vLerpRight = XMVector3Normalize(vLerpRight) * fScale_Right;
		vLerpUp = XMVector3Normalize(vLerpUp) * fScale_Up;
		vLerpLook = XMVector3Normalize(vLerpLook) * fScale_Look;

		_matrix LerpMatrix = { vLerpRight, vLerpUp, vLerpLook, vLerpPos };

		CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Set_WorldMatrix(LerpMatrix);
	}
}

void CPlayer::BrainField_ObjectAnimation_Socket(const string& strSocket, _float fLook, _float fUp)
{
	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		if (nullptr == m_pASM->GetSocketAnimation(strSocket))
			return;

		_float fPlayTime = m_pASM->GetSocketAnimation(strSocket)->GetPlayTime();

		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio(fPlayTime);
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix WaistMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist");
		_vector vLook = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fLook;
		_vector vUp = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_UP)) * fUp;

		_matrix	SocketMatrix = WaistMatrix * m_pTransformCom->Get_WorldMatrix();
		SocketMatrix.r[3] += vLook + vUp;

		_matrix KineticObjectPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();

		_float fScale_Right = XMVectorGetX(XMVector3Length(KineticObjectPos.r[0]));
		_float fScale_Up = XMVectorGetX(XMVector3Length(KineticObjectPos.r[1]));
		_float fScale_Look = XMVectorGetX(XMVector3Length(KineticObjectPos.r[2]));

		_float fRatio = m_pASM->GetSocketAnimation(strSocket)->GetPlayRatio() * 10.f;

		if (1.f < fRatio)
		{
			fRatio = 1.f;
		}

		_vector vLerpLook = XMVectorLerp(KineticObjectPos.r[2], SocketMatrix.r[2], fRatio);
		_vector vLerpRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook);
		_vector vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);
		_vector vLerpPos = XMVectorLerp(KineticObjectPos.r[3], SocketMatrix.r[3], fRatio);

		vLerpRight = XMVector3Normalize(vLerpRight) * fScale_Right;
		vLerpUp = XMVector3Normalize(vLerpUp) * fScale_Up;
		vLerpLook = XMVector3Normalize(vLerpLook) * fScale_Look;

		_matrix LerpMatrix = { vLerpRight, vLerpUp, vLerpLook, vLerpPos };

		CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Set_WorldMatrix(LerpMatrix);
	}
}

void CPlayer::BrainField_ObjectGather(_float fLook, _float fUp)
{
	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		_matrix matPlayer = m_pTransformCom->Get_WorldMatrix();
		_matrix matKinetic = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();

		_vector vLook = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fLook;
		_vector vUp = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_UP)) * fUp;

		matPlayer.r[3] += (vUp + vLook);

		_float fScale_Right = XMVectorGetX(XMVector3Length(matKinetic.r[0]));
		_float fScale_Up = XMVectorGetX(XMVector3Length(matKinetic.r[1]));
		_float fScale_Look = XMVectorGetX(XMVector3Length(matKinetic.r[2]));

		m_fGatherLerp = min(m_fGatherLerp + (m_fTimeDelta * 0.25f), 1.f);

		_vector vLerpLook = XMVectorLerp(matKinetic.r[2], matPlayer.r[2], m_fGatherLerp);
		_vector vLerpRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook);
		_vector vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);
		_vector vLerpPos = XMVectorLerp(matKinetic.r[3], matPlayer.r[3], m_fGatherLerp);

		vLerpRight = XMVector3Normalize(vLerpRight) * fScale_Right;
		vLerpUp = XMVector3Normalize(vLerpUp) * fScale_Up;
		vLerpLook = XMVector3Normalize(vLerpLook) * fScale_Look;

		_matrix LerpMatrix = { vLerpRight, vLerpUp, vLerpLook, vLerpPos };

		CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Set_WorldMatrix(LerpMatrix);

		// ¡÷∫Ø ø¿∫Í¡ß∆Æ ∏ÆΩ∫∆Æ ≈Ωªˆ«œø© ≈∞≥◊∆Ω ø¿∫Í¡ß∆ÆøÕ ∞°±ÓøÓ ø¿∫Í¡ß∆ÆµÈ¿ª ≤¯æÓµÈ¿”

		if (m_GatherObjectList.empty()) return;

		for (auto& iter : m_GatherObjectList)
		{
			_float4 vChildPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
			_float4 vParentPos = matKinetic.r[3];
			_float4 vDir = vParentPos - vChildPos;
			vDir.Normalize();
			vDir = vDir * m_fGatherPower;
			vDir.w = 0.f;

			static_cast<CMapKinetic_Object*>(iter)->Add_Physical({ vDir.x, vDir.y, vDir.z });
		}
	}
}

void CPlayer::BrainField_FindGatherObjectList()
{
	if (CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject") == nullptr
		|| CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects().empty())
	{
		return;
	}
	else
	{
		_float fDistance = 10.f;

		for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
		{
			if (iter == CPlayerInfoManager::GetInstance()->Get_KineticObject()) continue;

			static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(false);
			static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Kinetic(true);

			if (false == static_cast<CMapKinetic_Object*>(iter)->GetThrow() && true == static_cast<CMapKinetic_Object*>(iter)->Usable())
			{
				_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vMyPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

				_float fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

				if (fDistance > fCurDistance)
				{
					m_GatherObjectList.push_back(iter);
					static_cast<CMapKinetic_Object*>(iter)->Set_Dynamic();
				}
			}
		}
	}
}

void CPlayer::BrainField_ObjectGather_Throw()
{
	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject() && (nullptr == m_pCenterObject))
	{
		m_pCenterObject = CPlayerInfoManager::GetInstance()->Get_KineticObject();
	}

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCenterObject))
	{
		_uint iIndex = 0;
		_bool bHit = false;

		for (auto& iter : m_GatherObjectList)
		{
			if (false == CGameInstance::GetInstance()->Check_ObjectAlive(iter))
				continue;

			if (auto KineticCheck = dynamic_cast<CMapKinetic_Object*>(iter))
			{
				KineticCheck->SetThrow();

				if (KineticCheck->Get_Hit())
					bHit = true;

				if (bHit)
				{
					KineticCheck->Set_Dynamic();
					KineticCheck->Set_Hit();
				}
				else
				{
					_float4 vTargetPos = m_pCenterObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

					_float4 vChildPos = vTargetPos + m_vecGatherOverlap[iIndex];

					KineticCheck->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vChildPos);
				}
			}

			iIndex++;
		}
	}
	else
	{
		for (auto& iter : m_GatherObjectList)
		{
			if (false == CGameInstance::GetInstance()->Check_ObjectAlive(iter))
				continue;

			if (auto KineticCheck = dynamic_cast<CMapKinetic_Object*>(iter))
			{
				KineticCheck->SetThrow();
				KineticCheck->Set_Dynamic();
				KineticCheck->Set_Hit();
			}
		}
	}
}

void CPlayer::BrainField_ObjectGather_Animation()
{

	if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject() && (nullptr == m_pCenterObject))
	{
		m_pCenterObject = CPlayerInfoManager::GetInstance()->Get_KineticObject();
	}

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCenterObject))
	{
		_uint iIndex = 0;
		_bool bHit = false;

		for (auto& iter : m_GatherObjectList)
		{
			if (false == CGameInstance::GetInstance()->Check_ObjectAlive(iter))
				continue;

			static_cast<CMapKinetic_Object*>(iter)->SetThrow();

			if (static_cast<CMapKinetic_Object*>(iter)->Get_Hit())
				bHit = true;

			if (bHit)
			{
				static_cast<CMapKinetic_Object*>(iter)->Set_Dynamic();
				static_cast<CMapKinetic_Object*>(iter)->Set_Hit();
			}
			else
			{
				_float4 vTargetPos = m_pCenterObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

				_float4 vChildPos = vTargetPos + m_vecGatherOverlap[iIndex];

				iter->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vChildPos);
			}
			iIndex++;
		}
	}
	else
	{
		for (auto& iter : m_GatherObjectList)
		{
			if (false == CGameInstance::GetInstance()->Check_ObjectAlive(iter))
				continue;

			if (auto KineticCheck = dynamic_cast<CMapKinetic_Object*>(iter))
			{
				KineticCheck->SetThrow();
				KineticCheck->Set_Dynamic();
				KineticCheck->Set_Hit();
			}
		}
	}

}

void CPlayer::Start_RimLight(const string & strCurveName)
{
	m_pCurve = CGameInstance::GetInstance()->GetCurve(strCurveName);
}

void CPlayer::Tick_RimLight(_float fRatio)
{
	Assert(m_pCurve != nullptr);

	_float fLightPower = m_pCurve->GetValue(fRatio);

	for (auto& iter : m_pModel->GetMaterials())
	{
		iter->GetParam().Floats[0] = 7.f * fLightPower;
		iter->GetParam().Float4s[0] = { 0.945098f, 0.4f, 1.f, 1.f };
	}
}

void CPlayer::End_RimLight()
{
	m_pCurve = nullptr;

	for (auto& iter : m_pModel->GetMaterials())
	{
		iter->GetParam().Floats[0] = 0.f;
		//iter->GetParam().Float4s[0] = { 0.f, 0.f, 0.f, 0.f };
	}
}

void CPlayer::CreateSpecialUI(ESpecialType eType, _bool bAdditional)
{
	Json json;

	switch (eType)
	{
	case SPECIAL_TELEPHONEPOLE:

		json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAMouseLeft.json");
		m_pSpecialUI = CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAMouseLeft", &json);

		break;

	case SPECIAL_HBEAM_BUNDLE:

		json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SARebar.json");
		m_pSpecialUI = CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SARebar", &json);

		break;

	case SPECIAL_IRONBARS:

		if (bAdditional)
		{
			json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_Go.json");
			m_pSpecialUI = CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAGragting_Go", &json);
		}
		else
		{
			json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_S.json");
			m_pSpecialUI = CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAGragting_S", &json);
		}

		break;

	case SPECAIL_CONTAINER:

		if (bAdditional)
		{
			json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAContainer_Down.json");
			m_pSpecialUI = CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAContainer_Down", &json);
		}
		else
		{
			json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_S.json");
			m_pSpecialUI = CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAGragting_S", &json);
		}

		break;

	default:
		break;
	}
}

void CPlayer::SasGearEffect()
{
	_float4x4 Pivot01 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.17f, 0.09f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot01, "Sheath", true, true);

	_float4x4 Pivot02 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(-0.17f, 0.1f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot02, "Sheath", true, true);

	_float4x4 Pivot03 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.1f, -0.1f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot03, "Sheath", true, true);

	_float4x4 Pivot04 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.08f, -0.3f, 0.08f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot04, "Sheath", true, true);

	_float4x4 Pivot05 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(-0.172f, -0.274f, 0.045f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot05, "Sheath", true, true);
}

void CPlayer::SasGearReleaseEffect()
{
	_float4x4 Pivot01 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.17f, 0.09f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Dead_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot01, "Sheath", true, true);

	_float4x4 Pivot02 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(-0.17f, 0.1f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Dead_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot02, "Sheath", true, true);

	_float4x4 Pivot03 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.1f, -0.1f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Dead_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot03, "Sheath", true, true);

	_float4x4 Pivot04 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.08f, -0.3f, 0.08f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Dead_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot04, "Sheath", true, true);

	_float4x4 Pivot05 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(-0.172f, -0.274f, 0.045f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Dead_Sas_Gear"), LAYER_PLAYEREFFECT)
		->Start_AttachPivot(this, Pivot05, "Sheath", true, true);
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& iter : m_vecWeapon)
		Safe_Release(iter);
	m_vecWeapon.clear();
	
	for (auto& iter : m_vecSheath)
		Safe_Release(iter);
	m_vecSheath.clear();

	Safe_Release(m_pKineticComboStateMachine);
	Safe_Release(m_pKineticStataMachine);
	Safe_Release(m_pHitStateMachine);
	Safe_Release(m_pJustDodgeStateMachine);

	Safe_Release(m_pASM);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pController);
	Safe_Release(m_pPlayerCam);
	Safe_Release(m_pBattleChecker);

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pPlayer_AnimCam))
		Safe_Release(m_pPlayer_AnimCam);

	Safe_Release(m_pKineticAnimModel);

	Safe_Release(m_pCurve);
	Safe_Release(m_pRange);

	Safe_Release(m_pTrainStateMachine_Left);
	Safe_Release(m_pTelephonePoleStateMachine_Left);
	Safe_Release(m_pBrainCrashStateMachine);
	Safe_Release(m_pHBeamStateMachine_Left);
	Safe_Release(m_pTeleportStateMachine);
	Safe_Release(m_pSAS_Cable);
	Safe_Release(m_pBrainField);
	Safe_Release(m_pTrail);

	Safe_Release(m_pDropObjectStateMachine);
	Safe_Release(m_pSAS_Penetrate);
	Safe_Release(m_pSuperSpeedPostVFX);
	Safe_Release(m_pTeleportPostVFX);
	Safe_Release(m_pTankLorryStateMachine);
	Safe_Release(m_pIronBarsStateMachine);
	Safe_Release(m_pContainerStateMachine);

	Safe_Release(m_pDriveModeProductionStateMachine);
	Safe_Release(m_pBrainFieldProductStateMachine);

	Safe_Release(m_pBrainFieldAttackStateMachine);
	Safe_Release(m_pBrainFieldKineticStateMachine);
	Safe_Release(m_pBrainFieldKineticComboStateMachine);
	Safe_Release(m_pBrainFieldFallStateMachine);

	Safe_Release(m_pHotFixer);

//	Safe_Release(m_pContectRigidBody);

	Safe_Release(m_pDeBuffStateMachine);
	if (m_pNoticeNeon.first != nullptr && m_pNoticeNeon.second != nullptr)
	{
		m_pNoticeNeon.first->SetDelete();
		m_pNoticeNeon.second->Delete_Particles();

		Safe_Release(m_pNoticeNeon.first);
		Safe_Release(m_pNoticeNeon.second);

		m_pNoticeNeon.first = nullptr;
		m_pNoticeNeon.second = nullptr;
	}
	if (m_pCautionNeon.first != nullptr/* && m_pCautionNeon.second != nullptr*/)
	{
		m_pCautionNeon.first->SetDelete();
		//m_pCautionNeon.second->Delete_Particles();

		Safe_Release(m_pCautionNeon.first);
		//Safe_Release(m_pCautionNeon.second);

		m_pCautionNeon.first = nullptr;
		//m_pCautionNeon.second = nullptr;
	}
}
