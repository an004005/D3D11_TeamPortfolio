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
#include "TrailSystem.h"
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
#include "MonsterHpUI.h"
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

#include "Enemy.h"
#include "PlayerStartPosition.h"
#include "SuperSpeedTrail.h"


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

	m_pGameInstance->Add_EmptyLayer(LEVEL_NOW, LAYER_KINETIC);

	ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
	ZeroMemory(&m_AttackDesc, sizeof(DAMAGE_PARAM));

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pTransformCom->SetTransformDesc({ 5.f, XMConvertToRadians(720.f) });

	m_pPlayerCam = m_pGameInstance->Add_Camera("PlayerCamera", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Player");
	Safe_AddRef(m_pPlayerCam);

	m_pModel->FindMaterial(L"MI_ch0100_HOOD_0")->SetActive(false);

	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_00");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_01");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_02");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_03");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_04");
	m_vecRandomLandingDustName.push_back(L"Player_Foot_Smoke_05");

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

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 2.f, 0.f, 1.f));
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
	// 테스트
	//m_pKineticAnimModel->SetPlayAnimation("AS_no0000_271_AL_Pcon_cReL_Lv4");

	// 위치 지정은 여기에서
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pModel->Tick(TimeDelta);
	m_pTrail->Tick(TimeDelta);

	if (m_bOnBattle)
	{
		CPlayerInfoManager::GetInstance()->Set_KineticEnetgyType(0);
	}
	else
	{
		CPlayerInfoManager::GetInstance()->Set_KineticEnetgyType(2);

	}

	CPlayerInfoManager::GetInstance()->Tick(TimeDelta);

	KineticObject_OutLineCheck();
	SpecialObject_OutLineCheck();
	Update_TargetUI();

	// 콤보 연계 시간
	m_fKineticCombo_Slash -= TimeDelta;
	m_fKineticCombo_Kinetic -= TimeDelta;
	m_fJustDodgeAble -= TimeDelta;

	NetualChecker(TimeDelta);	// 전투상태 아닐 때 체크
	SasMgr();

	// 타게팅
	if (CGameInstance::GetInstance()->KeyDown(DIK_Q))
	{
		Enemy_Targeting(true);
	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_E))
	{

	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_R))
	{

	}

	 if (m_pPlayerCam->IsMainCamera())
		m_pController->Tick(TimeDelta);
	 else
		m_pController->Invalidate();

	MoveStateCheck(TimeDelta);
	BehaviorCheck(TimeDelta);

	// SAS특수
	if (CPlayerInfoManager::GetInstance()->Get_isSasUsing(ESASType::SAS_TELEPORT)) 
		m_pTeleportStateMachine->Tick(TimeDelta);
	// ~SAS특수

	m_pHitStateMachine->Tick(TimeDelta);

	HitCheck();

	// 브레인 크러시?
	m_pBrainCrashStateMachine->Tick(TimeDelta);

	// 특수기
	if (false == m_bKineticSpecial)
	{
		SpecialObject_Targeting();
	}

	if (!m_bHit)
	{
		if (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject())
		{
			if (SPECIAL_TRAIN == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pTrainStateMachine_Left->Tick(TimeDelta);
			}

			if (SPECIAL_TELEPHONEPOLE == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pTelephonePoleStateMachine_Left->Tick(TimeDelta);
			}

			if (SPECIAL_HBEAM_BUNDLE == dynamic_cast<CSpecialObject*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Get_SpecialType())
			{
				m_pHBeamStateMachine_Left->Tick(TimeDelta);
			}
		}
	}
	// ~특수기

	/*if(!m_bHit)
		m_pJustDodgeStateMachine->Tick(TimeDelta);*/

	if (!m_bHit && (false == m_bKineticCombo) && (false == m_bKineticSpecial)) // 콤보 타이밍이 아닐 때에는 일반 염력
	{
		m_pKineticStataMachine->Tick(TimeDelta);
	}
	if (!m_bHit && (false == m_bKineticSpecial)) // 콤보 타이밍에는 콤보 염력
	{
		m_pKineticComboStateMachine->Tick(TimeDelta);
		//m_pKineticStataMachine->SetState("NO_USE_KINETIC");
	}

	if (m_bHit)
	{
		m_pKineticComboStateMachine->SetState("KINETIC_COMBO_NOUSE");
		m_pKineticStataMachine->SetState("NO_USE_KINETIC");
		m_pJustDodgeStateMachine->SetState("JUSTDODGE_NONUSE");
		m_pTrainStateMachine_Left->SetState("TRAIN_LEFT_NOUSE");
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(false);
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, false);

		m_pCurve = nullptr;
		for (auto& iter : m_pModel->GetMaterials())
		{
			iter->GetParam().Floats[0] = 0.f;
			iter->GetParam().Float4s[0] = { 0.f, 0.f, 0.f, 0.f };
		}
	}

	SeperateCheck();

	m_pASM->Tick(TimeDelta);

	if (m_bCanMove)
	{
		//IM_LOG("Move");

		_float fSpeedControl = 0.f;

		if (m_bWalk)				fSpeedControl = g_fTimeDelta;

		if (m_bCanRun)				fSpeedControl *= 2.f;

		m_pTransformCom->Move(fSpeedControl, m_vMoveDir);
	}
//	IM_LOG("%d", m_bCanMove);
	for (auto& iter : m_vecWeapon)
	{
		iter->Tick(TimeDelta);
		{
			_bool bCol = Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART));

			if (bCol)
			{
				m_fKineticCombo_Slash = 5.f;

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
		}
	}

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	/*if (pGameInstance->KeyDown(DIK_NUMPAD1))
	{
		list<CAnimation*> TestAnim;
		TestAnim.push_back(m_pModel->Find_Animation("AS_BC_em_common_ch0100_end"));
		m_pASM->InputAnimSocket("Common_AnimSocket", TestAnim);
	}
	if (pGameInstance->KeyPressing(DIK_NUMPAD2))
	{
		static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
	}
	*/
	SocketLocalMoveCheck();

	if (pGameInstance->KeyDown(DIK_F1))
	{
		m_bHit = true;
		m_DamageDesc.m_vHitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_DamageDesc.m_eHitDir = EBaseAxis::NORTH;
		m_DamageDesc.m_iDamageType = EAttackType::ATK_LIGHT;
	}
	if (pGameInstance->KeyDown(DIK_F2))
	{
		m_bHit = true;
		m_DamageDesc.m_vHitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_DamageDesc.m_eHitDir = EBaseAxis::NORTH;
		m_DamageDesc.m_iDamageType = EAttackType::ATK_MIDDLE;
	}
	if (pGameInstance->KeyDown(DIK_F3))
	{
		m_bHit = true;
		m_DamageDesc.m_vHitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_DamageDesc.m_eHitDir = EBaseAxis::NORTH;
		m_DamageDesc.m_iDamageType = EAttackType::ATK_HEAVY;
	}
	if (pGameInstance->KeyDown(DIK_F4))
	{
		m_bHit = true;
		m_DamageDesc.m_vHitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_DamageDesc.m_eHitDir = EBaseAxis::NORTH;
		m_DamageDesc.m_iDamageType = EAttackType::ATK_TO_AIR;
	}

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
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (auto& iter : m_vecWeapon)
		iter->Late_Tick(TimeDelta);

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
}

void CPlayer::AfterPhysX()
{
	__super::AfterPhysX();

	for (auto& iter : m_vecWeapon)
	{
		static_cast<CScarletWeapon*>(iter)->Setup_BoneMatrix(m_pModel, m_pTransformCom->Get_WorldMatrix());
	}
	m_pCamSpot->SetUp_BoneMatrix(m_pModel, m_pTransformCom->Get_WorldMatrix());
	m_pRange->Update_Tick(m_pTransformCom);

}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

HRESULT CPlayer::Render_ShadowDepth()
{
	m_pModel->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CPlayer::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	// 현재 애니메이션 상태에 따라 저스트닷지 여부 판단
	if (m_pModel->GetPlayAnimation()->GetName().find("dodge") != string::npos &&
		m_pASM->isSocketExactlyEmpty())
	{
		m_fJustDodgeAble = 10.f;
	}
	else
	{
		//m_bHit = true;

		//m_DamageDesc.m_iDamage = tDamageParams.iDamage;
		//m_DamageDesc.m_iDamageType = tDamageParams.eAttackType;
		//m_DamageDesc.m_vHitDir = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - XMLoadFloat4(&tDamageParams.vHitFrom);
		//m_DamageDesc.m_eHitDir = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);

		//// 체력 깎이는 부분
		//CPlayerInfoManager::GetInstance()->Change_PlayerHP(CHANGE_DECREASE, tDamageParams.iDamage);

		//if (tDamageParams.eAttackType == EAttackType::ATK_HEAVY || tDamageParams.eAttackType == EAttackType::ATK_TO_AIR)
		//{
		//	m_pTransformCom->LookAt_NonY(tDamageParams.pCauser->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
		//}
	}
}

void CPlayer::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::InputFloat("ThrowPower", &m_fThrowPower);
	ImGui::InputFloat("ChargePower", &m_fChargePower);

	if (ImGui::CollapsingHeader("pivot1"))
	{
		static GUIZMO_INFO tp1;
		CImguiUtils::Render_Guizmo(&pivot1, tp1, true, true);
	}
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

	// HP Bar Check	
	ImGui::Text("HP : %d", CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP);
	ImGui::Text(m_pModel->GetPlayAnimation()->GetName().c_str());

	//m_pKineticStataMachine->Imgui_RenderProperty();
	//m_pTrainStateMachine_Left->Imgui_RenderProperty();
	//m_pBrainCrashStateMachine->Imgui_RenderProperty();
	m_pTeleportStateMachine->Imgui_RenderProperty();


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

	// 이펙트 장착 툴
	if (ImGui::CollapsingHeader("Effect_Attach"))
	{
		static string szAnimName = "";
		static _float fEffectPlayTime = 0.f;
		static _float fEffectSize = 1.f;
		static char cBoneName[MAX_PATH]{ "Eff01" };

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

		if (ImGui::Button("InputSocket"))
		{
			list<CAnimation*> TestAnimSocket;
			TestAnimSocket.push_back(m_pModel->Get_AnimList()[szAnimName]);
			m_pASM->InputAnimSocket("Common_AnimSocket", TestAnimSocket);
		}

		static char cEffectName[MAX_PATH]{};
		static string szEffectName = "";
		ImGui::InputText("Effect Search", cEffectName, MAX_PATH);
		szEffectName = cEffectName;

		if ((m_pModel->GetPlayAnimation()->GetName() == szAnimName) &&
			(fPrePlayTime <= fEffectPlayTime) &&
			(fCurPlayTime > fEffectPlayTime))
		{
			//Event_Effect(szEffectName, fEffectSize, cBoneName);
			//if (m_PlayerSasType == ESASType::SAS_NOT)
			//	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, s2ws(szEffectName))->Start_Attach(this, szBoneName);
			//else if (m_PlayerSasType == ESASType::SAS_FIRE)
			//	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, s2ws(szEffectName))->Start_Attach(this, szBoneName);
		}

		fPrePlayTime = fCurPlayTime;

		if("" != szAnimName)
			m_pModel->Get_AnimList()[szAnimName]->Imgui_RenderProperty();
	}
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
		m_bSASSkillInput[0] = !m_bSASSkillInput[0];
	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_2))
	{
		InputSas = ESASType::SAS_PENETRATE;
		m_bSASSkillInput[1] = !m_bSASSkillInput[1];

	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_3))
	{
		InputSas = ESASType::SAS_HARDBODY;
		m_bSASSkillInput[2] = !m_bSASSkillInput[2];

	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_4))
	{
		InputSas = ESASType::SAS_FIRE;
		m_bSASSkillInput[3] = !m_bSASSkillInput[3];

	}
	if (CGameInstance::GetInstance()->KeyDown(DIK_5))	InputSas = ESASType::SAS_SUPERSPEED;
	if (CGameInstance::GetInstance()->KeyDown(DIK_6))	InputSas = ESASType::SAS_COPY;
	if (CGameInstance::GetInstance()->KeyDown(DIK_7))	InputSas = ESASType::SAS_INVISIBLE;
	if (CGameInstance::GetInstance()->KeyDown(DIK_8))	InputSas = ESASType::SAS_ELETRIC;

	if (InputSas != ESASType::SAS_END)
	{
		// 해당 SAS를 사용중인지 아닌지 판단
		_bool bSasIsUsing = false;
		for (auto& SAS : CPlayerInfoManager::GetInstance()->Get_PlayerSasList())
		{
			if (InputSas == SAS) { bSasIsUsing = true; break; }
		}

		// 입력받은 SAS가 최소 사용 요건에 만족하지 못하며 사용중이 아닐 경우 사용 불가능 메시지 발생
		if (false == CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[static_cast<_uint>(InputSas)].bUsable &&
			false == bSasIsUsing)
		{
			IM_LOG("Sas_Unusable");
		}
		else
		{
			// 입력받은 SAS가 사용 중일 경우
			if (bSasIsUsing)
			{
				CPlayerInfoManager::GetInstance()->Finish_SasType(InputSas);

				if (ESASType::SAS_FIRE == InputSas)
				{
					if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSwordParticle)) m_pSwordParticle->SetDelete();
					m_pSAS_Cable->UnEquipCable();
				}
				else if (ESASType::SAS_TELEPORT == InputSas)
				{
					m_pASM->Add_SpairSasMotion(ESASType::SAS_NOT);
				}
			}
			else // 사용중이지 않을 경우
			{
				SasOn.Reset();

				SasGearEffect();	// 기어 이펙트 생성

				if (ESASType::SAS_FIRE == InputSas)
				{
					m_pSasPortrait->Start_SAS(InputSas);
					m_pSAS_Cable->EquipCable(ESASType::SAS_FIRE);
				}
				else if (ESASType::SAS_TELEPORT == InputSas)
				{
					m_pASM->Add_SpairSasMotion(ESASType::SAS_TELEPORT);
					m_pSAS_Cable->EquipCable(ESASType::SAS_TELEPORT);
				}

				CPlayerInfoManager::GetInstance()->Set_SasType(InputSas);
			}
		}
	}

	if (m_pSAS_Cable->GetIsActive())
	{
		if (SasOn.IsNotDo())
		{
			if (ESASType::SAS_FIRE == CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, TEXT("Sas_Fire_Start"))->Start_Attach(this, "Sheath");
				m_pSwordParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Weapon_Particle"));
				m_pSwordParticle->Start_Attach(this, "RightWeapon", true);
			}

			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			list<CAnimation*> SasDamage;
			SasDamage.push_back(m_pModel->Find_Animation("AS_ch0100_410_AL_damage_sas"));
			m_pASM->InputAnimSocket("Common_AnimSocket", SasDamage);
		}
	}

	if (!CPlayerInfoManager::GetInstance()->Get_PlayerSasList().empty())
	{
		if (m_pSasPortrait->isFinish())
		{
			_matrix EffectPivot = XMMatrixIdentity();


			switch (CPlayerInfoManager::GetInstance()->Get_PlayerSasList().back())
			{
			case ESASType::SAS_FIRE:
				break;
			default:
				break;
			}

		}
	}

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
		m_bVisible = false;

		for (auto& iter : m_vecWeapon)
			iter->SetVisible(false);

		m_pSAS_Cable->SetVisible(false);
	}
	else
	{
		m_bVisible = true;

		for (auto& iter : m_vecWeapon)
			iter->SetVisible(true);

		m_pSAS_Cable->SetVisible(true);
	}
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

			// 애니메이션만 돌릴 모델
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), L"Test2",
				(CComponent**)&m_pKineticAnimModel));
		}

		//FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody",
		//	L"ContectRigidBody", (CComponent**)&m_pContectRigidBody, pArg));
		
	}

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_LocalController"), TEXT("Com_Controller"),
		(CComponent**)&m_pController));
	
	NULL_CHECK(m_pASM = CBaseAnimInstance::Create(m_pModel, this));

	Json PlayerCollider = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Player/PlayerRange.json");
	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody",
		L"PlayerRangeCollider", (CComponent**)&m_pRange, &PlayerCollider));

	m_pSAS_Cable = dynamic_cast<CSAS_Cable*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Prototype_GameObject_SASCable"));
	Assert(m_pSAS_Cable != nullptr);
	m_pSAS_Cable->SetTargetInfo(m_pTransformCom, m_pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_SuperSpeedTrail", L"SuperSpeedTrail", (CComponent**)&m_pTrail));
	m_pTrail->SetOwnerModel(m_pModel);
	m_pTrail->SetActive(true);

	return S_OK;
}

HRESULT CPlayer::SetUp_RigidBody()
{
	//m_pCollider->SetContactCallback([this](CGameObject* pGameObject, ECOLLIDER_TYPE eType)
	//{
	//	if (auto pMonster = dynamic_cast<CMonster*>(pGameObject))
	//	{
	//		m_bOptionalMove = false;
	//	}
	//	else
	//	{
	//		m_bOptionalMove = true;
	//	}
	//});

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
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_5")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
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
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_5_PreEffect")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
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
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_Air_Dash_0")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
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
	m_pModel->Add_EventCaller("Default_Attack_Dash_Hold_0", [&]() {Event_Effect("Default_Attack_Dash_Hold_0"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_Hold_1", [&]() {Event_Effect("Default_Attack_Dash_Hold_1"); });
	m_pModel->Add_EventCaller("Default_Attack_Dash_Hold_2", [&]() {Event_Effect("Default_Attack_Dash_Hold_2"); });
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
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_1_001")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
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
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_1_001_Particle"))->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_2", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix = XMMatrixScaling(3.f, 3.f, 3.f)
				* XMMatrixRotationZ(XMConvertToRadians(160.f))
				* XMMatrixTranslation(0.f, 0.75f, 0.25f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_2")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_2_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix = XMMatrixScaling(3.f, 3.f, 3.f)
				* XMMatrixRotationZ(XMConvertToRadians(160.f))
				* XMMatrixTranslation(0.f, 0.75f, 0.25f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_2_Particle"))->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_3", [&]() {Event_Effect("Fire_Attack_3"); });
	m_pModel->Add_EventCaller("Fire_Attack_3_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_3_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_3_twist", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, 0.f, -2.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_3_Double_twist")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);		//CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Sas_Fire_Sword_Particle"))->Start_AttachSword(m_vecWeapon.front(), true);
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_3_twist_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixScaling(10.f, 10.f, 10.f) *
				XMMatrixTranslation(0.f, 0.f, -2.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, L"Fire_Attack_3_Twist_Particle")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", false);	
		
		}
	});

	m_pModel->Add_EventCaller("Fire_Attack_4", [&]() {Event_Effect("Fire_Attack_4"); });
	m_pModel->Add_EventCaller("Fire_Attack_4_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_4_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_4_001", [&]() {Event_Effect("Fire_Attack_4_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_4_001_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_4_001_Particle"))->Start_Attach(this, "Eff01");
	});


	m_pModel->Add_EventCaller("Fire_Attack_5", [&]() {Event_Effect("Fire_Attack_5"); });
	m_pModel->Add_EventCaller("Fire_Attack_5_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_5_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_5_PreEffect", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix = XMMatrixTranslation(0.f, 1.f, 0.5f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_5_PreEffect")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}

//		Event_Effect("Fire_Attack_5_PreEffect");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_1", [&]() {Event_Effect("Fire_Attack_Air_1"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_1_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_1_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_2", [&]() {Event_Effect("Fire_Attack_Air_2"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_2_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_2_Particle"))->Start_Attach(this, "Eff01");
	});


	m_pModel->Add_EventCaller("Fire_Attack_Air_Chase", [&]() {Event_Effect("Fire_Attack_Air_Chase"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_chase_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_chase_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_0", [&]() {Event_Effect("Fire_Attack_Air_dash_0"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_0_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_dash_0_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_End", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, -1.1f, -1.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Air_dash_0")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_End_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixTranslation(0.f, -1.1f, -1.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_dash_0_Particle"))->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});


	m_pModel->Add_EventCaller("Fire_AttacK_Air_dash_0_001", [&]() {Event_Effect("Fire_AttacK_Air_dash_0_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1", [&]() {Event_Effect("Fire_Attack_Air_dash_1"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1_001", [&]() {Event_Effect("Fire_Attack_Air_dash_1_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1_002", [&]() {Event_Effect("Fire_Attack_Air_dash_1_002"); });

	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold", [&]() {Event_Effect("Fire_Attack_Air_Hold");});
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_Hold_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_1", [&]() {Event_Effect("Fire_Attack_Air_Hold_1");});
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_1_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_Hold_1_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_2", [&]() {Event_Effect("Fire_Attack_Air_Hold_2");});
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_2_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_Hold_2_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_Landing", [&]() {Event_Effect("Fire_Attack_Air_Hold_Landing");});
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_Landing_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Air_Hold_Landing_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Charge03", [&]() {Event_Effect("Fire_Attack_Charge03");});
	m_pModel->Add_EventCaller("Fire_Attack_Charge03_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Charge03_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Charge03_001", [&]() {Event_Effect("Fire_Attack_Charge03_001");});
	m_pModel->Add_EventCaller("Fire_Attack_Charge03_001_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Charge03_001_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Charge03_002", [&]() {Event_Effect("Fire_Attack_Charge03_002");});
	m_pModel->Add_EventCaller("Fire_Attack_Charge03_002_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Charge03_002_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_00", [&]() {Event_Effect("Fire_Attack_Dash_00");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_00_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_00_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_01", [&]() {Event_Effect("Fire_Attack_Dash_01");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_01_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_01_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_001", [&]() {Event_Effect("Fire_Attack_Dash_02_001");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_001_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_02_001_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_002", [&]() {Event_Effect("Fire_Attack_Dash_02_002");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_002_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_02_002_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Fire_Attack_Dash_03", [&]() {Event_Effect("Fire_Attack_Dash_03");});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_03_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_03_Particle"))->Start_Attach(this, "Eff01");
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
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_Dash_Hold_003")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});
	m_pModel->Add_EventCaller("Fire_Attack_Dash_Hold_003_Particle", [&]()
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
		{
			_float4x4 EffectPivotMatrix =
				XMMatrixScaling(1.5f, 1.5f, 1.5f)
				* XMMatrixTranslation(-2.f, -1.5f, 2.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Dash_Hold_003_Particle"))->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
		}
	});

	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_01", [&]() {Event_Effect("Fire_Attack_Justdodge_01"); });
	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_02", [&]() {Event_Effect("Fire_Attack_Justdodge_02"); });
	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_03", [&]() {Event_Effect("Fire_Attack_Justdodge_03"); });
	m_pModel->Add_EventCaller("Fire_Attack_Upper", [&]() {Event_Effect("Fire_Attack_Upper");});
	m_pModel->Add_EventCaller("Fire_Attack_Upper_Particle", [&]() 
	{
		if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Attack_Upper_Particle"))->Start_Attach(this, "Eff01");
	});

	m_pModel->Add_EventCaller("Elec_Attack_1", [&]() {Event_Effect("Elec_Attack_1", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_1_001", [&]() {Event_Effect("Elec_Attack_1_001", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_2", [&]() {Event_Effect("Elec_Attack_2", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_3", [&]() {Event_Effect("Elec_Attack_3", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_3_001", [&]() {Event_Effect("Elec_Attack_3_001", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_001", [&]() {Event_Effect("Elec_Attack_4_001", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_002", [&]() {Event_Effect("Elec_Attack_4_002", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_003", [&]() {Event_Effect("Elec_Attack_4_003", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_004", [&]() {Event_Effect("Elec_Attack_4_004", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_4_005", [&]() {Event_Effect("Elec_Attack_4_005", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_5", [&]() {Event_Effect("Elec_Attack_5", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_1", [&]() {Event_Effect("Elec_Attack_Air_1", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_2", [&]() {Event_Effect("Elec_Attack_Air_2", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Chase", [&]() {Event_Effect("Elec_Attack_Air_Chase", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_0", [&]() {Event_Effect("Elec_Attack_Air_Dash_0", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_1", [&]() {Event_Effect("Elec_Attack_Air_Dash_1", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Dash_1_001", [&]() {Event_Effect("Elec_Attack_Air_Dash_1_001", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_00", [&]() {Event_Effect("Elec_Attack_Air_Hold_00", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_01", [&]() {Event_Effect("Elec_Attack_Air_Hold_01", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_02", [&]() {Event_Effect("Elec_Attack_Air_Hold_02", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_03", [&]() {Event_Effect("Elec_Attack_Air_Hold_03", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_04", [&]() {Event_Effect("Elec_Attack_Air_Hold_04", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_05", [&]() {Event_Effect("Elec_Attack_Air_Hold_05", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Air_Hold_Landing", [&]() {Event_Effect("Elec_Attack_Air_Hold_Landing", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_01", [&]() {Event_Effect("Elec_Attack_Charge_01", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_02", [&]() {Event_Effect("Elec_Attack_Charge_02", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Charge_03", [&]() {Event_Effect("Elec_Attack_Charge_03", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_1", [&]() {Event_Effect("Elec_Attack_Dash_1", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_2_001", [&]() {Event_Effect("Elec_Attack_Dash_2_001", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_2_002", [&]() {Event_Effect("Elec_Attack_Dash_2_002", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_3", [&]() {Event_Effect("Elec_Attack_Dash_3", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_End", [&]() {Event_Effect("Elec_Attack_Dash_End", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_0", [&]() {Event_Effect("Elec_Attack_Dash_Hold_0", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_1", [&]() {Event_Effect("Elec_Attack_Dash_Hold_1", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_2", [&]() {Event_Effect("Elec_Attack_Dash_Hold_2", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_3", [&]() {Event_Effect("Elec_Attack_Dash_Hold_3", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Dash_Hold_4", [&]() {Event_Effect("Elec_Attack_Dash_Hold_4", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_0", [&]() {Event_Effect("Elec_Attack_Justdodge_0", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_1", [&]() {Event_Effect("Elec_Attack_Justdodge_1", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_2", [&]() {Event_Effect("Elec_Attack_Justdodge_2", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_3", [&]() {Event_Effect("Elec_Attack_Justdodge_3", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_4", [&]() {Event_Effect("Elec_Attack_Justdodge_4", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Justdodge_5", [&]() {Event_Effect("Elec_Attack_Justdodge_5", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Upper_0", [&]() {Event_Effect("Elec_Attack_Upper_0", 1.f, "Eff02"); });
	m_pModel->Add_EventCaller("Elec_Attack_Upper_1", [&]() {Event_Effect("Elec_Attack_Upper_1", 1.f, "Eff02"); });


	m_pModel->Add_EventCaller("Trail_On", [&]() 
	{ 
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(true);
	});

	m_pModel->Add_EventCaller("Trail_Off", [&]() 
	{
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(false);
	});

	m_pModel->Add_EventCaller("WeaponBright_On", [&]()
	{
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, true);
	});

	m_pModel->Add_EventCaller("WeaponBright_Off", [&]()
	{
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, false);
	});

	m_pModel->Add_EventCaller("LandingDust", [&]()
	{
		random_shuffle(m_vecRandomLandingDustName.begin(), m_vecRandomLandingDustName.end());

		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, m_vecRandomLandingDustName.front())->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), _float4(0.f, 1.f, 0.f, 0.f));
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
	// 염력에 사용할 애니메이션 추가

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

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_cancel"));
	m_Kinetic_RB_Air_Throw02_Start.push_back(m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_start"));
	m_Kinetic_RB_Air_Throw02_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_loop"));
	m_Kinetic_RB_Air_Throw02_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_cancel"));

	// ~염력에 사용할 애니메이션 추가

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
				return m_bKineticRB && !m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()) && !m_bHit;
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
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Loop); })
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
			})
			.OnExit([&]() {m_fKineticCharge = 0.f; })

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
			.Predicator([&]()->_bool{return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_AnimSocket")->GetPlayRatio() >= 0.2f);})
			.Priority(0)
			
#pragma endregion KineticRB

#pragma region KineticRB_Throw

		// 일반 던지기
		.AddState("KINETIC_RB_THROW_01_START")	// 루프 / 앤드
			.OnStart([&]() 
			{
				m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw01_Start);

				if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
				{
					Event_Kinetic_Throw();
				}
			})
			.Tick([&](double fTimeDelta) 
			{
				m_bKineticMove = true;
			})

			.AddTransition("KINETIC_RB_THROW_01_START to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket") || m_bLeftClick || m_bDash || m_bJump; })
			.Priority(0)

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

	m_pHitStateMachine = CFSMComponentBuilder().InitState("NON_HIT")
		.AddState("NON_HIT")
			.OnStart([&]() 
			{ 
				//SetAbleState({ false, false, false, false, false, true, true, true, true, false });

				m_bHit = false;

				m_pModel->Reset_LocalMove(true);

				if (!m_pASM->isSocketEmpty("Hit_AnimSocket"))
					m_pASM->GetSocketAnimation("Hit_AnimSocket")->Reset();

				m_pASM->ClearAnimSocket("Hit_AnimSocket");
			})
				.Tick([&](double fTimeDelta) {m_bHit = false; })
				.OnExit([&]() 
			{ 
				m_pASM->SetCurState("IDLE");
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

				.AddTransition("KNUCKBACK to BREAKFALL_FRONT", "BREAKFALL_FRONT")
				.Predicator([&]()->_bool {return m_bBreakFall; })
				.Priority(0)

		.AddState("AIRBORNE")
			.OnStart([&]() { m_pASM->InputAnimSocket("Hit_AnimSocket", m_Airborne); Jump(); })
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

				.AddTransition("KNUCKBACK to BREAKFALL_FRONT", "BREAKFALL_FRONT")
				.Predicator([&]()->_bool {return m_bBreakFall; })
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

		.AddState("FALLDOWN")
			.OnStart([&]() { m_pASM->InputAnimSocket("Hit_AnimSocket", m_FallDown_Back); })
			.Tick([&](double TimeDelta) 
			{
				m_bWalk = false;
				m_bAir = false;
			})
				.AddTransition("FALLDOWN to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

		.AddState("BREAKFALL_FRONT")
			.OnStart([&]() { m_pASM->InputAnimSocket("Hit_AnimSocket", m_BreakFall_Front); })
			.Tick([&](double TimeDelta) 
			{
				m_bWalk = false;
				m_bActiveGravity = false;
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), "AS_ch0100_438_AL_breakfall_F");
				//m_pTransformCom->LocalMove(vLocal);
			})
			.OnExit([&]() {m_bActiveGravity = true; })
				.AddTransition("BREAKFALL_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Hit_AnimSocket", 0.5f) || m_bOnFloor; })
				.Priority(0)

		.AddState("HIT_LIGHT")
			.AddTransition("HIT_LIGHT to FRONT", "HIT_LIGHT_FRONT")
			.Predicator([&]()->_bool 
			{
				_float fDot = XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_DamageDesc.m_vHitDir));
				return (fDot > 0.f) ? true : false; 
			})
			.Priority(0)

			.AddTransition("HIT_LIGHT to BACK", "HIT_LIGHT_BACK")
			.Predicator([&]()->_bool
			{
				_float fDot = XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_DamageDesc.m_vHitDir));
				return (fDot < 0.f) ? true : false;
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

				.AddTransition("HIT_LIGHT_FRONT to HIT_LIGHT", "HIT_LIGHT")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_LIGHT) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

				.AddTransition("HIT_LIGHT_FRONT to HIT_MIDIUM", "HIT_MIDIUM")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_LIGHT_BACK")
			.OnStart([&]() 
			{
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_B_Level01);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
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

				.AddTransition("HIT_LIGHT_BACK to HIT_LIGHT", "HIT_LIGHT")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_LIGHT) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

				.AddTransition("HIT_LIGHT_BACK to HIT_MIDIUM", "HIT_MIDIUM")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM")
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

				.AddTransition("HIT_MIDIUM_FRONT to HIT_LIGHT", "HIT_LIGHT")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_LIGHT) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

				.AddTransition("HIT_MIDIUM_FRONT to HIT_MIDIUM", "HIT_MIDIUM")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_BACK")
			.OnStart([&]() 
			{ 
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_B_Level02);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
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

				.AddTransition("HIT_MIDIUM_BACK to HIT_LIGHT", "HIT_LIGHT")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_LIGHT) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

				.AddTransition("HIT_MIDIUM_BACK to HIT_MIDIUM", "HIT_MIDIUM")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_LEFT")
			.OnStart([&]() 
			{ 
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_L_Level02);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
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

				.AddTransition("HIT_MIDIUM_LEFT to HIT_LIGHT", "HIT_LIGHT")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_LIGHT) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

				.AddTransition("HIT_MIDIUM_LEFT to HIT_MIDIUM", "HIT_MIDIUM")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_RIGHT")
			.OnStart([&]() 
			{ 
				m_pASM->InputAnimSocket("Hit_AnimSocket", m_Hit_R_Level02);
				ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));
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

				.AddTransition("HIT_MIDIUM_RIGHT to HIT_LIGHT", "HIT_LIGHT")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_LIGHT) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

				.AddTransition("HIT_MIDIUM_RIGHT to HIT_MIDIUM", "HIT_MIDIUM")
				.Predicator([&]()->_bool {return m_bHit && !m_bAir && (m_DamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE) && m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
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

m_pKineticComboStateMachine = CFSMComponentBuilder()

		.InitState("KINETIC_COMBO_NOUSE")

		.AddState("KINETIC_COMBO_NOUSE")
		.OnStart([&]() 
		{
			m_bKineticCombo = false;

			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 0.f;

			m_pASM->ClearAnimSocket("Kinetic_Combo_AnimSocket");
			m_pASM->SetCurState("IDLE");

			// 림라이트 종료
			m_pCurve = nullptr;

			for (auto& iter : m_pModel->GetMaterials())
			{
				iter->GetParam().Floats[0] = 0.f;
				iter->GetParam().Float4s[0] = { 0.f, 0.f, 0.f, 0.f };
			}

			Event_Trail(false);
			static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(ESASType::SAS_NOT, false);

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
			static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(ESASType::SAS_NOT, true);
		})

			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_SLASH01", "KINETIC_COMBO_SLASH01")
			.Predicator([&]()->_bool { return !m_bHit && m_bLeftClick && (m_fKineticCombo_Kinetic > 0.f) && !m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_KINETIC01_CHARGE", "KINETIC_COMBO_KINETIC01_CHARGE")
			.Predicator([&]()->_bool { return !m_bHit && m_bKineticRB && (m_fKineticCombo_Slash > 0.f) && !m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

#pragma region 슬래시 콤보 1

		.AddState("KINETIC_COMBO_SLASH01")
		.OnStart([&]() 
		{
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
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

#pragma endregion 슬래시 콤보 1

#pragma region 키네틱 콤보 1

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
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Kinetic(true);
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

			// 임시로 뒤로 빠지게 하는 코드
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
			// 림라이트 커브 생성
			Start_RimLight("Kinetic_Combo_01_RimLight");

			m_fKineticCharge = 0.f;
			m_fKineticCombo_Kinetic = 10.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv1);

			Kinetic_Combo_KineticAnimation();
		})
		.Tick([&](double fTimeDelta)
		{
			// 림라이트 커브 동작 (애니메이션 재생 속도에 맞춰서)
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

			// 림라이트 종료
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

#pragma endregion 키네틱 콤보 1

#pragma region 슬래시 콤보 2

		.AddState("KINETIC_COMBO_SLASH02")
		.OnStart([&]() 
		{
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
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)


#pragma endregion 슬래시 콤보 2

#pragma region 키네틱 콤보 2

		.AddState("KINETIC_COMBO_KINETIC02_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Kinetic(true);
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

#pragma endregion 키네틱 콤보 2

#pragma region 슬래시 콤보 3

		.AddState("KINETIC_COMBO_SLASH03")
		.OnStart([&]() 
		{
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
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.25f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

#pragma endregion 슬래시 콤보 3

#pragma region 키네틱 콤보 3

		.AddState("KINETIC_COMBO_KINETIC03_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Kinetic(true);
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
			.Predicator([&]()->_bool {return m_bLeftClick; })
			.Priority(0)


#pragma endregion 키네틱 콤보 3

#pragma region 슬래시 콤보 4

		.AddState("KINETIC_COMBO_SLASH04")
		.OnStart([&]() 
		{
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
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
			.Priority(0)

#pragma endregion 슬래시 콤보 4

#pragma region 키네틱 콤보 4

		.AddState("KINETIC_COMBO_KINETIC04_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
				Enemy_Targeting(true);

			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
			{
				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Kinetic(true);
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


#pragma endregion 키네틱 콤보 4

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
				m_pASM->SetCurState("IDLE");
			})
			.AddTransition("JUSTDODGE_NONUSE to JUSTDODGE_USABLE", "JUSTDODGE_USABLE")
			.Predicator([&]()->_bool { return m_fJustDodgeAble > 0.f; })
			.Priority(0)

		.AddState("JUSTDODGE_USABLE")
			.OnStart([&]()
			{
//				CGameInstance::GetInstance()->SetTimeRatioCurve("JustDodge_Income");	// 슬로우
//				CGameInstance::GetInstance()->SetCameraFovCurve("Charge01_ActionCam");
			})
			.Tick([&](double fTimeDelta)
			{
				//IM_LOG("%f", m_fJustDodgeAble);
				//IM_LOG("JUSTDODGE!!");
				// 저스트닷지 조건 발생 시 돌릴 Tick함수
			})
			.OnExit([&]()
			{
				m_pASM->SetCurState("IDLE");

//				CGameInstance::GetInstance()->ReleaseCameraFovCurve();
			})
			
				.AddTransition("JUSTDODGE_USABLE to JUSEDODGE_NONUSE", "JUSTDODGE_NONUSE")
				.Predicator([&]()->_bool{ return m_fJustDodgeAble <= 0.f; })
				.Priority(0)

				.AddTransition("JUSTDODGE_USABLE to JUSTDODGE_SLASH", "JUSTDODGE_SLASH")
				.Predicator([&]()->_bool { return m_bLeftClick; })
				.Priority(0)

		.AddState("JUSTDODGE_SLASH")
			.OnStart([&]()
			{
				m_fJustDodgeAble = 0.f;
				SetAbleState({ false, false, false ,false, true, true, true, true, true, false });
				m_pASM->SetCurState("IDLE");
				m_pASM->AttachAnimSocket("JustDodge_AnimSocket", m_JustDodgeSlash);
			})
			.Tick([&](double fTimeDelta)
			{
				// 저스트닷지 피니시 발생 시 돌릴 Tick함수
			})
			.OnExit([&](){})

				.AddTransition("JUSTDODGE_SLASH to JUSTDODGE_NONUSE", "JUSTDODGE_NONUSE")
				.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("JustDodge_AnimSocket"); })
				.Priority(0)

				.AddTransition("JUSTDODGE_SLASH to JUSTDODGE_NONUSE", "JUSTDODGE_NONUSE")
				.Predicator([&]()->_bool { return m_pASM->isSocketPassby("JustDodge_AnimSocket", 0.5f) && (m_bLeftClick || m_bKineticRB || m_bDash || m_bWalk || m_bJump); })
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

HRESULT CPlayer::SetUp_AttackDesc()
{
	m_mapCollisionEvent.emplace("ATK_A1", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A2", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A3", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A4", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A5", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATTACK_DASH_END", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATTACK_NONCHARGE", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_01", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_02", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_03", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR1", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR2", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR_NONCHARGE", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
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
			m_AttackDesc.iDamage = 100;
			m_AttackDesc.pCauser = this;
			m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		}
		else
		{
			m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
			m_AttackDesc.eAttackType = EAttackType::ATK_DOWN;
			m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
			m_AttackDesc.iDamage = 100;
			m_AttackDesc.pCauser = this;
			m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		}
	});
	m_mapCollisionEvent.emplace("ATK_AIR_CHARGE_FALL", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_DOWN;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATTACK_UPPER_START", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_TO_AIR;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_207_AL_atk_justdodge", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_216_AL_atk_dash1_start", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_217_AL_atk_dash2_end", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_218_AL_atk_dash3_end", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_219_AL_atk_dash4_end", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_214_AL_sas_dash_start_Telepo", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("AS_ch0100_222_AL_atk_air2", [this]()
	{
		m_AttackDesc.eAttackSAS = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 100;
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

			m_pSAS_Cable->SetVisible(true);

			m_fTeleportAttack_GC = 0.f;
		})
		.Tick([&](double fTimeDelta) 
		{
			m_fTeleportAttack_GC += (_float)fTimeDelta;
		})
		.OnExit([&]() 
		{

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

		// 지상 /////////////////////////////
		.AddState("TELEPORTATTACK_FLOOR_START")
		.OnStart([&]() 
		{
			m_pASM->SetCurState("IDLE");
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_pASM->AttachAnimSocket("SAS_Special_AnimSocket", m_Teleport_FloorAttack_Start);

			m_bVisible = false;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(false);

			m_pSAS_Cable->SetVisible(false);

			// 순간이동 위치 잡기
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

			m_pSAS_Cable->SetVisible(true);
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]() 
		{
			m_bVisible = true;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(true);

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

		// 공중 /////////////////////////////
		.AddState("TELEPORTATTACK_AIR_START")
		.OnStart([&]() 
		{
			m_pASM->SetCurState("JUMP_FALL");
			SetAbleState({ false, false, false, false, true, false, true, true, true, false });

			m_pASM->AttachAnimSocket("SAS_Special_AnimSocket", m_Teleport_AirAttack_Start);

			m_bVisible = false;

			for (auto& iter : m_vecWeapon)
				iter->SetVisible(false);

			m_pSAS_Cable->SetVisible(false);

			// 순간이동 위치 잡기
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

			m_pSAS_Cable->SetVisible(true);
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

			m_pSAS_Cable->SetVisible(false);

			//m_fYSpeed = 0.f;

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

			m_pSAS_Cable->SetVisible(true);
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

HRESULT CPlayer::SetUp_TrainStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	// 좌측 차지
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_372_AL_Cap_sLcLeL_start"));
	m_Train_Charge_L.push_back(pAnimation);
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_372_AL_Cap_sLcLeL_loop"));
	m_Train_Charge_L.push_back(pAnimation);
	// 좌측 캔슬
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_372_AL_Cap_sLcLeL_end"));
	m_Train_Cancel_L.push_back(pAnimation);
	// 좌측 던지기
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_372_AL_mg_train_L"));
	m_Train_Throw_L.push_back(pAnimation);

	m_pTrainStateMachine_Left = 
		CFSMComponentBuilder()
		.InitState("TRAIN_LEFT_NOUSE")

		.AddState("TRAIN_LEFT_NOUSE")
		.OnStart([&]() 
		{
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
			m_fKineticCharge += (_float)fTimeDelta;


		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
		})

			.AddTransition("TRAIN_LEFT_CHARGE to TRAIN_LEFT_THROW", "TRAIN_LEFT_THROW")
			.Predicator([&]()->_bool { return m_fKineticCharge >= 2.f; })
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
			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Train_Set_Animation("AS_mg02_372_train_start_L");
			static_cast<CSpecial_Train*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->Activate_Animation(true);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();


			m_pASM->InputAnimSocket("Kinetic_Special_AnimSocket", m_Train_Throw_L);
		})
		.Tick([&](double fTimeDelta)
		{
			
		})
		.OnExit([&]()
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_fKineticCharge = 0.f;
		})

			.AddTransition("TRAIN_LEFT_THROW to TRAIN_LEFT_NOUSE", "TRAIN_LEFT_NOUSE")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket") || 
					( ((m_pASM->isSocketPassby("Kinetic_Special_AnimSocket"), 0.8f) && (m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB))));
			})
			.Priority(0)


		.Build();

	// 차지 / 캔슬 / 동작 3개로 스테이트 분할

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
			if (m_fKineticCharge > 1.f)
			{
				static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
					->TelephonePole_Bend(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), CMathUtils::RandomFloat(0.f, 0.5f));
			}

			m_fKineticCharge += (_float)fTimeDelta;
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
		})
			.AddTransition("TELEPHONEPOLE_LEFT_CHARGE to TELEPHONEPOLE_LEFT_START", "TELEPHONEPOLE_LEFT_START")
			.Predicator([&]()->_bool { return m_fKineticCharge >= 2.f; })
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
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_Start_L);
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecial_TelephonePole*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())
				->TelephonePole_PullOut(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f - m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio());
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
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
		})
		.OnExit([&]()
		{
			CGameInstance::GetInstance()->SetTimeRatioCurve("TelephonePoleSlow");
			m_fKineticCharge = 0.f;
		})
			.AddTransition("TELEPHONEPOLE_LEFT_THROW to TELEPHONEPOLE_LEFT_WAIT", "TELEPHONEPOLE_LEFT_WAIT")
			.Predicator([&]()->_bool 
			{ 
				return (m_pASM->isSocketAlmostFinish("Kinetic_Special_AnimSocket"));
			})
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_WAIT")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_Swing_L);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
		})
		.Tick([&](double fTimeDelta)
		{
			
		})
		.OnExit([&]()
		{
			CGameInstance::GetInstance()->ResetTimeRatio();
			m_fKineticCharge = 0.f;
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
				return (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.05f));
			})
			.Priority(0)


		.AddState("TELEPHONEPOLE_LEFT_END")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_TelephonePole_End_L);
		})
		.Tick([&](double fTimeDelta)
		{
			
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
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
		})
		.OnExit([&]()
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_fKineticCharge = 0.f;
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
	m_BrainCrash_Activate.push_back(m_pModel->Find_Animation("AS_BC_em_common_ch0100_end"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_BrainCrash_Cutin"));
	m_BrainCrash_CutScene.push_back(m_pModel->Find_Animation("AS_ch0100_BrainCrash_Cutin"));

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

		})
			.AddTransition("BRAINCRASH_NOUSE to BRAINCRASH_CUTSCENE", "BRAINCRASH_CUTSCENE")
			.Predicator([&]()->_bool { return CGameInstance::GetInstance()->KeyDown(DIK_H); })
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

		})
			.AddTransition("BRAINCRASH_CUTSCENE to BRAINCRASH_ACTIVATE", "BRAINCRASH_ACTIVATE")
			.Predicator([&]()->_bool { return m_pSasPortrait->isFinish(); })
			.Priority(0)

		.AddState("BRAINCRASH_ACTIVATE")
		.OnStart([&]()
		{
			m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_BrainCrash_Activate);
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]()
		{

		})
			.AddTransition("BRAINCRASH_ACTIVATE to BRAINCRASH_NOUSE", "BRAINCRASH_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("BrainCrash_AnimSocket", 0.7f); })
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

	m_pHBeamStateMachine_Left =
		CFSMComponentBuilder()
		.InitState("HBEAM_LEFT_NOUSE")

		.AddState("HBEAM_LEFT_NOUSE")
		.OnStart([&]()
		{
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
			.AddTransition("HBEAM_LEFT_NOUSE to HBEAM_LEFT_CHARGE", "HBEAM_LEFT_CHARGE")
			.Predicator([&]()->_bool 
			{
				_bool bResult = (nullptr != CPlayerInfoManager::GetInstance()->Get_SpecialObject());
				return m_bKineticG && bResult;
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

			m_fKineticCharge += (_float)fTimeDelta;
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
		})
			.AddTransition("HBEAM_LEFT_CHARGE to HBEAM_LEFT_THROW", "HBEAM_LEFT_THROW")
			.Predicator([&]()->_bool { return m_fKineticCharge >= 2.f; })
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
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_Throw_L);
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
			CGameInstance::GetInstance()->SetTimeRatioCurve("TelephonePoleSlow");
			m_fKineticCharge = 0.f;
		})
			.AddTransition("HBEAM_LEFT_THROW to HBEAM_LEFT_WAIT", "HBEAM_LEFT_WAIT")
			.Predicator([&]()->_bool 
			{ 
				return (false == static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_isDecomposed());
			})
			.Priority(0)

		.AddState("HBEAM_LEFT_WAIT")
		.OnStart([&]() 
		{
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_Rotation_L);
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
		})
		.Tick([&](double fTimeDelta)
		{
			
		})
		.OnExit([&]()
		{
			CGameInstance::GetInstance()->ResetTimeRatio();
			m_fKineticCharge = 0.f;
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
				return (0.05f <= m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetPlayRatio());
			})
			.Priority(0)


		.AddState("HBEAM_LEFT_END")
		.OnStart([&]() 
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_pASM->AttachAnimSocket("Kinetic_Special_AnimSocket", m_HBeam_End_L);
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_SetKinetic(true);
		})
		.Tick([&](double fTimeDelta)
		{
			static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_Finish();
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
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
		})
		.OnExit([&]()
		{
			m_fKineticCharge = 0.f;
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
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pASM->isSocketPassby("Kinetic_Special_AnimSocket", 0.2f))
				static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_Finish();
			else
				static_cast<CSpecial_HBeam_Bundle*>(CPlayerInfoManager::GetInstance()->Get_SpecialObject())->HBeam_Single_Turn();
		})
		.OnExit([&]()
		{
			static_cast<CCamSpot*>(m_pCamSpot)->Switch_CamMod();
			m_fKineticCharge = 0.f;
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

	if (szCurAnim.find("AS_ch0100_2") != string::npos)
	{
		m_bOnBattle = true;
		return true;
	}

	if (szCurAnim.find("AS_ch0100_3") != string::npos)
	{
		m_bOnBattle = true;
		return true;
	}

	if (szCurAnim.find("AS_ch0100_4") != string::npos)
	{
		m_bOnBattle = true;
		return true;
	}

	if (szCurAnim.find("AS_ch0100_5") != string::npos)
	{
		m_bOnBattle = true;
		return true;
	}

	if (!CPlayerInfoManager::GetInstance()->Get_PlayerSasList().empty())
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
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Default_Attack_Charge_Effect_01"))->Start_Attach(this, "RightWeapon");
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Charge_White_Particle"))->Start_Attach(this, "RightWeapon");

			if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
			{
				_matrix EffectPivot = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Default_Charge_1_MeshParticle"))->Start_AttachPivot(this, EffectPivot, "RightWeapon", true, false);
			}
			else if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			{
				_matrix EffectPivot = XMMatrixScaling(5.f, 5.f, 5.f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Fire_Charge_1_MeshParticle"))->Start_AttachPivot(this, EffectPivot, "RightWeapon", true, false);
			}

		}
		if ((1 == iNum) && fCharge <= m_fCharge[iNum])
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Default_Attack_Charge_Effect_02"))->Start_Attach(this, "RightWeapon");
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Charge_White_Particle"))->Start_Attach(this, "RightWeapon");
			
			if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_NOT)
			{
				_matrix EffectPivot = XMMatrixScaling(6.f, 6.f, 6.f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(-20.f)) * XMMatrixTranslation(0.f, 0.4f, -0.5f);
				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Default_Charge_2_MeshParticle"))->Start_AttachPivot(this, EffectPivot, "RightWeapon", true, false);
			}
			else if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type == ESASType::SAS_FIRE)
			{
				_matrix EffectPivot = XMMatrixScaling(5.f, 5.f, 5.f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Fire_Charge_2_MeshParticle"))->Start_AttachPivot(this, EffectPivot, "RightWeapon", true, false);
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
	CEffectGroup* pEffect = nullptr;
	Json Effect;

	wstring EffectName = s2ws(szEffectName);

	switch (CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type)
	{
	case ESASType::SAS_NOT:
		if (szEffectName.find("Fire") != string::npos || szEffectName.find("Elec") != string::npos)
			break;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, EffectName)->Start_Attach(this, szBoneName);
		break;
	case ESASType::SAS_FIRE:
		if (szEffectName.find("Fire") == string::npos)
			break;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, EffectName)->Start_Attach(this, szBoneName);
		break;
	case ESASType::SAS_ELETRIC:
		if (szEffectName.find("Elec") == string::npos)
			break;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, EffectName)->Start_Attach(this, szBoneName);
		break;
	}

	if (pEffect == nullptr)
		return;
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

void CPlayer::Event_CollisionStart()
{
	m_bAttackEnable = true;

	static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, true);

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
	static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, false);

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
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Throw_Kinetic_Distortion"))->Start_AttachOnlyPos(vKineticPos);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Kinetic_Shoot_Smoke"))->Start_AttachPosition(this, vKineticPos, _float4(0.f, 1.f, 0.f, 0.f));

		//Vector4 vTargetPos = static_cast<CMonster*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetKineticTargetPos();
		Vector4 vTargetPos = static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->GetKineticTargetPos();
		Vector4 vMyPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		Vector4 vDir = vTargetPos - vMyPos;
		vDir.w = 0.f;
		vDir = XMVector3Normalize(vDir);

		_float3 vForce = { vDir.x, vDir.y, vDir.z };
		vForce *= m_fThrowPower * g_fTimeDelta;
		static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Add_Physical(vForce);
		static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->SetThrow();

		m_SoundStore.PlaySound("fx_kinetic_shot", m_pTransformCom);
	}
	else if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		_vector vKineticPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Throw_Kinetic_Distortion"))->Start_AttachOnlyPos(vKineticPos);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Kinetic_Shoot_Smoke"))->Start_AttachPosition(this, vKineticPos, _float4(0.f, 1.f, 0.f, 0.f));

		_float3 vForce = { XMVectorGetX(m_vCamLook), XMVectorGetY(m_vCamLook), XMVectorGetZ(m_vCamLook) };
		vForce = XMVector3Normalize(vForce);
		vForce *= m_fThrowPower * g_fTimeDelta;
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
	static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(bTrail);
}

void CPlayer::Event_Dust()
{
	//CVFX_Manager::GetInstance()->GetParticle(PARTICLE::)
}

void CPlayer::Event_KineticCircleEffect()
{
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Kinetic_BaseCircle")->Start_Attach(this, "Eff01");
	_matrix MatScale = XMMatrixIdentity() * XMMatrixScaling(10.f, 10.f, 10.f);
//	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, L"Player_Kinetic_Particle")->Start_AttachPivot(this, MatScale, "Reference", true, false);
}

void CPlayer::Event_KineticCircleEffect_Attach()
{
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Kinetic_BaseCircle")->Start_Attach(this, "Eff01", true);
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

	m_bBreakFall = m_pController->KeyDown(CController::SPACE);

	if (m_bHit)
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

	m_vCamLook = pGameInstance->Get_CamLook();
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

			if (m_bAir || m_bHit)	// 공중 상태에선 회전이 먹지 않음
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
	if (m_bHit)		// 맞으면 아무것도 못하게 해야한다
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
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(false);
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, false);
		Event_collisionEnd();
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

	// ??되나이거
	if (!m_pASM->isSocketEmpty("Kinetic_Special_AnimSocket"))
	{
		string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Special_AnimSocket")->GetName();
		_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		//IM_LOG("%f %f %f", XMVectorGetX(vLocal), XMVectorGetY(vLocal), XMVectorGetZ(vLocal));
		m_pTransformCom->LocalMove(vLocal);
	}

	if (!m_pASM->isSocketEmpty("BrainCrash_AnimSocket"))
	{
		string szCurAnimName = m_pASM->GetSocketAnimation("BrainCrash_AnimSocket")->GetName();
		_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		m_pTransformCom->LocalMove(vLocal);
	}

	if (!m_pASM->isSocketEmpty("SAS_Special_AnimSocket"))
	{
		string szCurAnimName = m_pASM->GetSocketAnimation("SAS_Special_AnimSocket")->GetName();
		_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		m_pTransformCom->LocalMove(vLocal);

		_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
		m_pTransformCom->LocalMove(vOpTest);
	}
}

void CPlayer::Update_NotiveNeon()
{	
	// 체력이 10% 이하일때 생성
	static _bool bCreate = false;
	if (false == bCreate && 0.1f > _float(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP) / CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP)
	{
		bCreate = true;

		Json json;
		json["NoticeNeon"] = "NoticeNeon_HP";
		m_pNeonUI = dynamic_cast<CNoticeNeonUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_NoticeNeonUI"), &json));
		//m_pNeonUI->SetOwner(this);
	}

	// 삭제
	if (nullptr != m_pNeonUI)
	{
		if (true == m_pNeonUI->doKill())
		{
			m_pNeonUI->SetDelete();
		}
	}

	// 리셋
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
	//타겟이 사라졌을때, 쓰레기값이 들어가있어서 한번 검사를 시켜줌
	if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster() != nullptr)
		Enemy_Targeting(true);

	if (m_pSettedTarget != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		//원래 타겟이 없다가 생긴 경우
		if (m_pSettedTarget == nullptr && CPlayerInfoManager::GetInstance()->Get_TargetedMonster() != nullptr)
		{
			m_pUI_LockOn = dynamic_cast<CMonsterLockonUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterLockon")));
			assert(m_pUI_LockOn != nullptr);
			m_pUI_LockOn->Set_Owner(CPlayerInfoManager::GetInstance()->Get_TargetedMonster());
				
		}

		//원래 타겟이 있었는데 사라진 경우
		else if (m_pSettedTarget != nullptr && CPlayerInfoManager::GetInstance()->Get_TargetedMonster() == nullptr)
		{
			m_pUI_LockOn->SetDelete();
		}

		//다른 타겟으로 바뀐 경우
		else if (m_pSettedTarget != nullptr && CPlayerInfoManager::GetInstance()->Get_TargetedMonster() != nullptr)
		{
			m_pUI_LockOn->SetDelete();

			m_pUI_LockOn = dynamic_cast<CMonsterLockonUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterLockon")));
			assert(m_pUI_LockOn != nullptr);
			m_pUI_LockOn->Set_Owner(CPlayerInfoManager::GetInstance()->Get_TargetedMonster());
			//m_pUI_LockOn->Set_UIPivotMatrix(dynamic_cast<CMonster*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->Get_UIPivotMatrix(FINDEYES));
			//m_pUI_LockOn->Set_UIPivotMatrix(dynamic_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->Get_UIPivotMatrix(FINDEYES));
		}

		//info bar 설정
		if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster() != nullptr)
			Create_TargetInfoBar(CPlayerInfoManager::GetInstance()->Get_TargetedMonster());
	
		m_pSettedTarget = CPlayerInfoManager::GetInstance()->Get_TargetedMonster();
	}

}

void CPlayer::Create_TargetInfoBar(CGameObject* pTarget)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CMonster* pMonster = dynamic_cast<CMonster*>(pTarget);
	if (pMonster == nullptr) return;
	
	//몬스터 info바가 없으면 생성
	if (pMonster->GetHasName() == true)
		return;

	CMonsterHpUI* pUI_HP = nullptr;
	pUI_HP = dynamic_cast<CMonsterHpUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterHP")));

	assert(pUI_HP != nullptr);
	pUI_HP->Set_Owner(pTarget);

	_float4x4 PivotMatrix = pMonster->Get_UIPivotMatrix(INFOBAR);
	pUI_HP->SetPivotMatrix(PivotMatrix);

	_int iLevel = pMonster->Get_MonsterLevel();
	_int iName = pMonster->Get_MonsterName();
	pUI_HP->Set_MonsterInfo(iLevel, iName);

	pMonster->Set_HasName();
}

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

	if (m_bOnBattle)	// 전투 중 파티클 생성
	{
		m_fBattleParticleTime += (_float)TimeDelta;

		if (1.f < m_fBattleParticleTime)
		{
			m_fBattleParticleTime = 0.f;
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, TEXT("Player_Text_Particle_NoLoop"))
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

	Json Weapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/wp0190.json");
	Weapon["Model"] = "../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model";

	CGameObject*	pGameObject = nullptr;

	WEAPON_DESC		Desc;
	ZeroMemory(&Desc, sizeof(WEAPON_DESC));
	Desc.m_PivotMatrix = m_pModel->GetPivotMatrix();
	//Desc.m_pSocket = m_pModel->Get_BonePtr("RightWeapon");
	//Desc.m_pTransform = m_pTransformCom;
	Desc.m_pJson = &Weapon;

//	pGameInstance->Clone_GameObject(TEXT("Layer_Player"), TEXT("PlayerWeapon"), &Desc);

	pGameObject = pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("PlayerWeapon"), &Desc);
	m_vecWeapon.push_back(pGameObject);

	return S_OK;
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
		list<CGameObject*>	DistanceList;
		for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
		{
			//if ((!static_cast<CMonster*>(iter)->IsDead()) && (!static_cast<CMonster*>(iter)->GetInvisible()))
			if ((!static_cast<CEnemy*>(iter)->IsDead()))
			{
				_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float	fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

				if (fDistance > fCurDistance)
				{
					DistanceList.push_front(iter);
					fDistance = fCurDistance;
				}
			}
		}

		if (bNear)
		{
			if (DistanceList.empty())
				return;

			CPlayerInfoManager::GetInstance()->Set_TargetedMonster(DistanceList.front());
		}
		else if (!bNear)
		{
			if (DistanceList.empty())
				return;

			_bool bChange = false;
			for (auto& iter : DistanceList)
			{
				if (bChange)
				{
					CPlayerInfoManager::GetInstance()->Set_TargetedMonster(iter);
					return;
				}
				else
				{
					if (iter == pBeforeEnemy)
					{
						bChange = true;
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
		_float fDistance = 20.f;

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

	//if (1.f > m_fSwingLerpTimer)	// 빙 돌면서 나한테까지 끌어오는 과정, 1초 안에 이뤄짐
	//{
	//	static _vector vBeforePos;

	//	_float fScalar = XMVectorGetX(XMVector3Length(m_vToKineticObj)) - XMVectorGetX(XMVector3Length(m_vKineticPoint));
	//	fScalar *= (1.f - m_fSwingLerpTimer);

	//	_float fAngle = XMConvertToRadians(180.f);

	//	_vector vVector = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	//	// 좌우 체크
	//	_vector vMyright = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	//	if (0.f > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyright), XMVector3Normalize(m_vToKineticObj))))
	//	{
	//		// 왼쪽
	//		vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
	//		vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(-(fAngle * m_fSwingLerpTimer)));
	//		m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
	//	}
	//	else
	//	{
	//		// 오른쪽
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
	//else // 나한테까지 오고 나서 부터의 빙빙 회전과 적으로 날아감
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
	//			// 바라보며 우측으로 이동
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
	//			// 바라보며 좌측으로 이동
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

	//	// 좌우 체크
	//	_vector vMyright = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	//	if (0.f > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyright), XMVector3Normalize(m_vToKineticObj))))
	//	{
	//		// 왼쪽
	//		vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
	//		vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(-(fAngle * m_fSwingLerpTimer)));
	//		m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
	//		m_bRight = true;	// 플레이어 바라보는 축 기준 오른쪽 축으로 도는가?
	//	}
	//	else
	//	{
	//		// 오른쪽
	//		vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
	//		vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(fAngle * m_fSwingLerpTimer));
	//		m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
	//		m_bRight = false;
	//	}

	//	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//	m_pKineticObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vMyPos + vVector);

	//}
	//else // 나한테까지 오고 나서 부터의 빙빙 회전과 적으로 날아감
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
	// 몬스터를 바라보는 작업은 여기에서 일어나며, 실제 이동해야 하는 위치의 위치벡터를 생성한다.

	if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster() && 
		nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
	{
		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones(0.f, EAnimUpdateType::NORMAL);
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();

		_vector vDiff = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - SocketMatrix.r[3];	// 회전하기 전 물체 본과 실제 물체 사이의 방향벡터
		_vector vRePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + vDiff; // 물체 사이의 거리만큼 이동한 위치
		_vector vTmpLookAtMonster = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - vRePos; // 예비 이동 위치에서 몬스터를 바라본 방향벡터

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector vLookPoint = vMyPos + vTmpLookAtMonster;
		vLookPoint = XMVector3Normalize(vLookPoint);
		vLookPoint = XMVectorSetW(vLookPoint, 1.f);
//		m_pTransformCom->LookAt_NonY(vLookPoint);	// 예비 Look벡터로 바라보는 방향을 바꿈

		_matrix ModifiedMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();
		_vector vRenewalDiff = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - ModifiedMatrix.r[3];
		_vector vRenewalRePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + vRenewalDiff;	// 이동해야 하는 최종 위치

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

void CPlayer::SasGearEffect()
{
	_float4x4 Pivot01 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.17f, 0.09f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"))
		->Start_AttachPivot(this, Pivot01, "Sheath", true, true);

	_float4x4 Pivot02 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(-0.17f, 0.1f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"))
		->Start_AttachPivot(this, Pivot02, "Sheath", true, true);

	_float4x4 Pivot03 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.1f, -0.1f, 0.2f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"))
		->Start_AttachPivot(this, Pivot03, "Sheath", true, true);

	_float4x4 Pivot04 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(0.08f, -0.3f, 0.08f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"))
		->Start_AttachPivot(this, Pivot04, "Sheath", true, true);

	_float4x4 Pivot05 = XMMatrixScaling(0.1f, 0.1f, 0.1f)
		* XMMatrixRotationX(XMConvertToRadians(-180.f))
		* XMMatrixRotationZ(XMConvertToRadians(-180.f))
		* XMMatrixTranslation(-0.172f, -0.274f, 0.045f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, TEXT("Use_Sas_Gear"))
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

	Safe_Release(m_pKineticComboStateMachine);
	Safe_Release(m_pKineticStataMachine);
	Safe_Release(m_pHitStateMachine);
	Safe_Release(m_pJustDodgeStateMachine);

	Safe_Release(m_pASM);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pController);
	Safe_Release(m_pPlayerCam);

	Safe_Release(m_pKineticAnimModel);

	Safe_Release(m_pCurve);
	Safe_Release(m_pRange);

	Safe_Release(m_pTrainStateMachine_Left);
	Safe_Release(m_pTelephonePoleStateMachine_Left);
	Safe_Release(m_pBrainCrashStateMachine);
	Safe_Release(m_pHBeamStateMachine_Left);
	Safe_Release(m_pTeleportStateMachine);
	Safe_Release(m_pSAS_Cable);
	Safe_Release(m_pTrail);

//	Safe_Release(m_pContectRigidBody);
}
