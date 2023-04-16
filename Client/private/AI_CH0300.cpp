#include "stdafx.h"
#include "AI_CH0300.h"
#include "GameInstance.h"
#include "Shader.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "ScarletWeapon.h"
#include "RigidBody.h"
#include "EffectSystem.h"
#include "ControlledRigidBody.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "AI_CH0300_AnimInstance.h"
#include "PlayerInfoManager.h"
#include "Weapon_wp0300.h"
#include "PlayerStartPosition.h"

#include "Enemy.h"

CAI_CH0300::CAI_CH0300(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CScarletCharacter(pDevice, pContext)
{
}

CAI_CH0300::CAI_CH0300(const CAI_CH0300& rhs)
	:CScarletCharacter(rhs)
{
}

HRESULT CAI_CH0300::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_CH0300::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_Parts()))
		return E_FAIL;

	if (FAILED(SetUp_Event()))
		return E_FAIL;

	if (FAILED(SetUp_EffectEvent()))
		return E_FAIL;

	if (FAILED(SetUp_AttackDesc()))
		return E_FAIL;

	if (FAILED(SetUp_Sound()))
		return E_FAIL;

	ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC));

	m_pTransformCom->SetTransformDesc({ 5.f, XMConvertToRadians(180.f) });

	m_pModel->FindMaterial(L"MI_ch0300_HOOD_0")->SetActive(false);

	m_SoundStore.CloneSound("AI_fire_swing_1");
	m_SoundStore.CloneSound("AI_fire_swing_2");
	m_SoundStore.CloneSound("AI_fire_stab_ready");
	m_SoundStore.CloneSound("AI_fire_stab_mult_1");
	m_SoundStore.CloneSound("attack_fire_3");

	return S_OK;
}

void CAI_CH0300::BeginTick()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (auto pStartPos = pGameInstance->Find_OneObjectByType<CHanabiStartPosition>(LEVEL_NOW, LAYER_TRIGGER))
	{
		m_pTransformCom->CopyWorld(pStartPos->GetTransform());
		pStartPos->SetDelete();
	}

	__super::BeginTick();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, PLATERTEST_LAYER_PLAYER)->GetGameObjects())
	{
		if (L"Player" == iter->GetPrototypeTag())
		{
			NULL_CHECK(m_pPlayer = iter);
			Safe_AddRef(m_pPlayer);
			break;
		}
	}
}

void CAI_CH0300::Tick(_double TimeDelta)
{
	m_fTimeDelta = TimeDelta;

	if (false == CPlayerInfoManager::GetInstance()->isHanabiActive())
		return;

	__super::Tick(TimeDelta);
	m_pModel->Tick(TimeDelta);

	m_pTransformCom->SetTransformDesc({ 5.f, XMConvertToRadians(360.f) });

	DistanceCheck();

	m_pASM->Tick(TimeDelta);
	BehaviorCheck();

	for (auto& iter : m_vecWeapon)
	{
		if (m_bWeaponOn && 1.f > m_fWeaponDissolve)
		{
			m_fWeaponDissolve = min(1.f, m_fWeaponDissolve + (_float)TimeDelta);
			static_cast<CWeapon_wp0300*>(iter)->SetDissolve(m_fWeaponDissolve);
		}
		else if (!m_bWeaponOn && 0.f < m_fWeaponDissolve)
		{
			m_fWeaponDissolve = max(0.f, m_fWeaponDissolve - (_float)TimeDelta);
			static_cast<CWeapon_wp0300*>(iter)->SetDissolve(m_fWeaponDissolve);
		}

		iter->Tick(TimeDelta);
		{
			_bool bCol = Collision_Check_Capsule_Improved(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), m_AttackDesc, m_bAttackEnable, ECOLLIDER_TYPE_BIT(ECOLLIDER_TYPE_BIT::CTB_MONSTER | ECOLLIDER_TYPE_BIT::CTB_MONSTER_PART));
		}
	}
}

void CAI_CH0300::Late_Tick(_double TimeDelta)
{
	if (false == CPlayerInfoManager::GetInstance()->isHanabiActive())
		return;

	__super::Late_Tick(TimeDelta);

	for (auto& iter : m_vecWeapon)
		iter->Late_Tick(TimeDelta);

	if (m_bVisible && (nullptr != m_pRenderer))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
	}
}

void CAI_CH0300::AfterPhysX()
{
	if (true == CPlayerInfoManager::GetInstance()->GetAILock())
	{
		m_pCollider->SetActive(true);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -100.f, 0.f, 0.f));
		m_pCollider->SetFootPosition(XMVectorSet(0.f, -100.f, 0.f, 0.f));
		m_pCollider->SetActive(false);
	}

	if (false == CPlayerInfoManager::GetInstance()->isHanabiActive())
	{
		m_pCollider->SetActive(false);
		return;
	}
	else
	{
		m_pCollider->SetActive(true);
	}

	__super::AfterPhysX();

	for (auto& iter : m_vecWeapon)
	{
		static_cast<CScarletWeapon*>(iter)->Setup_BoneMatrix(m_pModel, m_pTransformCom->Get_WorldMatrix());
		static_cast<CScarletWeapon*>(iter)->Trail_Tick(m_fTimeDelta);

		m_fEffectTime += m_fTimeDelta;

		if (m_fEffectTime >= 0.2f && m_bAttackEnable)
		{
			static_cast<CWeapon_wp0300*>(iter)->FireEffect();
			m_fEffectTime = 0.f;
		}
	}

	MovePerSecondCheck();
}

HRESULT CAI_CH0300::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

HRESULT CAI_CH0300::Render_ShadowDepth()
{
	m_pModel->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CAI_CH0300::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	m_bHit = true;

	m_DamageDesc.m_iDamage = tDamageParams.iDamage;
	m_DamageDesc.m_iDamageType = tDamageParams.eAttackType;
	m_DamageDesc.m_vHitDir = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - XMLoadFloat4(&tDamageParams.vHitFrom);
	m_DamageDesc.m_eHitDir = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);

	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, L"Default_Hit_EF_TEX")->Start_Attach(this, "Waist", true);
	CPlayerInfoManager::GetInstance()->Change_HanabiHP(CHANGE_DECREASE, tDamageParams.iDamage);

	if (tDamageParams.eAttackType == EAttackType::ATK_HEAVY || tDamageParams.eAttackType == EAttackType::ATK_TO_AIR)
	{
		if(CGameInstance::GetInstance()->Check_ObjectAlive(tDamageParams.pCauser))
			m_pTransformCom->LookAt_NonY(tDamageParams.pCauser->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
		else if (tDamageParams.vHitFrom.w != 0.f)
			m_pTransformCom->LookAt_NonY(tDamageParams.vHitFrom);
	}

	Collision_End();
}

void CAI_CH0300::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	m_pASM->Imgui_RenderState();

	if (ImGui::Button("Hit_Light"))
	{
		m_bHit = true;
		m_DamageDesc.m_iDamageType = EAttackType::ATK_LIGHT;
		m_DamageDesc.m_vHitDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	}
	if (ImGui::Button("Hit_Middle"))
	{
		m_bHit = true;
		m_DamageDesc.m_iDamageType = EAttackType::ATK_MIDDLE;
		m_DamageDesc.m_eHitDir = EBaseAxis::NORTH;
	}
	if (ImGui::Button("Hit_Knuckback"))
	{
		m_bHit = true;
		m_DamageDesc.m_iDamageType = EAttackType::ATK_HEAVY;
	}
	if (ImGui::Button("Hit_Airborne"))
	{
		m_bHit = true;
		m_DamageDesc.m_iDamageType = EAttackType::ATK_TO_AIR;
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

		// ?ㅼ쭏?곸씤 ?댄럺??遺숈씠湲?
		static char cEffectName[MAX_PATH]{};
		static string szEffectName = "";
		ImGui::InputText("Effect Search", cEffectName, MAX_PATH);
		szEffectName = cEffectName;

		static char cBoneName[MAX_PATH]{};
		static string szBoneName = "";
		ImGui::InputText("Bone Search", cBoneName, MAX_PATH);
		szBoneName = cBoneName;

		ImGui::InputFloat("EffectTime", &fEffectPlayTime);

		if ((m_pModel->GetPlayAnimation()->GetName() == szAnimName) &&
			(fPrePlayTime <= fEffectPlayTime) &&
			(fCurPlayTime > fEffectPlayTime))
		{
			//CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, s2ws(szEffectName), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, pivot1, szBoneName, true, m_bEffectUpdate);
		}

		fPrePlayTime = fCurPlayTime;

		if ("" != szAnimName)
			m_pModel->Get_AnimList()[szAnimName]->Imgui_RenderProperty();
	}

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
}

_float4x4 CAI_CH0300::GetBoneMatrix(const string& strBoneName, _bool bPivotapply)
{
	if (m_pModel->Get_BonePtr(strBoneName) == nullptr)
		return XMMatrixIdentity();

	return m_pModel->GetBoneMatrix(strBoneName, bPivotapply);
}

_float4x4 CAI_CH0300::GetPivotMatrix()
{
	if (m_pModel == nullptr)
		return XMMatrixIdentity();

	return m_pModel->GetPivotMatrix();
}

HRESULT CAI_CH0300::SetUp_Components(void* pArg)
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
		}
	}

	NULL_CHECK(m_pASM = CAI_CH0300_AnimInstance::Create(m_pModel, this));

	return S_OK;
}

HRESULT CAI_CH0300::SetUp_Event()
{
	m_pModel->Add_EventCaller("Collision_Start", [&]() {Collision_Start(); });
	m_pModel->Add_EventCaller("Collision_End", [&]() {Collision_End(); });
	m_pModel->Add_EventCaller("Collision_Twist", [&]() {Collision_Twist(); });
	m_pModel->Add_EventCaller("Jump", [&]() {m_fYSpeed = 10.f ; });
	m_pModel->Add_EventCaller("HitDown", [&]() {m_fYSpeed = -30.f; });
	m_pModel->Add_EventCaller("Twist_Particle", [&]() {Twist_Particle(); });


	/*
	m_SoundStore.CloneSound("AI_fire_swing_1");
	m_SoundStore.CloneSound("AI_fire_swing_2");
	m_SoundStore.CloneSound("AI_fire_stab_ready");
	m_SoundStore.CloneSound("AI_fire_stab_mult_1");
	m_SoundStore.CloneSound("attack_fire_3");
	*/

	m_pModel->Add_EventCaller("AI_fire_swing_1", [&]() {m_SoundStore.PlaySound("AI_fire_swing_1", m_pTransformCom); });
	m_pModel->Add_EventCaller("AI_fire_swing_2", [&]() {m_SoundStore.PlaySound("AI_fire_swing_2", m_pTransformCom); });
	m_pModel->Add_EventCaller("AI_fire_stab_ready", [&]() {m_SoundStore.PlaySound("AI_fire_stab_ready", m_pTransformCom); });
	m_pModel->Add_EventCaller("AI_fire_stab_mult_1", [&]() {m_SoundStore.PlaySound("AI_fire_stab_mult_1", m_pTransformCom); });
	m_pModel->Add_EventCaller("attack_fire_3", [&]() {m_SoundStore.PlaySound("attack_fire_3", m_pTransformCom); });

	return S_OK;
}

HRESULT CAI_CH0300::SetUp_EffectEvent()
{
	return S_OK;
}

HRESULT CAI_CH0300::SetUp_Sound()
{
	return S_OK;
}

HRESULT CAI_CH0300::SetUp_AttackDesc()
{
	m_mapCollisionEvent.emplace("ATK_A1", [this]()
	{
		m_AttackDesc.eAttackSAS = ESASType::SAS_FIRE;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 0.1f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A2_START", [this]()
	{
		m_AttackDesc.eAttackSAS = ESASType::SAS_FIRE;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 0.1f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A2", [this]()
	{
		m_AttackDesc.eAttackSAS = ESASType::SAS_FIRE;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 0.1f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});
	m_mapCollisionEvent.emplace("ATK_A3", [this]()
	{
		m_AttackDesc.eAttackSAS = ESASType::SAS_FIRE;
		m_AttackDesc.eAttackType = EAttackType::ATK_LIGHT;
		m_AttackDesc.eDeBuff = EDeBuffType::DEBUFF_END;
		m_AttackDesc.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 0.1f);
		m_AttackDesc.pCauser = this;
		m_AttackDesc.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	});

	return S_OK;
}

void CAI_CH0300::BehaviorCheck()
{
	string strCurState = m_pASM->GetCurStateName();

	if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
	{
		// 타게팅 된 적이 없으면 플레이어 추적
		if ("WALK" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(m_vPlayerPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta);
		}

		if ("RUN" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(m_vPlayerPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta * 2.f);
		}

		if ("JUMP_RISE" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(m_vPlayerPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta);
		}

		if ("JUMP_FALL" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(m_vPlayerPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta);
		}

		if ("HIT_KNUCKBACK_START" == strCurState)
		{
			m_pTransformCom->Go_Backward(m_fTimeDelta * 2.f * (1.f - m_pModel->GetPlayAnimation()->GetPlayRatio()));
		}
		
		Collision_End();
	}
	else
	{
		// 타게팅 된 적이 있으면 적 추적
		_float4 vEnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		if ("WALK" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vEnemyPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta);
		}

		if ("RUN" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vEnemyPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta * 2.f);
		}

		if ("JUMP_RISE" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vEnemyPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta);
		}

		if ("JUMP_FALL" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vEnemyPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta);
		}

		if ("HIT_KNUCKBACK_START" == strCurState)
		{
			m_pTransformCom->Go_Backward(m_fTimeDelta * 2.f * (1.f - m_pModel->GetPlayAnimation()->GetPlayRatio()));
		}

		if ("ATK_B1_START" == strCurState && (0.15f < m_pModel->GetPlayAnimation()->GetPlayRatio()))
		{
			m_pTransformCom->Go_Straight(m_fTimeDelta * 0.2f);
		}
	}
}

void CAI_CH0300::DistanceCheck()
{
	_float4 vPlayerPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vPlayerLeft = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_RIGHT) * -1.f;
	_float4 vPosDir = XMVector3Normalize(vPlayerLeft);
	m_vPlayerPos = vPlayerPos + vPosDir;

	_float4 vPlayerDistance = XMLoadFloat4(&m_vPlayerPos)
		- m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	m_fDistance_toPlayer = vPlayerDistance.Length();

	CGameObject* pEnemy = nullptr;
	if (nullptr != (pEnemy = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()))
	{
		// 몬스터와의 거리는 레이캐스트로 탐지한다.
		physx::PxRaycastHit hitBuffer[1];
		physx::PxRaycastBuffer rayOut(hitBuffer, 1);

		RayCastParams param;
		param.rayOut = &rayOut;
		param.vOrigin = GetColliderPosition();
		param.vDir = static_cast<CScarletCharacter*>(pEnemy)->GetColliderPosition() - GetColliderPosition();
		param.fDistance = param.vDir.Length() * 2.f;
		param.iTargetType = CTB_MONSTER | CTB_MONSTER_PART;
		param.bSingle = true;
		param.fVisibleTime = 1.f;

		if (CGameInstance::GetInstance()->RayCast(param))
		{
			for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
			{
				auto pHit = rayOut.getAnyHit(i);
				m_fDistance_toEnemy = pHit.distance;
			}
		}
		else
		{
			m_fDistance_toEnemy = -1.f;
		}

	}
	else
	{
		m_fDistance_toEnemy = -1.f;
	}

	TeleportCheck();

}

void CAI_CH0300::MovePerSecondCheck()
{
	if (m_pASM->GetCurStateName() == "WALK" || m_pASM->GetCurStateName() == "RUN")
	{
		if (1.f > m_fObstacleDetectTimer)
		{
			m_fObstacleDetectTimer += (_float)m_fTimeDelta;

			_float fMoveDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - XMLoadFloat4(&m_vBefPos)));
			m_fMovePerSecond += fMoveDistance;
		}
		else
		{
			if (3.f > m_fMovePerSecond)
			{
				m_bJump = true;
			}
			
			m_fMovePerSecond = 0.f;
			m_fObstacleDetectTimer = 0.f;
		}
	}
	else
	{
		m_fMovePerSecond = 0.f;
		m_fObstacleDetectTimer = 0.f;
	}

	m_vBefPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
}

void CAI_CH0300::TeleportCheck()
{
	if (15.f <= m_fDistance_toPlayer)
	{
		TeleportToPlayerBack();
	}
}

void CAI_CH0300::TeleportToPlayerBack()
{
	_float4 vPlayerPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_float4 vCamBack = CGameInstance::GetInstance()->Get_CamLook() * -5.f;
	vCamPos += vCamBack;
	vCamPos.w = 1.f;
	_float4 vResetPos = { vCamPos.x, vPlayerPos.y, vCamPos.z, 1.f };

	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer rayOut(hitBuffer, 1);

	_float fDistanceMag = 1.f;

	RayCastParams param;
	param.rayOut = &rayOut;
	param.vOrigin = vPlayerPos;
	param.vDir = vResetPos - vPlayerPos;
	param.fDistance = param.vDir.Length();
	param.iTargetType = CTB_STATIC | CTB_PSYCHICK_OBJ;
	param.bSingle = true;
	param.fVisibleTime = 1.f;

	if (CGameInstance::GetInstance()->RayCast(param))
	{
		for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
		{
			auto pHit = rayOut.getAnyHit(i);
			fDistanceMag = pHit.distance / param.fDistance;

			fDistanceMag *= 0.8f;
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vResetPos);
	m_pTransformCom->LookAt_NonY(vPlayerPos);
	m_pCollider->SetFootPosition(vResetPos);
}

void CAI_CH0300::Collision_Start()
{
	m_bAttackEnable = true;

	//static_cast<CScarletWeapon*>(m_vecWeapon.front())->Set_Bright(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_eAttack_SAS_Type, true);

	string szCurAttackState = m_pASM->GetCurStateName();
	const auto iter = m_mapCollisionEvent.find(szCurAttackState);
	if (iter == m_mapCollisionEvent.end())
		return;
	iter->second();

	static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(true);
}

void CAI_CH0300::Collision_End()
{
	m_bAttackEnable = false;

	static_cast<CScarletWeapon*>(m_vecWeapon.front())->Trail_Setting(false);
}

void CAI_CH0300::Collision_Twist()
{
	physx::PxSweepHit hitBuffer[4];
	physx::PxSweepBuffer overlapOut(hitBuffer, 4);
	SphereSweepParams params2;
	params2.sweepOut = &overlapOut;
	params2.fRadius = 2.f;
	params2.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 2.f) + XMVectorSet(0.f, 2.f, 0.f, 0.f);
	params2.vUnitDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	params2.fDistance = 2.f;
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
				DAMAGE_PARAM tParam;
				ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
				tParam.eAttackSAS = ESASType::SAS_FIRE;
				tParam.eAttackType = EAttackType::ATK_LIGHT;
				tParam.eDeBuff = EDeBuffType::DEBUFF_END;
				tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_END;
				tParam.iDamage = static_cast<_uint>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage * 0.1f);

				tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				pMonster->TakeDamage(tParam);
			}
		}
	}

	_float4x4 EffectPivotMatrix =
		XMMatrixTranslation(0.f, 0.f, -3.f);
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_FIRE_ATTACK, L"Fire_Attack_3_Double_twist")->Start_AttachPivot(this, EffectPivotMatrix, "Eff01", true);
}

void CAI_CH0300::Twist_Particle()
{
	_float4x4 ParticlePivotMatrix =
		XMMatrixScaling(10.f, 10.f, 10.f) *
		XMMatrixTranslation(0.f, 0.f, -3.f);
	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, L"Fire_Attack_3_Twist_Particle")->Start_AttachPivot(this, ParticlePivotMatrix, "Eff01", false);
}

HRESULT CAI_CH0300::Setup_Parts()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Json Weapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/wp0300.json");
	Weapon["Model"] = "../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_300/wp0300.static_model";

	CGameObject* pGameObject = nullptr;

	WEAPON_DESC		Desc;
	ZeroMemory(&Desc, sizeof(WEAPON_DESC));
	Desc.m_PivotMatrix = m_pModel->GetPivotMatrix();
	Desc.m_pJson = &Weapon;

	//pGameInstance->Clone_GameObject(LAYER_AI, TEXT("HanabiWeapon"), &Desc);

	pGameObject = pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("HanabiWeapon"), &Desc);
	m_vecWeapon.push_back(pGameObject);

	return S_OK;
}

CAI_CH0300* CAI_CH0300::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAI_CH0300* pInstance = new CAI_CH0300(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAI_CH0300");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CAI_CH0300::Clone(void* pArg)
{
	CAI_CH0300* pInstance = new CAI_CH0300(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAI_CH0300");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CAI_CH0300::Free()
{
	__super::Free();

	for (auto& iter : m_vecWeapon)
		Safe_Release(iter);
	m_vecWeapon.clear();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pASM);
	Safe_Release(m_pPlayer);
}
