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
#include "MonsterLockonUI.h"

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

	if (FAILED(SetUp_Event()))	//-> 이건 진짜 천천히 보자...
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

	//Load_DefaultEffects("../Bin/Resources/Curve/Default_Attack/");
	//Load_DefaultEffects("../Bin/Resources/Curve/Fire_Attack/");

	Load_Effect("../Bin/Resources/Curve/Default_Attack/");
	Load_Effect("../Bin/Resources/Curve/Fire_Attack/");

	ZeroMemory(&m_PlayerStat, sizeof(PLAYER_STAT));
	m_PlayerStat.m_iHP = 1000;
	m_PlayerStat.m_iMaxHP = 1000;
	m_PlayerStat.m_iKineticEnergy = 100;
	m_PlayerStat.m_iMaxKineticEnergy = 100;
	m_PlayerStat.m_iKineticEnergyLevel = 0;
	m_PlayerStat.m_iKineticEnergyType = 2;
	ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 1.f, 0.f, 0.f));


	m_pTransformCom->SetTransformDesc({ 5.f, XMConvertToRadians(720.f) });

	m_pPlayerCam = m_pGameInstance->Add_Camera("PlayerCamera", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Player");
	Safe_AddRef(m_pPlayerCam);

	Initalize_Sas();

	//CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	//Json ScifiEffect = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_DefaultAttack_1.json");
	//m_pEffect = pGameInstance->Clone_GameObject_Get(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &ScifiEffect);
	
		/*Json ScifiEffect = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_DefaultAttack_1.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &ScifiEffect);*/
	
	//m_pPlayerCam = dynamic_cast<CCamera*>(m_pGameInstance->Clone_GameObject_Get(L"Layer_Camera", TEXT("Prototype_GameObject_Camera_Player")));
	//Assert(m_pPlayerCam != nullptr);
	//Safe_AddRef(m_pPlayerCam);

	return S_OK;
}

void CPlayer::BeginTick()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 2.f, 0.f, 1.f));
	__super::BeginTick();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Player")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == TEXT("CamSpot"))
		{
			m_pCamSpot = static_cast<CCamSpot*>(iter);
		}
	}


	// 테스트
	//m_pKineticAnimModel->SetPlayAnimation("AS_no0000_271_AL_Pcon_cReL_Lv4");

	// 위치 지정은 여기에서
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (nullptr != m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
	{
		//IM_LOG(ws2s(m_pTargetedEnemy->GetPrototypeTag()).c_str());
		if (m_pUI_LockOn == nullptr)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			m_pUI_LockOn = dynamic_cast<CMonsterLockonUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterLockon")));
			assert(m_pUI_LockOn != nullptr);
			m_pUI_LockOn->Set_Owner(m_pTargetedEnemy);
		}
	}

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
	//if (CGameInstance::GetInstance()->KeyDown(DIK_E))
	//{
	//	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0650_Bullet_Birth")->Start_AttachPosition(this, _float4(0.f, 0.f, 0.f, 1.f), _float4(0.f, 1.f, 0.f, 0.f));
	//}

	 if (m_pPlayerCam->IsMainCamera())
		m_pController->Tick(TimeDelta);
	 else
		 m_pController->Invalidate();

	MoveStateCheck(TimeDelta);
	BehaviorCheck(TimeDelta);

	m_pHitStateMachine->Tick(TimeDelta);

	HitCheck();

	if(!m_bHit)
		m_pJustDodgeStateMachine->Tick(TimeDelta);

	if (!m_bHit && (0.f > m_fKineticCombo_Slash) && (false == m_bKineticCombo)) // 콤보 타이밍이 아닐 때에는 일반 염력
	{
		m_pKineticStataMachine->Tick(TimeDelta);
	}
	else if (!m_bHit) // 콤보 타이밍에는 콤보 염력
	{
		m_pKineticComboStateMachine->Tick(TimeDelta);
		m_pKineticStataMachine->SetState("NO_USE_KINETIC");
	}
	else
	{
		m_pKineticComboStateMachine->SetState("KINETIC_COMBO_NOUSE");
		m_pKineticStataMachine->SetState("NO_USE_KINETIC");
		m_pJustDodgeStateMachine->SetState("JUSTDODGE_NONUSE");
	}

	//IM_LOG(m_pKineticComboStateMachine->GetCurStateName());

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
				m_fKineticCombo_Slash = 1.f;

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

	if (pGameInstance->KeyDown(DIK_K))
	{
		list<CAnimation*> TestAnim;
		TestAnim.push_back(m_pModel->Find_Animation("AS_ch0100_207_AL_atk_justdodge"));
		m_pASM->InputAnimSocket("Common_AnimSocket", TestAnim);
	}

	if (pGameInstance->KeyPressing(DIK_L))
	{
		//Kinetic_Combo_Test();
	}

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

	if (m_bVisible && (nullptr != m_pRenderer))
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
}

void CPlayer::AfterPhysX()
{
	__super::AfterPhysX();

	for (auto& iter : m_vecWeapon)
	{
		static_cast<CScarletWeapon*>(iter)->Setup_BoneMatrix(m_pModel, m_pTransformCom->Get_WorldMatrix());
	}

	m_pCamSpot->SetUp_BoneMatrix(m_pModel, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

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
	//else
	//{
	//	m_bHit = true;

	//	m_DamageDesc.m_iDamage = tDamageParams.iDamage;
	//	m_DamageDesc.m_iDamageType = tDamageParams.eAttackType;
	//	m_DamageDesc.m_vHitDir = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - XMLoadFloat3(&tDamageParams.vHitFrom);
	//	m_DamageDesc.m_eHitDir = CClientUtils::GetDamageFromAxis(m_pTransformCom, XMLoadFloat3(&tDamageParams.vHitFrom));

	//	// 체력 깎이는 부분
	//	m_PlayerStat.m_iHP -= tDamageParams.iDamage;

	//	if (tDamageParams.eAttackType == EAttackType::ATK_HEAVY || tDamageParams.eAttackType == EAttackType::ATK_TO_AIR)
	//	{
	//		m_pTransformCom->LookAt_NonY(tDamageParams.pCauser->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
	//	}
	//}
}

void CPlayer::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	// HP Bar Check	
	ImGui::Text("HP : %d", m_PlayerStat.m_iHP);

	m_pKineticStataMachine->Imgui_RenderProperty();

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
		static char cEffectName[MAX_PATH]{};
		static string szEffectName = "";

		static char cAninName[MAX_PATH]{};
		static string szAnimName = "";
		static _float fEffectPlayTime = 0.f;
		static _float fEffectSize = 1.f;
		static char cBoneName[MAX_PATH]{ "Eff01" };

		static _float fPrePlayTime = 0.f;
		_float fCurPlayTime = m_pModel->GetPlayAnimation()->GetPlayTime();

		if (ImGui::BeginListBox("Animation List"))
		{
			static char szSeachAnim[MAX_PATH] = "";
			ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

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

		if (ImGui::BeginListBox("Default Effect List"))
		{
			static char szSeachDefault[MAX_PATH] = "";
			ImGui::InputText("Default Effect Search", szSeachDefault, MAX_PATH);

			const string strSearch = szSeachDefault;
			const _bool bSearch = strSearch.empty() == false;

			
			for (auto& Pair : m_mapDefaultEffect)
			{
				if (bSearch)
				{
					if (Pair.first.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = szEffectName == Pair.first;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(Pair.first.c_str(), bSelected))
					szEffectName = Pair.first;
			}
			ImGui::EndListBox();
		}

		if (ImGui::BeginListBox("Fire Effect List"))
		{
			static char szSeachFire[MAX_PATH] = "";
			ImGui::InputText("Fire Effect Search", szSeachFire, MAX_PATH);

			const string strSearch = szSeachFire;
			const _bool bSearch = strSearch.empty() == false;

			for (auto& Pair : m_mapFireEffect)
			{
				if (bSearch)
				{
					if (Pair.first.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = szEffectName == Pair.first;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(Pair.first.c_str(), bSelected))
					szEffectName = Pair.first;
			}
			ImGui::EndListBox();
		}

		ImGui::InputFloat("Effect_Size", &fEffectSize);
		ImGui::InputText("Effect_Bone", cBoneName, MAX_PATH);
		ImGui::InputFloat("Effect_Time", &fEffectPlayTime);

		if ((m_pModel->GetPlayAnimation()->GetName() == szAnimName) &&
			(fPrePlayTime <= fEffectPlayTime) &&
			(fCurPlayTime > fEffectPlayTime))
		{
			Event_Effect(szEffectName, fEffectSize, cBoneName);
		}

		fPrePlayTime = fCurPlayTime;

		if("" != szAnimName)
			m_pModel->Get_AnimList()[szAnimName]->Imgui_RenderProperty();
	}
}

CGameObject * CPlayer::Get_TargetedEnemy()
{
	if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
	{
		return nullptr;
	}

	return m_pTargetedEnemy;
}

void CPlayer::CamBoneTest()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_7))
	{
		list<CAnimation*>	TestList;
		TestList.push_back(m_pModel->Find_Animation("AS_ch0100_271_AL_Pcon_cReL_Lv4"));

		m_pASM->InputAnimSocket("JustDodge_AnimSocket", TestList);

		m_pCamSpot->Switch_CamMod();
	}
	else if (CGameInstance::GetInstance()->KeyDown(DIK_8))
	{
		m_pCamSpot->Switch_CamMod();
	}
}

void CPlayer::Initalize_Sas()
{
	m_PlayerSasType = ESASType::SAS_END;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].UseRate = 1.f;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].bUsable = true;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].UseRate = 1.f;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].bUsable = true;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].UseRate = 1.f;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].bUsable = true;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].UseRate = 1.f;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].bUsable = true;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].UseRate = 1.f;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].bUsable = true;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].UseRate = 1.f;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].bUsable = true;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].UseRate = 1.f;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].bUsable = true;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].UseRate = 1.f;

	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].MaxEnergy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].MinEnergy = 20.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].bUsable = true;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].Energy = 60.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].RecoveryRate = 1.f;
	m_PlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].UseRate = 1.f;
}

void CPlayer::SasMgr()
{
	ESASType InputSas = ESASType::SAS_END;
	if (CGameInstance::GetInstance()->KeyDown(DIK_1))	InputSas = ESASType::SAS_TELEPORT;
	if (CGameInstance::GetInstance()->KeyDown(DIK_2))	InputSas = ESASType::SAS_PENETRATE;
	if (CGameInstance::GetInstance()->KeyDown(DIK_3))	InputSas = ESASType::SAS_HARDBODY;
	if (CGameInstance::GetInstance()->KeyDown(DIK_4))	InputSas = ESASType::SAS_FIRE;
	if (CGameInstance::GetInstance()->KeyDown(DIK_5))	InputSas = ESASType::SAS_SUPERSPEED;
	if (CGameInstance::GetInstance()->KeyDown(DIK_6))	InputSas = ESASType::SAS_COPY;
	if (CGameInstance::GetInstance()->KeyDown(DIK_7))	InputSas = ESASType::SAS_INVISIBLE;
	if (CGameInstance::GetInstance()->KeyDown(DIK_8))	InputSas = ESASType::SAS_ELETRIC;

	if (InputSas != ESASType::SAS_END)
	{
		if (!m_PlayerStat.Sasese[static_cast<_uint>(InputSas)].bUsable)
		{
			IM_LOG("Sas_Unusable");
		}
		else
		{
			if (m_PlayerSasType != InputSas)
			{
				m_pASM->SetCurState("IDLE");
				SetAbleState({ false, false, false, false, false, true, true, true, true, false });

				list<CAnimation*> SasDamage;
				SasDamage.push_back(m_pModel->Find_Animation("AS_ch0100_410_AL_damage_sas"));
				m_pASM->InputAnimSocket("Common_AnimSocket", SasDamage);
				m_PlayerSasType = InputSas;
			}
			else if (m_PlayerSasType == InputSas)
			{
				IM_LOG("Sas End");

				m_PlayerSasType = ESASType::SAS_END;

				if (m_PlayerStat.Sasese[static_cast<_uint>(InputSas)].MinEnergy > m_PlayerStat.Sasese[static_cast<_uint>(InputSas)].Energy)
				{
					m_PlayerStat.Sasese[static_cast<_uint>(InputSas)].bUsable = false;
				}
			}
		}
	}

	for (_uint i = 0; i < SAS_CNT; ++i)
	{
		if (i == static_cast<_uint>(m_PlayerSasType))
		{
			// 현재 사용중인 SAS

			m_PlayerStat.Sasese[i].Energy -= (g_fTimeDelta * m_PlayerStat.Sasese[i].UseRate);

			if (0.f >= m_PlayerStat.Sasese[i].Energy)
			{
				m_PlayerSasType = ESASType::SAS_END;
				m_PlayerStat.Sasese[i].bUsable = false;
			}
		}
		else
		{
			m_PlayerStat.Sasese[i].Energy += (g_fTimeDelta * m_PlayerStat.Sasese[i].RecoveryRate);

			if (m_PlayerStat.Sasese[i].MaxEnergy <= m_PlayerStat.Sasese[i].Energy)
			{
				m_PlayerStat.Sasese[i].Energy = m_PlayerStat.Sasese[i].MaxEnergy;
			}

			if (m_PlayerStat.Sasese[i].MinEnergy <= m_PlayerStat.Sasese[i].Energy)
			{
				m_PlayerStat.Sasese[i].bUsable = true;
			}
		}
	}

//	if (m_PlayerSasType != ESASType::SAS_END)
//		IM_LOG("%f", m_PlayerStat.Sasese[static_cast<_uint>(m_PlayerSasType)].Energy);
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
	m_pModel->Add_EventCaller("Default_Attack_5", [&]() {Event_Effect("Default_Attack_5"); });
	m_pModel->Add_EventCaller("Default_Attack_5_BackGround", [&]() {Event_Effect("Default_Attack_5_BackGround"); });
	m_pModel->Add_EventCaller("Default_Attack_5_Final", [&]() {Event_Effect("Default_Attack_5_Final"); });
	m_pModel->Add_EventCaller("Default_Attack_5_PreEffect", [&]() {Event_Effect("Default_Attack_5_PreEffect"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Attack_1", [&]() {Event_Effect("Default_Attack_Air_Attack_1"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Attack_2", [&]() {Event_Effect("Default_Attack_Air_Attack_2"); });
	m_pModel->Add_EventCaller("Default_Attack_Air_Attack_Chase", [&]() {Event_Effect("Default_Attack_Air_Attack_Chase"); });
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
	m_pModel->Add_EventCaller("Default_Attack_Dash_2", [&]() {Event_Effect("Default_Attack_Dash_2"); });				// 2랑 3 순서 바뀜
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
	m_pModel->Add_EventCaller("Fire_Attack_1", [&]() {Event_Effect("Fire_Attack_1"); });
	m_pModel->Add_EventCaller("Fire_Attack_1_001", [&]() {Event_Effect("Fire_Attack_1_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_2", [&]() {Event_Effect("Fire_Attack_2"); });
	m_pModel->Add_EventCaller("Fire_Attack_3", [&]() {Event_Effect("Fire_Attack_3"); });
	m_pModel->Add_EventCaller("Fire_Attack_3_Double_twist", [&]() {Event_Effect("Fire_Attack_3_Double_twist"); });
	m_pModel->Add_EventCaller("Fire_Attack_3_twist", [&]() {Event_Effect("Fire_Attack_3_twist"); });
	m_pModel->Add_EventCaller("Fire_Attack_4", [&]() {Event_Effect("Fire_Attack_4"); });
	m_pModel->Add_EventCaller("Fire_Attack_4_001", [&]() {Event_Effect("Fire_Attack_4_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_5", [&]() {Event_Effect("Fire_Attack_5"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_1", [&]() {Event_Effect("Fire_Attack_Air_1"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_2", [&]() {Event_Effect("Fire_Attack_Air_2"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_Chase", [&]() {Event_Effect("Fire_Attack_Air_Chase"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_0", [&]() {Event_Effect("Fire_Attack_Air_dash_0"); });
	m_pModel->Add_EventCaller("Fire_AttacK_Air_dash_0_001", [&]() {Event_Effect("Fire_AttacK_Air_dash_0_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1", [&]() {Event_Effect("Fire_Attack_Air_dash_1"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1_001", [&]() {Event_Effect("Fire_Attack_Air_dash_1_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_dash_1_002", [&]() {Event_Effect("Fire_Attack_Air_dash_1_002"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold", [&]() {Event_Effect("Fire_Attack_Air_Hold"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_1", [&]() {Event_Effect("Fire_Attack_Air_Hold_1"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_2", [&]() {Event_Effect("Fire_Attack_Air_Hold_2"); });
	m_pModel->Add_EventCaller("Fire_Attack_Air_Hold_Landing", [&]() {Event_Effect("Fire_Attack_Air_Hold_Landing"); });
	m_pModel->Add_EventCaller("Fire_Attack_Charge03", [&]() {Event_Effect("Fire_Attack_Charge03"); });
	m_pModel->Add_EventCaller("Fire_Attack_Charge03_001", [&]() {Event_Effect("Fire_Attack_Charge03_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_Charge03_002", [&]() {Event_Effect("Fire_Attack_Charge03_002"); });
	m_pModel->Add_EventCaller("Fire_Attack_Dash_00", [&]() {Event_Effect("Fire_Attack_Dash_00"); });
	m_pModel->Add_EventCaller("Fire_Attack_Dash_01", [&]() {Event_Effect("Fire_Attack_Dash_01"); });
	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_001", [&]() {Event_Effect("Fire_Attack_Dash_02_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_Dash_02_002", [&]() {Event_Effect("Fire_Attack_Dash_02_002"); });
	m_pModel->Add_EventCaller("Fire_Attack_Dash_03", [&]() {Event_Effect("Fire_Attack_Dash_03"); });
	m_pModel->Add_EventCaller("Fire_Attack_Dash_Hold_001", [&]() {Event_Effect("Fire_Attack_Dash_Hold_001"); });
	m_pModel->Add_EventCaller("Fire_Attack_Dash_Hold_002", [&]() {Event_Effect("Fire_Attack_Dash_Hold_002"); });
	m_pModel->Add_EventCaller("Fire_Attack_Dash_Hold_003", [&]() {Event_Effect("Fire_Attack_Dash_Hold_003"); });
	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_01", [&]() {Event_Effect("Fire_Attack_Justdodge_01"); });
	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_02", [&]() {Event_Effect("Fire_Attack_Justdodge_02"); });
	m_pModel->Add_EventCaller("Fire_Attack_Justdodge_03", [&]() {Event_Effect("Fire_Attack_Justdodge_03"); });
	m_pModel->Add_EventCaller("Fire_Attack_Upper", [&]() {Event_Effect("Fire_Attack_Upper"); });

	m_pModel->Add_EventCaller("Trail_On", [&]() 
	{ 
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(m_PlayerSasType, true);
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(true); 
	});

	m_pModel->Add_EventCaller("Trail_Off", [&]() 
	{
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(m_PlayerSasType, false);
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(false);
	});

	m_pModel->Add_EventCaller("WeaponBright_On", [&]()
	{
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(m_PlayerSasType, true);
	});

	m_pModel->Add_EventCaller("WeaponBright_Off", [&]()
	{
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(m_PlayerSasType, false);
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
				return m_bKineticRB && !m_bAir && (nullptr != m_pKineticObject) && !m_bHit;
			})
			.Priority(0)

			.AddTransition("NO_USE_KINETIC to KINETIC_RB_AIR_START", "KINETIC_RB_AIR_START")
			.Predicator([&]()->_bool 
			{
				return m_bKineticRB && m_bAir && (nullptr != m_pKineticObject) && !m_bHit;
			})
			.Priority(0)

#pragma region KineticRB

		.AddState("KINETIC_RB_START")
			.OnStart([&]() 
			{ 
				Enemy_Targeting(true);
				m_pASM->InputAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Start);
			})
			.Tick([&](double fTimeDelta) 
			{ 
				m_bKineticMove = true;
			})
			.AddTransition("KINETIC_RB_START to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool{return m_pKineticObject == nullptr;})
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
				if (nullptr != m_pKineticObject)
				{
					random_device rng;
					uniform_real_distribution<_double> dist0(-1.0, 1.0);
					uniform_real_distribution<_double> dist1(-1.0, 1.0);
					uniform_real_distribution<_double> dist2(-1.0, 1.0);

					_float3 vForce = _float3(0.f, 120.f, 0.f);
					_float3 vTorque = _float3(dist0(rng), dist1(rng), dist2(rng));
					static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce, vTorque);
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
				m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Cancel); 
			})
			.Tick([&](double fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_CANCEL to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool{return !m_bKineticRB && m_pASM->isSocketEmpty("Kinetic_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_CANCEL to KINETIC_RB_START", "KINETIC_RB_START")
			.Predicator([&]()->_bool{return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_AnimSocket")->GetPlayRatio() >= 0.2f);})
			.Priority(0)
			
#pragma endregion KineticRB

#pragma region KineticRB_Throw

		// 일반 던지기
		.AddState("KINETIC_RB_THROW_01_START")	// 루프 / 앤드
			.OnStart([&]() 
			{
				m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw01_Start);

				if (nullptr != m_pKineticObject)
				{
					if (nullptr != m_pTargetedEnemy)
					{
						Event_Kinetic_Throw();
					}
					else
					{
						_float3 vForce = { XMVectorGetX(m_vCamLook), XMVectorGetY(m_vCamLook), XMVectorGetZ(m_vCamLook) };
						vForce = XMVector3Normalize(vForce);
						vForce *= 1000.f;
						static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce);
					}

				}
			})
			.Tick([&](double fTimeDelta) 
			{
				m_bKineticMove = true;
			})

			.AddTransition("KINETIC_RB_THROW_01_START to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket") || m_bLeftClick || m_bDash || m_bJump; })
			.Priority(0)

			//.AddTransition("KINETIC_RB_THROW_01_START to KINETIC_RB_THROW_01_LOOP", "KINETIC_RB_THROW_01_LOOP")
			//.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket") && m_pASM->CheckSocketAnim("Kinetic_AnimSocket", "AS_ch0100_303_AL_throw2_wait_start"); })
			//.Priority(0)

		.AddState("KINETIC_RB_THROW_01_LOOP")	// 앤드 / 스타트2
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw01_Loop); })
			.Tick([&](double fTimeDelta) 
			{
				m_bKineticMove = true;
				m_fKineticCharge += fTimeDelta;
				if (nullptr != m_pKineticObject)
				{
					random_device rng;
					uniform_real_distribution<_double> dist0(-1.0, 1.0);
					uniform_real_distribution<_double> dist1(-1.0, 1.0);
					uniform_real_distribution<_double> dist2(-1.0, 1.0);

					_float3 vForce = _float3(0.f, 120.f, 0.f);
					_float3 vTorque = _float3(dist0(rng), dist1(rng), dist2(rng));
					static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce, vTorque);
				}

			})
			.OnExit([&]() {m_fKineticCharge = 0.f; })

			.AddTransition("KINETIC_RB_THROW_01_LOOP to KINETIC_RB_THROW_01_END", "KINETIC_RB_THROW_01_END")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_RB_THROW_01_LOOP to KINETIC_RB_THROW_02_START", "KINETIC_RB_THROW_02_START")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket") && m_pASM->CheckSocketAnim("Kinetic_AnimSocket", "AS_ch0100_303_AL_throw2_wait_loop"); })
			.Priority(0)

		//.AddState("KINETIC_RB_THROW_01_END")	// 종료 / 처음시작
		//	.OnStart([&]() { m_pASM->InputAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw01_Cancel); })
		//	.Tick([&](double fTimeDelta) {m_bKineticMove = true; })

		//	.AddTransition("KINETIC_RB_THROW_01_END to NO_USE_KINETIC", "NO_USE_KINETIC")
		//	.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Kinetic_AnimSocket"); })
		//	.Priority(0)

		//	.AddTransition("KINETIC_RB_THROW_01_END to NO_USE_KINETIC", "NO_USE_KINETIC")
		//	.Predicator([&]()->_bool {return m_bLeftClick || m_bDash || m_bJump; })
		//	.Priority(0)

			//.AddTransition("KINETIC_RB_THROW_01_END to KINETIC_RB_START", "KINETIC_RB_START")
			//.Predicator([&]()->_bool {return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_AnimSocket")->GetPlayRatio() >= 0.2f); })
			//.Priority(0)

		// 일반 던지기 연속
		.AddState("KINETIC_RB_THROW_02_START")
			.OnStart([&]() 
			{ 
				m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw02_Start);
				if (nullptr != m_pTargetedEnemy)
				{
					Vector4 vTargetPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					Vector4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					Vector4 vDir = vTargetPos - vMyPos;

					_float3 vForce = { vDir.x, vDir.y, vDir.z };
					vForce *= 1000.f;
					static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce);
				}
				else
				{
					_float3 vForce = { XMVectorGetX(m_vCamLook), XMVectorGetY(m_vCamLook), XMVectorGetZ(m_vCamLook) };
					vForce *= 1000.f;
					static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce);
				}
			})
			.Tick([&](double fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_THROW_02_START to KINETIC_RB_THROW_02_END", "KINETIC_RB_THROW_02_END")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket"); })
			.Priority(0)

		.AddState("KINETIC_RB_THROW_02_LOOP")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw02_Loop); })
			.Tick([&](double fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_THROW to KINETIC_RB_THROW_02_END", "KINETIC_RB_THROW_02_END")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket"); })
			.Priority(0)

		.AddState("KINETIC_RB_THROW_02_END")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw02_Cancel); })
			.Tick([&](double fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_THROW to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Kinetic_AnimSocket"); })
			.Priority(0)

#pragma endregion KineticRB_Throw

#pragma region KineticRB_Air

		.AddState("KINETIC_RB_AIR_START")
			.OnStart([&]() 
			{
				Enemy_Targeting(true);
				m_pASM->InputAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Start);
			})
			.Tick([&](double TimeDelta) 
			{
				m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false;
			})

			.AddTransition("KINETIC_RB_AIR_START to KINETIC_RB_AIR_LOOP", "KINETIC_RB_AIR_LOOP")
			.Predicator([&]()->_bool{return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_START to KINETIC_RB_AIR_CANCEL", "KINETIC_RB_AIR_CANCEL")
			.Predicator([&]()->_bool{return m_pKineticObject == nullptr;})
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_START to KINETIC_RB_AIR_CANCEL", "KINETIC_RB_AIR_CANCEL")
			.Predicator([&]()->_bool{return !m_bKineticRB;})
			.Priority(0)

		.AddState("KINETIC_RB_AIR_LOOP")
			.OnStart([&]() { 
				m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Loop);
			})
			.Tick([&](double TimeDelta) 
			{ 
				m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false;
				Kinetic_ByTurn();

				if (m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket"))
					m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Loop);
			})
			.OnExit([&]() { 
				m_bSwingKineticThrow = false; 
				m_fSwingLerpTimer = 0.f; 
			})
			.AddTransition("KINETIC_RB_AIR_LOOP to KINETIC_RB_AIR_THROW_01_START", "KINETIC_RB_AIR_THROW_01_START")
			.Predicator([&]()->_bool {return m_bKineticRB && m_bSwingKineticThrow; })
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_LOOP to KINETIC_RB_AIR_CANCEL", "KINETIC_RB_AIR_CANCEL")
			.Predicator([&]()->_bool{return !m_bKineticRB;})
			.Priority(0)

		.AddState("KINETIC_RB_AIR_CANCEL")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Cancel); })
			.Tick([&](double TimeDelta) { m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })
			.OnExit([&]() {m_bCanMove = true; m_bActiveGravity = true; })

			.AddTransition("KINETIC_RB_AIR_CANCEL to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool{return !m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket");})	// 취소시간만 조절해주면 될듯
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_CANCEL to KINETIC_RB_AIR_START", "KINETIC_RB_AIR_START")
			.Predicator([&]()->_bool{return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_Air_AnimSocket")->GetPlayRatio() >= 0.2f);})
			.Priority(0)

#pragma endregion KineticRB_Air

#pragma region KineticRB_Air_Throw

		.AddState("KINETIC_RB_AIR_THROW_01_START")
			.OnStart([&]() 
			{ 
				if (nullptr != m_pKineticObject)
				{
					if (nullptr != m_pTargetedEnemy)
					{
						Event_Kinetic_Throw();
					}
					else
					{
						_float3 vForce = { XMVectorGetX(m_vCamLook), XMVectorGetY(m_vCamLook), XMVectorGetZ(m_vCamLook) };
						vForce = XMVector3Normalize(vForce);
						vForce *= 1000.f;
						static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce);
					}
				}
			})
			.Tick([&](double TimeDelta) 
			{

			})
			.OnExit([&]()
			{
				//SetAbleState({ false, true, false ,false, true, true, false, false, true, false });
			})

			.AddTransition("KINETIC_RB_AIR_THROW_01_START to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket") || m_pASM->isSocketEmpty("Kinetic_Air_AnimSocket"); })
			.Priority(0)

		//	.AddTransition("KINETIC_RB_AIR_THROW_01_START to KINETIC_RB_AIR_THROW_01_LOOP", "KINETIC_RB_AIR_THROW_01_LOOP")
		//	.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket") && m_pASM->CheckSocketAnim("Kinetic_Air_AnimSocket", "AS_ch0100_312_AL_AirCap_throw1_start"); })
		//	.Priority(0)

		//.AddState("KINETIC_RB_AIR_THROW_01_LOOP")
		//	.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw01_Loop); })
		//	.Tick([&](double TimeDelta) {m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; }) // 루프 반복되도록 끝나가면 더 넣어주기

		//	.AddTransition("KINETIC_RB_AIR_THROW_01_LOOP to KINETIC_RB_AIR_THROW_01_END", "KINETIC_RB_AIR_THROW_01_END")
		//	.Predicator([&]()->_bool {return !m_bKineticRB; })
		//	.Priority(0)

		//	.AddTransition("KINETIC_RB_AIR_THROW_01_LOOP to KINETIC_RB_AIR_THROW_02_START", "KINETIC_RB_AIR_THROW_02_START")
		//	.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket") && m_pASM->CheckSocketAnim("Kinetic_Air_AnimSocket", "AS_ch0100_312_AL_AirCap_throw1_loop"); })
		//	.Priority(0)

		//.AddState("KINETIC_RB_AIR_THROW_01_END")
		//	.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw01_Cancel); })
		//	.Tick([&](double TimeDelta) {m_bKineticMove = false; m_bCanMove = true; m_bActiveGravity = true; })

		//	.AddTransition("KINETIC_RB_AIR_THROW_01_END to NO_USE_KINETIC", "NO_USE_KINETIC")
		//	.Predicator([&]()->_bool {return !m_bKineticRB && m_bOnFloor; })
		//	.Priority(0)

		//	.AddTransition("KINETIC_RB_AIR_THROW_01_END to NO_USE_KINETIC", "NO_USE_KINETIC")
		//	.Predicator([&]()->_bool {return !m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Air_AnimSocket", 0.7f); })
		//	.Priority(0)

		//	.AddTransition("KINETIC_RB_AIR_THROW_01_END to KINETIC_RB_AIR_START", "KINETIC_RB_AIR_START")
		//	.Predicator([&]()->_bool {return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_Air_AnimSocket")->GetPlayRatio() >= 0.2f); })
		//	.Priority(0)

		//.AddState("KINETIC_RB_AIR_THROW_02_START")
		//	.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw02_Start); })
		//	.Tick([&](double TimeDelta) {m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })

		//	.AddTransition("KINETIC_RB_AIR_THROW_02_START to KINETIC_RB_AIR_THROW_02_END", "KINETIC_RB_AIR_THROW_02_END")
		//	.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket"); })
		//	.Priority(0)

		//.AddState("KINETIC_RB_AIR_THROW_02_LOOP")
		//	.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw02_Loop); })
		//	.Tick([&](double TimeDelta) {m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })

		//	.AddTransition("KINETIC_RB_AIR_THROW_02_LOOP to KINETIC_RB_AIR_THROW_02_END", "KINETIC_RB_AIR_THROW_02_END")
		//	.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket"); })
		//	.Priority(0)

		//.AddState("KINETIC_RB_AIR_THROW_02_END")
		//	.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw02_Cancel); })
		//	.Tick([&](double TimeDelta) {m_bKineticMove = false; m_bCanMove = true; m_bActiveGravity = true; })

		//	.AddTransition("KINETIC_RB_AIR_THROW_02_END to NO_USE_KINETIC", "NO_USE_KINETIC")
		//	.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Kinetic_Air_AnimSocket", 0.7f) || (!m_bAir); })
		//	.Priority(0)

#pragma endregion KineticRB_Air_Throw

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
			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 0.f;

			m_bKineticCombo = false;
			m_pASM->ClearAnimSocket("Kinetic_Combo_AnimSocket");
			m_pASM->SetCurState("IDLE");

			IM_LOG("Kinetic No Use");
		})
		.Tick([&](double fTimeDelta)
		{
			if (nullptr != m_pKineticObject)
			{
				static_cast<CMapKinetic_Object*>(m_pKineticObject)->Set_Kinetic(false);
				IM_LOG("FALSE");
			}
		})
		.OnExit([&]()
		{
			m_bKineticMove = false;
			m_bSeperateAnim = false;
			SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})

			//.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_SLASH01", "KINETIC_COMBO_SLASH01")
			//.Predicator([&]()->_bool { return !m_bHit && m_bLeftClick && (m_fKineticCombo_Kinetic > 0.f) && !m_bAir && (nullptr != m_pKineticObject); })
			//.Priority(0)

			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_KINETIC01_CHARGE", "KINETIC_COMBO_KINETIC01_CHARGE")
			.Predicator([&]()->_bool { return !m_bHit && m_bKineticRB && (m_fKineticCombo_Slash > 0.f) && !m_bAir; })
			.Priority(0)

#pragma region 슬래시 콤보 1

		.AddState("KINETIC_COMBO_SLASH01")
		.OnStart([&]() 
		{
			m_bSeperateAnim = false;
			m_bKineticMove = false;

//			KineticObject_Targeting();

			if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
				Enemy_Targeting(true);

			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 10.f;

			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				m_pTransformCom->LookAt_NonY(m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
			}

			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash01);
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				_vector EnemyPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				m_pTransformCom->Chase(EnemyPos, fDistance * 0.01f, 3.f);
			}
		})

			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_bLeftClick && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_KINETIC01_CHARGE", "KINETIC_COMBO_KINETIC01_CHARGE")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f) && (nullptr != m_pTargetedEnemy) && (nullptr != m_pKineticObject); })
			.Priority(0)

#pragma endregion 슬래시 콤보 1

#pragma region 키네틱 콤보 1

		.AddState("KINETIC_COMBO_KINETIC01_CHARGE")
		.OnStart([&]() 
		{
			m_bSeperateAnim = false;
			m_bKineticMove = false;

			KineticObject_Targeting();

			if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
				Enemy_Targeting(true);

			if (nullptr != m_pKineticObject)
			{
				m_KineticObjectOrigionPos = m_pKineticObject->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(m_pKineticObject)->Set_Kinetic(true);
			}

			if (nullptr != m_pTargetedEnemy)
			{
				LookTarget();
			}

			m_bKineticCombo = true;
			m_fKineticCombo_Slash = 0.f;
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);

			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_245_AL_Pcon_cLeR_Lv1");
			Kinetic_Combo_MoveToKineticPoint();
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);

				Kinetic_Combo_AttachLerpObject();
			}

			m_fKineticCharge += (_float)fTimeDelta;
		})

			.AddTransition("KINETIC_COMBO_KINETIC01_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

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
			m_fKineticCharge = 0.f;
			m_fKineticCombo_Kinetic = 10.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv1);

			Kinetic_Combo_KineticAnimation();
		})
		.Tick([&](double fTimeDelta)
		{
			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}

			Kinetic_Combo_KineticAnimation();
		})

			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_SLASH02", "KINETIC_COMBO_SLASH02")
			.Predicator([&]()->_bool {return m_bLeftClick; })
			.Priority(0)

#pragma endregion 키네틱 콤보 1

#pragma region 슬래시 콤보 2

		.AddState("KINETIC_COMBO_SLASH02")
		.OnStart([&]() 
		{
			m_fKineticCombo_Kinetic = 0.f;
			m_fKineticCombo_Slash = 10.f;

			if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
				Enemy_Targeting(true);

			if (nullptr != m_pTargetedEnemy)
			{
				LookTarget();
			}

			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash02);
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				_vector EnemyPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				m_pTransformCom->Chase(EnemyPos, fDistance * 0.01f, 3.f);
			}
		})

			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_bLeftClick && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_KINETIC02_CHARGE", "KINETIC_COMBO_KINETIC02_CHARGE")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f); })
			.Priority(0)


#pragma endregion 슬래시 콤보 2

#pragma region 키네틱 콤보 2

		.AddState("KINETIC_COMBO_KINETIC02_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
				Enemy_Targeting(true);

			if (nullptr != m_pKineticObject)
			{
				m_KineticObjectOrigionPos = m_pKineticObject->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(m_pKineticObject)->Set_Kinetic(true);
			}

			if (nullptr != m_pTargetedEnemy)
			{
				LookTarget();
			}

			m_bKineticCombo = true;
			m_fKineticCombo_Slash = 0.f;
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);

			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_252_AL_Pcon_cLeR_Lv2");
			Kinetic_Combo_MoveToKineticPoint();
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);

				Kinetic_Combo_AttachLerpObject();
			}

			m_fKineticCharge += (_float)fTimeDelta;
		})

			.AddTransition("KINETIC_COMBO_KINETIC02_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

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
			m_fKineticCharge = 0.f;
			m_fKineticCombo_Kinetic = 10.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv2);
		})
		.Tick([&](double fTimeDelta)
		{
			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}

			Kinetic_Combo_KineticAnimation();
		})

			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump); })
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

			if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
				Enemy_Targeting(true);

			if (nullptr != m_pTargetedEnemy)
			{
				LookTarget();
			}

			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash03);
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				_vector EnemyPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				m_pTransformCom->Chase(EnemyPos, fDistance * 0.01f, 3.f);
			}
		})

			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_bLeftClick && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.25f); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_KINETIC03_CHARGE", "KINETIC_COMBO_KINETIC03_CHARGE")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.25f); })
			.Priority(0)

#pragma endregion 슬래시 콤보 3

#pragma region 키네틱 콤보 3

		.AddState("KINETIC_COMBO_KINETIC03_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
				Enemy_Targeting(true);

			if (nullptr != m_pKineticObject)
			{
				m_KineticObjectOrigionPos = m_pKineticObject->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(m_pKineticObject)->Set_Kinetic(true);
			}

			if (nullptr != m_pTargetedEnemy)
			{
				LookTarget();
			}

			m_bKineticCombo = true;
			m_fKineticCombo_Slash = 0.f;
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);

			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_259_AL_Pcon_cLeR_Lv3");
			Kinetic_Combo_MoveToKineticPoint();
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);

				Kinetic_Combo_AttachLerpObject();
			}

			m_fKineticCharge += (_float)fTimeDelta;
		})

			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_KINETIC03_CANCEL", "KINETIC_COMBO_KINETIC03_CANCEL")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_KINETIC03_THROW", "KINETIC_COMBO_KINETIC03_THROW")
			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f); })
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
			m_fKineticCharge = 0.f;
			m_fKineticCombo_Kinetic = 10.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv3);
		})
		.Tick([&](double fTimeDelta)
		{
			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}

			Kinetic_Combo_KineticAnimation();
		})
			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump); })
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

			if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
				Enemy_Targeting(true);

			if (nullptr != m_pTargetedEnemy)
			{
				LookTarget();
			}

			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash04);
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				_vector EnemyPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				m_pTransformCom->Chase(EnemyPos, fDistance * 0.01f, 3.f);
			}
		})

			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool {return m_bLeftClick && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_KINETIC04_CHARGE", "KINETIC_COMBO_KINETIC04_CHARGE")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f); })
			.Priority(0)

#pragma endregion 슬래시 콤보 4

#pragma region 키네틱 콤보 4

		.AddState("KINETIC_COMBO_KINETIC04_CHARGE")
		.OnStart([&]() 
		{
			KineticObject_Targeting();

			if (nullptr == m_pTargetedEnemy || static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())
				Enemy_Targeting(true);

			if (nullptr != m_pKineticObject)
			{
				m_KineticObjectOrigionPos = m_pKineticObject->GetTransform()->Get_WorldMatrix();
				static_cast<CMapKinetic_Object*>(m_pKineticObject)->Set_Kinetic(true);
			}

			if (nullptr != m_pTargetedEnemy)
			{
				LookTarget();
			}

			m_bKineticCombo = true;
			m_fKineticCombo_Slash = 0.f;
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);

			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_266_AL_Pcon_cLeR_Lv4");
			Kinetic_Combo_MoveToKineticPoint();
		})
		.Tick([&](double fTimeDelta)
		{
			if (m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
			{
				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;

				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));

				m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);

				Kinetic_Combo_AttachLerpObject();
			}

			m_fKineticCharge += (_float)fTimeDelta;
		})
			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)
		
			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_KINETIC04_CANCEL", "KINETIC_COMBO_KINETIC04_CANCEL")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_KINETIC04_THROW", "KINETIC_COMBO_KINETIC04_THROW")
			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f); })
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
			m_fKineticCharge = 0.f;
			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv4);
		})
		.Tick([&](double fTimeDelta)
		{
			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
			{
				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				m_pTransformCom->LocalMove(vLocal);
			}

			Kinetic_Combo_KineticAnimation();
		})

			.AddTransition("KINETIC_COMBO_KINETIC04_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return (nullptr == m_pTargetedEnemy) || (nullptr == m_pKineticObject); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC04_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_COMBO_KINETIC04_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB); })
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
			.AddTransition("JUSTDODGE_NONUSE to JUSTDODGE_USABLE", "JUSTDODGE_USABLE")
			.Predicator([&]()->_bool { return m_fJustDodgeAble > 0.f; })
			.Priority(0)

		.AddState("JUSTDODGE_USABLE")
			.OnStart([&]()
			{
				CGameInstance::GetInstance()->SetTimeRatioCurve("JustDodge_Imcome");	// 슬로우
			})
			.Tick([&](double fTimeDelta)
			{
				//IM_LOG("%f", m_fJustDodgeAble);
				//IM_LOG("JUSTDODGE!!");
				// 저스트닷지 조건 발생 시 돌릴 Tick함수
			})
			.OnExit([&](){})
			
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

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_AttackDesc()
{
	m_mapCollisionEvent.emplace("ATK_A1", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = 0.f;// (rand() % 50) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A2", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 100) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A3", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 100) + 200;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A4", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 50) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A5", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 200) + 300;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATTACK_DASH_START", [this]()
	{
		
	});
	m_mapCollisionEvent.emplace("ATTACK_DASH_END", [this]()
	{
		
	});
	m_mapCollisionEvent.emplace("ATTACK_NONCHARGE", [this]()
	{
		
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_01", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 50) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_02", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 100) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("CHARGE_ATTACK_03", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_MIDDLE;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 100) + 200;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR1", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 50) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR2", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 50) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR_NONCHARGE", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 50) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR_CHARGE_START", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 50) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_AIR_CHARGE_FALL", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 50) + 100;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATTACK_UPPER_START", [this]()
	{
		m_AttackDesc.eAttackSAS = m_PlayerSasType;
		m_AttackDesc.eAttackType = EAttackType::ATK_TO_AIR;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = (rand() % 100) + 200;
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});


	m_mapCollisionEvent.emplace("AS_ch0100_207_AL_atk_justdodge", [this]()
	{

	});
	m_mapCollisionEvent.emplace("AS_ch0100_216_AL_atk_dash1_start", [this]()
	{

	});
	m_mapCollisionEvent.emplace("AS_ch0100_217_AL_atk_dash2_end", [this]()
	{

	});
	m_mapCollisionEvent.emplace("AS_ch0100_218_AL_atk_dash3_end", [this]()
	{

	});
	m_mapCollisionEvent.emplace("AS_ch0100_219_AL_atk_dash4_end", [this]()
	{

	});

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

	return false;
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

	switch (m_PlayerSasType)
	{
	case ESASType::SAS_END:
		if (szEffectName.find("Fire") != string::npos)	// Fire키워드 들어간 이펙트는 거름
			break;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, EffectName)->Start_Attach(this, szBoneName);
//		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"FlowerLeg_Fall_Rose")->Start_Attach(this, "Eff01",true);
		// Effect = CJsonStorage::GetInstance()->FindOrLoadJson(m_mapDefaultEffect[szEffectName]);
		// pEffect = static_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_PlayerEffect", L"ProtoVFX_EffectGroup", &Effect));
		break;
	case ESASType::SAS_FIRE:
		if (szEffectName.find("Fire") == string::npos)	// Fire키워드 없으면 거름
			break;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, EffectName)->Start_Attach(this, szBoneName);
//		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, EffectName)->Start_AttachPivot(this, m_pModel->GetPivotMatrix(), szBoneName, true);

		break;
	}

	if (pEffect == nullptr)
		return;

	//pEffect->SetPlay();
	
	// _matrix	SocketMatrix = m_pModel->GetPivotMatrix() * m_pModel->GetBoneMatrix(szBoneName) * m_pTransformCom->Get_WorldMatrix();
	//
	// SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]) * fSize;
	// SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]) * fSize;
	// SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]) * fSize;
	//
	// if (nullptr != pEffect)
	// 	pEffect->Set_Transform(SocketMatrix);
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
	if (nullptr != m_pKineticObject && (nullptr != m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead())))
	{
		Vector4 vTargetPos = static_cast<CScarletCharacter*>(m_pTargetedEnemy)->GetColliderPosition();
		Vector4 vMyPos = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		Vector4 vDir = vTargetPos - vMyPos;
		vDir.w = 0.f;
		vDir = XMVector3Normalize(vDir);


		_float3 vForce = { vDir.x, vDir.y, vDir.z };
		vForce *= 1000.f;
		static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce);
	}
}

void CPlayer::Event_KineticSlowAction()
{
	CGameInstance::GetInstance()->SetTimeRatioCurve("Kinetic_Combo_Slow");
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

	if (m_bLeftClick)
		m_fKineticCombo_Slash = 10.f;

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
		static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(m_PlayerSasType, false);
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

	if (m_bOnBattle && (m_fNetualTimer >= 10.f))
	{
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

	if (nullptr == m_pTargetedEnemy || (static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
	{
		Enemy_Targeting(true);
	}

	if (nullptr != m_pTargetedEnemy && (!static_cast<CMonster*>(m_pTargetedEnemy)->IsDead()))
	{
		_vector vTargetPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
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
	Desc.m_pSocket = m_pModel->Get_BonePtr("RightWeapon");
	Desc.m_pTransform = m_pTransformCom;
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
	CGameObject* pBeforeEnemy = m_pTargetedEnemy;

	m_pTargetedEnemy = nullptr;

	if (pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster") == nullptr ||
		pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects().empty())
	{
		m_pTargetedEnemy = nullptr;
	}
	else
	{
		_float fDistance = 20.f;
		list<CGameObject*>	DistanceList;
		for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
		{
			if ((!static_cast<CMonster*>(iter)->IsDead())/* || (static_cast<CMonster*>(iter)->IsActive())*/)
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

			m_pTargetedEnemy = DistanceList.front();
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
					m_pTargetedEnemy = iter;
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

	m_pKineticObject = nullptr;

	_uint iCount = 0;
	_uint iTest = 0;

	if (pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject") == nullptr
		|| pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects().empty())
	{
		m_pKineticObject = nullptr;
	}
	else
	{
		_float fDistance = 20.f;

		for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
		{
			//if (false == static_cast<CMapKinetic_Object*>(iter)->Usable())
			//	continue;

			_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			_float fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

			if (fDistance > fCurDistance)
			{
				fDistance = fCurDistance;
				m_pKineticObject = iter;
			}
			else
			{
				static_cast<CMapKinetic_Object*>(iter)->Set_Kinetic(false);
			}
		}
	}
}

void CPlayer::Spline_Kinetic(_double TimeDelta)
{
	static _double fLerpTime = 0.f;
	static _vector vSplinePoint_01, vSplinePoint_02, vSplinePoint_03, vSplinePoint_04;
	static _vector vBeforePos;
	
	if (nullptr == m_pTargetedEnemy) { fLerpTime = 0.f; return; }
	if (nullptr == m_pKineticObject) { fLerpTime = 0.f; return; }

	if (0.f == fLerpTime)
	{
		vSplinePoint_01 = XMLoadFloat4(&m_vSplinePoint_01);
		vSplinePoint_02 = XMLoadFloat4(&m_vSplinePoint_02);
		vSplinePoint_03 = XMLoadFloat4(&m_vSplinePoint_03);
		vSplinePoint_04 = XMLoadFloat4(&m_vSplinePoint_04);
	}

	if (2.f >= fLerpTime)
	{
		_vector vCurPos = XMVectorCatmullRom(vSplinePoint_01, vSplinePoint_02, vSplinePoint_03, vSplinePoint_04, fLerpTime * 0.5f);
		_vector vDir = vCurPos - vBeforePos;
		_float3 vVelocity = _float3(XMVectorGetX(vDir) * 10.f, XMVectorGetY(vDir) * 10.f, XMVectorGetZ(vDir) * 10.f);
		m_pKineticObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vCurPos);
		vBeforePos = vCurPos;

		fLerpTime += TimeDelta;
	}

	if (2.f < fLerpTime)
	{
		fLerpTime = 0.f;
		vSplinePoint_01 = XMLoadFloat4(&m_vSplinePoint_01);
		vSplinePoint_02 = XMLoadFloat4(&m_vSplinePoint_02);
		vSplinePoint_03 = XMLoadFloat4(&m_vSplinePoint_03);
		vSplinePoint_04 = XMLoadFloat4(&m_vSplinePoint_04);
	}
}

void CPlayer::Kinetic_Test(_float fRatio)
{
	if (0.f == m_fSwingLerpTimer)
		m_vKineticInitLook = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_LOOK);

	m_fSwingLerpTimer += g_fTimeDelta * 0.5f;

	if (1.f > m_fSwingLerpTimer)	// 빙 돌면서 나한테까지 끌어오는 과정, 1초 안에 이뤄짐
	{
		static _vector vBeforePos;

		_float fScalar = XMVectorGetX(XMVector3Length(m_vToKineticObj)) - XMVectorGetX(XMVector3Length(m_vKineticPoint));
		fScalar *= (1.f - m_fSwingLerpTimer);

		_float fAngle = XMConvertToRadians(180.f);

		_vector vVector = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		// 좌우 체크
		_vector vMyright = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		if (0.f > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyright), XMVector3Normalize(m_vToKineticObj))))
		{
			// 왼쪽
			vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
			vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(-(fAngle * m_fSwingLerpTimer)));
			m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
		}
		else
		{
			// 오른쪽
			vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
			vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(fAngle * m_fSwingLerpTimer));
			m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
		}

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		m_pKineticObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vMyPos + vVector);
	//	m_pKineticObject->GetTransform()->LookAt_Lerp(m_vKineticInitLook, vMyPos, m_fSwingLerpTimer);
	//	m_pKineticObject->GetTransform()->LookAt_Lerp_Test(vMyPos, m_fSwingLerpTimer);

		if(!XMVector3Equal((vMyPos + vVector), vBeforePos))
			m_vKineticOrbit = (vMyPos + vVector) - vBeforePos;

		_vector vKineticRight = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_RIGHT);
		if (0.f < XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_vKineticOrbit), XMVector3Normalize(vKineticRight))))
			m_bRight = true;
		else
			m_bRight = false;

		vBeforePos = vMyPos + vVector;
	}
	else // 나한테까지 오고 나서 부터의 빙빙 회전과 적으로 날아감
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_vector vMonsterPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vKineticPos = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vKineticToMonster = vMonsterPos - vKineticPos;
		_vector vKineticRight = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_RIGHT);

		_vector vOrbitLook = vMyPos - vKineticPos;
		_vector vMove = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vOrbitLook));

	//	m_pKineticObject->GetTransform()->LookAt_Lerp(m_vKineticInitLook, vMyPos, m_fSwingLerpTimer);
	//	m_pKineticObject->GetTransform()->LookAt_Lerp_Test(vMyPos, m_fSwingLerpTimer);

		_float fOrbitSpeed = XMVectorGetX(XMVector3Length(m_vToKineticObj));

		if (m_bRight)
		{
			if (0.9f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKineticToMonster), XMVector3Normalize(-vMove))))
			{
				//m_vThrow = XMVector3Normalize(vKineticToMonster);
				//m_vThrow = vKineticToMonster;
				//m_bSwingKineticThrow = true;
				//m_fSwingLerpTimer = 0.f;
			}
			else
			{
				// 바라보며 우측으로 이동
				//m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, vKineticRight);
				m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
				m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, -vMove);
			}
		}
		else
		{
			if (0.9f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKineticToMonster), XMVector3Normalize(vMove))))
			{
				//m_vThrow = XMVector3Normalize(vKineticToMonster);
				//m_vThrow = vKineticToMonster;
				//m_bSwingKineticThrow = true;
				//m_fSwingLerpTimer = 0.f;
			}
			else
			{
				// 바라보며 좌측으로 이동
				//m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, -vKineticRight);
				m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
				m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, vMove);
			}
		}
	}
}

void CPlayer::Kinetic_ByTurn()
{
	if (nullptr == m_pTargetedEnemy) { m_fSwingLerpTimer = 0.f; return; }
	if (nullptr == m_pKineticObject) { m_fSwingLerpTimer = 0.f; return; }

	m_fSwingLerpTimer += g_fTimeDelta;

	if (1.f > m_fSwingLerpTimer)
	{
		static _vector vBeforePos;

		_float fScalar = XMVectorGetX(XMVector3Length(m_vToKineticObj)) - XMVectorGetX(XMVector3Length(m_vKineticPoint));
		fScalar *= (1.f - m_fSwingLerpTimer);

		_float fAngle = XMConvertToRadians(180.f);

		_vector vVector = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		// 좌우 체크
		_vector vMyright = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		if (0.f > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyright), XMVector3Normalize(m_vToKineticObj))))
		{
			// 왼쪽
			vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
			vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(-(fAngle * m_fSwingLerpTimer)));
			m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
			m_bRight = true;	// 플레이어 바라보는 축 기준 오른쪽 축으로 도는가?
		}
		else
		{
			// 오른쪽
			vVector = XMVector3Normalize(m_vToKineticObj) * (fScalar + 3.f);
			vVector = XMVector3TransformNormal(vVector, XMMatrixRotationY(fAngle * m_fSwingLerpTimer));
			m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
			m_bRight = false;
		}

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pKineticObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vMyPos + vVector);

	}
	else // 나한테까지 오고 나서 부터의 빙빙 회전과 적으로 날아감
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_vector vMonsterPos = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vKineticPos = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vKineticToMonster = vMonsterPos - vKineticPos;

		_vector vOrbitLook = vMyPos - vKineticPos;
		_vector vMove = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vOrbitLook));

		_float fOrbitSpeed = XMVectorGetX(XMVector3Length(m_vToKineticObj));

		if (m_bRight)
		{
			if (0.f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKineticToMonster), XMVector3Normalize(vMove))))
			{
				m_vThrow = vKineticToMonster;
				m_bSwingKineticThrow = true;
				m_fSwingLerpTimer = 0.f;
				m_fKineticCombo_Kinetic = 10.f;
			}
			else
			{
				m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), g_fTimeDelta * 0.5f);
				m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, vMove);
			}
		}
		else
		{
			if (0.f <= XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKineticToMonster), XMVector3Normalize(-vMove))))
			{
				m_vThrow = vKineticToMonster;
				m_bSwingKineticThrow = true;
				m_fSwingLerpTimer = 0.f;
				m_fKineticCombo_Kinetic = 10.f;
			}
			else
			{
				m_pKineticObject->GetTransform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(g_fTimeDelta * 0.5f));
				m_pKineticObject->GetTransform()->Move(g_fTimeDelta * fOrbitSpeed, -vMove);
			}
		}
	}
}

void CPlayer::Kinetic_Combo_KineticAnimation()
{
	if (nullptr != m_pTargetedEnemy && nullptr != m_pKineticObject && !m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
	{
		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio(m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayTime());
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		m_pKineticObject->GetTransform()->Set_WorldMatrix(SocketMatrix);
	}
}

void CPlayer::Kinetic_Combo_MoveToKineticPoint()
{
	// 몬스터를 바라보는 작업은 여기에서 일어나며, 실제 이동해야 하는 위치의 위치벡터를 생성한다.

	if (nullptr != m_pTargetedEnemy && nullptr != m_pKineticObject)
	{
		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones(0.f, EAnimUpdateType::NORMAL);
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();

		_vector vDiff = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - SocketMatrix.r[3];	// 회전하기 전 물체 본과 실제 물체 사이의 방향벡터
		_vector vRePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + vDiff; // 물체 사이의 거리만큼 이동한 위치
		_vector vTmpLookAtMonster = m_pTargetedEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - vRePos; // 예비 이동 위치에서 몬스터를 바라본 방향벡터

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector vLookPoint = vMyPos + vTmpLookAtMonster;
		vLookPoint = XMVector3Normalize(vLookPoint);
		vLookPoint = XMVectorSetW(vLookPoint, 1.f);
//		m_pTransformCom->LookAt_NonY(vLookPoint);	// 예비 Look벡터로 바라보는 방향을 바꿈

		_matrix ModifiedMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();
		_vector vRenewalDiff = m_pKineticObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - ModifiedMatrix.r[3];
		_vector vRenewalRePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + vRenewalDiff;	// 이동해야 하는 최종 위치

		m_vKineticComboRefPoint = vRenewalRePos;
	}
}

void CPlayer::Kinetic_Combo_AttachLerpObject()
{
	if (nullptr != m_pTargetedEnemy && nullptr != m_pKineticObject && !m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
	{
		m_pKineticAnimModel->GetPlayAnimation()->Update_Bones_SyncRatio(0.f);
		m_pKineticAnimModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticAnimModel->GetBoneMatrix("Waist") * m_pTransformCom->Get_WorldMatrix();

		_float fScale_Right = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[0]));
		_float fScale_Up = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[1]));
		_float fScale_Look = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[2]));

//		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);// *fScale_Right;
//		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);// *fScale_Up;
//		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);// *fScale_Look;

		_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();

		if (1.f < fRatio)
		{
			fRatio = 1.f;
		}

		_vector vLerpLook = XMVectorLerp(m_KineticObjectOrigionPos.r[2], SocketMatrix.r[2], fRatio);
		_vector vLerpRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook);
		_vector vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);

		//_vector vLerpRight = XMVectorLerp(m_KineticObjectOrigionPos.r[0], SocketMatrix.r[0], fRatio);
		//_vector vLerpUp = XMVectorLerp(m_KineticObjectOrigionPos.r[1], SocketMatrix.r[1], fRatio);
		//_vector vLerpLook = XMVectorLerp(m_KineticObjectOrigionPos.r[2], SocketMatrix.r[2], fRatio);
		_vector vLerpPos = XMVectorLerp(m_KineticObjectOrigionPos.r[3], SocketMatrix.r[3], fRatio);

		vLerpRight = XMVector3Normalize(vLerpRight) *fScale_Right;
		vLerpUp = XMVector3Normalize(vLerpUp) *fScale_Up;
		vLerpLook = XMVector3Normalize(vLerpLook) *fScale_Look;

		_matrix LerpMatrix = { vLerpRight, vLerpUp, vLerpLook, vLerpPos };

		m_pKineticObject->GetTransform()->Set_WorldMatrix(LerpMatrix);

		//m_pKineticObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
	}
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

	//for (auto& iter : m_mapPlayerEffect)
	//	Safe_Release(iter);
	//m_mapPlayerEffect.clear();

	//for (auto& iter : m_mapFireEffect)
	//	Safe_Release(iter);
	//m_mapFireEffect.clear();

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

//	Safe_Release(m_pContectRigidBody);
}
