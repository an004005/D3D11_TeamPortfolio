#include "stdafx.h"
#include "..\public\Enemy.h"
#include "Model.h"
#include "Animation.h"
#include "Renderer.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "GameUtils.h"
#include "Player.h"
#include "RigidBody.h"
#include "FSMComponent.h"
#include "TestTarget.h"

vector<wstring>			CEnemy::s_vecDefaultBlood{
	L"Default_Blood_00",
	L"Default_Blood_01",
	L"Default_Blood_02",
	L"Default_Blood_03",
	L"Default_Blood_04",
	L"Default_Blood_05",
	L"Default_Blood_06"
};
vector<wstring>			CEnemy::s_vecFireBlood{
	L"Fire_Blood_00",
	L"Fire_Blood_01",
	L"Fire_Blood_02",
	L"Fire_Blood_03",
	L"Fire_Blood_04",
	L"Fire_Blood_05",
	L"Fire_Blood_06"
};
vector<wstring>			CEnemy::s_vecElecBlood{
	L"Elec_Blood_00",
	L"Elec_Blood_01",
	L"Elec_Blood_02",
	L"Elec_Blood_03",
	L"Elec_Blood_04",
	L"Elec_Blood_05",
	L"Elec_Blood_06",
};

vector<wstring>			CEnemy::s_vecDefaultHit{
	L"Default_Attack_OnHit_00",
	L"Default_Attack_OnHit_01"
};
vector<wstring>			CEnemy::s_vecFireHit{
	L"Fire_Attack_OnHit_00",
	L"Fire_Attack_OnHit_01"
};
vector<wstring>			CEnemy::s_vecElecHit{
	L"Elec_Attack_OnHit_00",
	L"Elec_Attack_OnHit_01"
};

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CScarletCharacter(pDevice, pContext)
{
}

CEnemy::CEnemy(const CScarletCharacter& rhs)
	: CScarletCharacter(rhs)
{
}

HRESULT CEnemy::Initialize(void* pArg)
{
	FAILED_CHECK(CScarletCharacter::Initialize(pArg));
	m_DeathTimeline.SetFinishFunction([this]
	{
		m_bDelete = true;
	});
	m_DeathTimeline.SetCurve("Simple_Increase");

	m_FireTick.Initialize(1.0, false);

	SetUpComponents(pArg);
	SetUpSound();
	SetUpAnimationEvent();
	SetUpFSM();

	return S_OK;
}

void CEnemy::Tick(_double TimeDelta)
{
	CScarletCharacter::Tick(TimeDelta);
	FindTarget();
	Update_DeadDissolve(TimeDelta);
}

void CEnemy::Late_Tick(_double TimeDelta)
{
	CScarletCharacter::Late_Tick(TimeDelta);
	if (m_bVisible)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

void CEnemy::Imgui_RenderProperty()
{
	CScarletCharacter::Imgui_RenderProperty();
	ImGui::Checkbox("Use TestTarget", &m_bFindTestTarget);

	if (ImGui::CollapsingHeader("Edit Stat"))
	{
		ImGui::InputInt("MaxHP", &m_iMaxHP);
		ImGui::InputInt("MaxCrushGage", &m_iMaxCrushGage);
		ImGui::Checkbox("HasCrushGage", &m_bHasCrushGage);
		_int iLevel = m_iLevel;
		ImGui::InputInt("Level", &iLevel);
		m_iLevel = iLevel;
		ImGui::InputInt("AtkDamage", &m_iAtkDamage);
	}

	m_DeathTimeline.Imgui_RenderEditor();

	if (ImGui::CollapsingHeader("Anim Fast Modifier"))
	{
		static string szMonsterAnimName = "";
		if (ImGui::BeginListBox("Animation List"))
		{
		  static char szSeachAnim[MAX_PATH] = "";
		  ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

		  const string strSearch = szSeachAnim;
		  const _bool bSearch = strSearch.empty() == false;

		  for (auto& Pair : m_pModelCom->Get_AnimList())
		  {
		     if (bSearch)
		     {
		        if (Pair.first.find(strSearch) == string::npos)
		           continue;
		     }

		     const bool bSelected = szMonsterAnimName == Pair.first;
		     if (bSelected)
		        ImGui::SetItemDefaultFocus();

		     if (ImGui::Selectable(Pair.first.c_str(), bSelected))
		        szMonsterAnimName = Pair.first;
		  }
		  ImGui::EndListBox();
		}

		if ("" != szMonsterAnimName)
		  m_pModelCom->Get_AnimList()[szMonsterAnimName]->Imgui_RenderProperty();
	}
}

HRESULT CEnemy::Render_ShadowDepth()
{
	m_pModelCom->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CEnemy::SetUpComponents(void* pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));
}

void CEnemy::SetUpSound()
{
	if (m_strDeathSoundTag.empty() == false)
		m_SoundStore.CloneSound(m_strDeathSoundTag);
	if (m_strImpactTag.empty() == false)
		m_SoundStore.CloneSound(m_strImpactTag);
	if (m_strImpactVoiceTag.empty() == false)
		m_SoundStore.CloneSound(m_strImpactVoiceTag);
}

void CEnemy::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	if (m_bDead)
		return;


	// 이상한 데미지 들어오는거 감지용, 버그 다 찾으면 지우기
	Assert(tDamageParams.iDamage > 0);
	Assert(tDamageParams.iDamage < 20000);

	// ex) 데미지 100 => 90 ~ 110 랜덤으로 변경
	const _int iDamageRandomRange = tDamageParams.iDamage / 5;
	const _int iDamageRandomize = (_int)CMathUtils::RandomUInt((_uint)iDamageRandomRange);
	tDamageParams.iDamage += iDamageRandomize - iDamageRandomRange / 2;

	m_eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom, &m_eSimpleHitFrom);
	m_eCurAttackType = tDamageParams.eAttackType;
	m_bHitWeak = IsWeak(dynamic_cast<CRigidBody*>(tDamageParams.pContactComponent));


	CheckDeBuff(tDamageParams.eDeBuff);
	HitEffect(tDamageParams);
	CheckCrushGage(tDamageParams);
	CheckHP(tDamageParams);
}

void CEnemy::SetBrainCrush()
{
	if (m_iCrushGage <= 0)
	{
		m_DeathTimeline.PlayFromStart();
	}
}

void CEnemy::SetEnemyBatchDataStat(ENEMY_STAT tStat)
{
	m_iMaxHP = tStat.iMaxHP;
	m_iHP = m_iMaxHP;
	m_iMaxCrushGage = tStat.iMaxCrushGage;
	m_iCrushGage = m_iMaxCrushGage;
	m_bHasCrushGage = tStat.bHasCrushGage;
	m_iAtkDamage = tStat.iAtkDamage;
	m_iLevel = tStat.iLevel;
}

ENEMY_STAT CEnemy::GetEnemyBatchDataStat()
{
	ENEMY_STAT tStat;
	tStat.iMaxHP = m_iMaxHP;
	tStat.iMaxCrushGage = m_iMaxCrushGage;
	tStat.bHasCrushGage = m_bHasCrushGage;
	tStat.iAtkDamage = m_iAtkDamage;
	tStat.iLevel = m_iLevel;
	return tStat;
}

void CEnemy::SetDead()
{
	if (m_bDead)
		return;
	if (m_iCrushGage <= 0)
		return;

	m_bDead = true;
	m_DeathTimeline.PlayFromStart();
}

void CEnemy::FindTarget()
{
	if (m_bFindTestTarget)
	{
		auto pPlayer = CGameInstance::GetInstance()->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pObj)
		{
			return dynamic_cast<CTestTarget*>(pObj) != nullptr;
		});
		m_pTarget = dynamic_cast<CScarletCharacter*>(pPlayer);
	}
	else
	{
		// todo 임시 코드, AI추가되면 바꿔야됨
		auto pPlayer = CGameInstance::GetInstance()->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pObj)
		{
			return dynamic_cast<CPlayer*>(pObj) != nullptr;
		}, PLATERTEST_LAYER_PLAYER);
		m_pTarget = dynamic_cast<CScarletCharacter*>(pPlayer);
	}
}

_float4x4 CEnemy::GetBoneMatrix(const string& strBoneName, _bool bPivotapply)
{
	if (m_pModelCom->Get_BonePtr(strBoneName) == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->GetBoneMatrix(strBoneName, bPivotapply);
}

_float4x4 CEnemy::GetPivotMatrix()
{
	if (m_pModelCom == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->GetPivotMatrix();
}

void CEnemy::HitEffect(DAMAGE_PARAM& tDamageParams)
{
	if (tDamageParams.vHitPosition == _float4(0.f, 0.f, 0.f, 1.f) && tDamageParams.pCauser != nullptr)
	{
		_vector vRayPos = static_cast<CScarletCharacter*>(tDamageParams.pCauser)->GetColliderPosition();
		vRayPos = XMVectorSetW(vRayPos, 1.f);
		_vector vRayDir = GetColliderPosition() - static_cast<CScarletCharacter*>(tDamageParams.pCauser)->GetColliderPosition();
		vRayDir = XMVectorSetW(vRayDir, 0.f);

		physx::PxRaycastHit hitBuffer[1];
		physx::PxRaycastBuffer rayOut(hitBuffer, 1);

		RayCastParams param;
		param.rayOut = &rayOut;
		param.vOrigin = vRayPos;
		param.vDir = vRayDir;
		param.fDistance = 10.f;
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
					tDamageParams.vHitPosition = XMVectorSet(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
				}
			}
		}
	}

	wstring HitBloodName;
	wstring HitEffectName;

	switch (tDamageParams.eAttackSAS)
	{
	case ESASType::SAS_FIRE:
		HitBloodName = s_vecFireBlood[CMathUtils::RandomUInt(s_vecFireBlood.size() - 1)];
		HitEffectName = s_vecFireHit[CMathUtils::RandomUInt(s_vecFireHit.size() - 1)];
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, L"Player_Fire_Sword_Particle")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Sas_Fire_Sword_Particle"))->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
		break;

	case ESASType::SAS_ELETRIC:
		HitBloodName = s_vecElecBlood[CMathUtils::RandomUInt(s_vecElecBlood.size() - 1)];
		HitEffectName = s_vecElecHit[CMathUtils::RandomUInt(s_vecElecHit.size() - 1)];
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Player_Elec_Sword_Particle")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
		break;

	case ESASType::SAS_NOT:
		HitBloodName = s_vecDefaultBlood[CMathUtils::RandomUInt(s_vecDefaultBlood.size() - 1)];
		HitEffectName = s_vecDefaultHit[CMathUtils::RandomUInt(s_vecDefaultHit.size() - 1)];
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, L"Player_Default_Sword_Particle")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
		break;
	}
	

	if (HitBloodName.empty() == false)
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitBloodName)->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
	if (HitEffectName.empty() == false)
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitEffectName)->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);

	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Circle_Distortion_NonFlip_Short")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);

	if (m_strImpactTag.empty() == false)
		m_SoundStore.PlaySound(m_strImpactTag, &tDamageParams.vHitPosition);
	if (m_strImpactVoiceTag.empty() == false)
		m_SoundStore.PlaySound(m_strImpactVoiceTag, &tDamageParams.vHitPosition);
}

void CEnemy::CheckDeBuff(EDeBuffType eDeBuff)
{
	switch (eDeBuff)
	{
	case EDeBuffType::DEBUFF_FIRE:
		if (m_eDeBuff == EDeBuffType::DEBUFF_OIL 
			|| CGameUtils::GetRandFloat() >= m_fFireResist)
			m_eDeBuff = EDeBuffType::DEBUFF_FIRE;
		break;
	case EDeBuffType::DEBUFF_OIL: 
		m_eDeBuff = EDeBuffType::DEBUFF_OIL;
		break;
	case EDeBuffType::DEBUFF_THUNDER:
		if (m_eDeBuff == EDeBuffType::DEBUFF_WATER 
			|| CGameUtils::GetRandFloat() >= m_fThunderResist)
			m_eDeBuff = EDeBuffType::DEBUFF_THUNDER;
		break;
	case EDeBuffType::DEBUFF_WATER:
		m_eDeBuff = EDeBuffType::DEBUFF_WATER;
		break;
	case EDeBuffType::DEBUFF_END:
		break;
	default: 
		NODEFAULT;
	}
}

void CEnemy::CheckCrushGage(DAMAGE_PARAM& tDamageParams)
{
	if (m_bHasCrushGage)
	{
		_int iDamage = tDamageParams.iDamage;
		if (m_bHitWeak)
			iDamage *= 2;

		if (tDamageParams.eAttackSAS == ESASType::SAS_HARDBODY)
			iDamage *= 3;

		switch (m_eCurAttackType)
		{
		case EAttackType::ATK_LIGHT: break;
		case EAttackType::ATK_MIDDLE: break;
		case EAttackType::ATK_HEAVY:
			iDamage *= 2;
			break;
		case EAttackType::ATK_TO_AIR: break;
		case EAttackType::ATK_END: break;
		default: 
			NODEFAULT;
		}

		m_iCrushGage -= iDamage / 10;
		if (m_iCrushGage < 0)
			m_iCrushGage = 0;
	}	
}

void CEnemy::CheckHP(DAMAGE_PARAM& tDamageParams)
{
	_int iDamage = tDamageParams.iDamage;
	if (m_bHitWeak)
		iDamage *= 2;

	m_iHP -= iDamage;
	if (m_iHP < 0)
	{
		if (m_iCrushGage > 0)
			SetDead();
		m_iHP = 0;
	}
}

void CEnemy::ResetHitData()
{
	m_eCurAttackType = EAttackType::ATK_END;
	m_eHitFrom = EBaseAxis::AXIS_END;
	m_eSimpleHitFrom = ESimpleAxis::AXIS_END;
	m_bHitWeak = false;
}

void CEnemy::Update_DeadDissolve(_double TimeDelta)
{
	_float fOut = 0.f;
	if (m_DeathTimeline.Tick(TimeDelta, fOut))
	{
		for (auto pMtrl : m_pModelCom->GetMaterials())
		{
			pMtrl->GetParam().Floats[0] = fOut;
		}
	}
}

void CEnemy::DeBuff_End()
{
	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 0;
	}
}

void CEnemy::DeBuff_Fire()
{
	m_fDeBuffTime = 8.f;
	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 1;
	}
}

void CEnemy::DeBuff_Oil()
{
	m_fDeBuffTime = 10.f;
	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 2;
	}
}

void CEnemy::Update_DeBuff(_double TimeDelta)
{
	CScarletCharacter::Update_DeBuff(TimeDelta);
		
	if (m_eDeBuff == EDeBuffType::DEBUFF_FIRE)
	{
		m_FireTick.Tick(TimeDelta);
		if (m_FireTick.Use())
		{
			DAMAGE_PARAM tParam;
			tParam.iDamage = _uint((_float)m_iMaxHP / 100.f * (1.f - m_fFireResist));
			tParam.pCauser = this;
			TakeDamage(tParam);
		}
	}
}

void CEnemy::MoveJsonData(Json& jsonDest, void* pArg)
{
	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		CTransform::MoveTransformJson(jsonDest, json);
	}
}

_bool CEnemy::CheckDamagedTarget(CScarletCharacter* pTarget)
{
	const auto itr = m_DamagedTargetList.find(pTarget);
	if (itr == m_DamagedTargetList.end())
	{
		m_DamagedTargetList.insert(pTarget);
		return true;
	}

	return false;
}

void CEnemy::ClearDamagedTarget()
{
	m_DamagedTargetList.clear();
}

void CEnemy::HitTargets(physx::PxSweepBuffer& sweepOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff)
{
	for (int i = 0; i < sweepOut.getNbAnyHits(); ++i)
	{
		auto pTarget = dynamic_cast<CScarletCharacter*>(CPhysXUtils::GetOnwer(sweepOut.getAnyHit(i).actor));
		if (pTarget == nullptr)
			continue;

		if (CheckDamagedTarget(pTarget))
		{
			DAMAGE_PARAM tDamageParams;
			tDamageParams.iDamage = iDamage;
			tDamageParams.eAttackType = eAtkType;
			tDamageParams.eDeBuff = eDeBuff;
			tDamageParams.pCauser = this;
			tDamageParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			memcpy(&tDamageParams.vHitPosition, &sweepOut.getAnyHit(i).position, sizeof(_float3));
			memcpy(&tDamageParams.vHitNormal, &sweepOut.getAnyHit(i).normal, sizeof(_float3));
			pTarget->TakeDamage(tDamageParams);
		}
	}
}

void CEnemy::HitTargets(physx::PxOverlapBuffer& overlapOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff)
{
	for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
	{
		auto pTarget = dynamic_cast<CScarletCharacter*>(CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor));
		if (pTarget == nullptr)
			continue;

		if (CheckDamagedTarget(pTarget))
		{
			DAMAGE_PARAM tDamageParams;
			tDamageParams.iDamage = iDamage;
			tDamageParams.eAttackType = eAtkType;
			tDamageParams.eDeBuff = eDeBuff;
			tDamageParams.pCauser = this;
			tDamageParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			tDamageParams.vHitPosition = tDamageParams.vHitFrom;
			pTarget->TakeDamage(tDamageParams);
		}
	}
}

void CEnemy::Free()
{
	CScarletCharacter::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
}
