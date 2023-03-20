#include "stdafx.h"
#include "..\public\Monster.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "Shader.h"
#include "Model.h"
#include "Animation.h"
#include "VFX_Manager.h"
#include "JsonStorage.h"
#include "MonsterHpUI.h"



CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CScarletCharacter(pDevice, pContext)
{
}

CMonster::CMonster(const CScarletCharacter& rhs)
	: CScarletCharacter(rhs)
{
}

HRESULT CMonster::Initialize(void* pArg)
{
	FAILED_CHECK(CScarletCharacter::Initialize(pArg));
	m_DeathTimeline.SetFinishFunction([this]
	{
		m_bDelete = true;
	});
	m_DeathTimeline.SetCurve("Simple_Increase");

	m_vecDefaultBlood.push_back(L"Default_Blood_00");
	m_vecDefaultBlood.push_back(L"Default_Blood_01");
	m_vecDefaultBlood.push_back(L"Default_Blood_02");
	m_vecDefaultBlood.push_back(L"Default_Blood_03");
	m_vecDefaultBlood.push_back(L"Default_Blood_04");
	m_vecDefaultBlood.push_back(L"Default_Blood_05");
	m_vecDefaultBlood.push_back(L"Default_Blood_06");

	m_vecFireBlood.push_back(L"Fire_Blood_00");
	m_vecFireBlood.push_back(L"Fire_Blood_01");
	m_vecFireBlood.push_back(L"Fire_Blood_02");
	m_vecFireBlood.push_back(L"Fire_Blood_03");
	m_vecFireBlood.push_back(L"Fire_Blood_04");
	m_vecFireBlood.push_back(L"Fire_Blood_05");
	m_vecFireBlood.push_back(L"Fire_Blood_06");

	m_vecElecBlood.push_back(L"Elec_Blood_00");
	m_vecElecBlood.push_back(L"Elec_Blood_01");
	m_vecElecBlood.push_back(L"Elec_Blood_02");
	m_vecElecBlood.push_back(L"Elec_Blood_03");
	m_vecElecBlood.push_back(L"Elec_Blood_04");
	m_vecElecBlood.push_back(L"Elec_Blood_05");
	m_vecElecBlood.push_back(L"Elec_Blood_06");

	m_vecDefaultHit.push_back(L"Default_Attack_OnHit_00");
	m_vecDefaultHit.push_back(L"Default_Attack_OnHit_01");

	m_vecFireHit.push_back(L"Fire_Attack_OnHit_00");
	m_vecFireHit.push_back(L"Fire_Attack_OnHit_01");

	m_vecElecHit.push_back(L"Elec_Attack_OnHit_00");
	m_vecElecHit.push_back(L"Elec_Attack_OnHit_01");

	//HP UI
	m_UI_PivotMatrixes.fill(XMMatrixIdentity());
	SetUp_UI();

	m_SoundStore.CloneSound("fx_impact_flesh");

	return S_OK;
}

void CMonster::Tick(_double TimeDelta)
{
	CScarletCharacter::Tick(TimeDelta);
	Update_DeadDissolve(TimeDelta);
}

void CMonster::Late_Tick(_double TimeDelta)
{
	CScarletCharacter::Late_Tick(TimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
}

void CMonster::Imgui_RenderProperty()
{
	CScarletCharacter::Imgui_RenderProperty();
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


	if (ImGui::Button("Activate"))
	{
		SetActive();
	}
}

void CMonster::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	wstring HitBloodName = L"";
	wstring HitEffectName = L"";

	switch (tDamageParams.eAttackSAS)
	{
	case ESASType::SAS_FIRE:
		random_shuffle(m_vecFireBlood.begin(), m_vecFireBlood.end());
		random_shuffle(m_vecFireHit.begin(), m_vecFireHit.end());

		HitBloodName = m_vecFireBlood.front();
		HitEffectName = m_vecFireHit.front();
		break;

	case ESASType::SAS_ELETRIC:
		random_shuffle(m_vecElecBlood.begin(), m_vecElecBlood.end());
		random_shuffle(m_vecElecHit.begin(), m_vecElecHit.end());

		HitBloodName = m_vecElecBlood.front();
		HitEffectName = m_vecElecHit.front();
		break;

	default:
		random_shuffle(m_vecDefaultBlood.begin(), m_vecDefaultBlood.end());
		random_shuffle(m_vecDefaultHit.begin(), m_vecDefaultHit.end());

		HitBloodName = m_vecDefaultBlood.front();
		HitEffectName = m_vecDefaultHit.front();
		break;
	}


	if (tDamageParams.vHitPosition == _float4(0.f, 0.f, 0.f, 1.f))
	{
		// 타격점이 원점으로 찍히면 레이캐스트를 쏴서 위치를 판단
		if (tDamageParams.pCauser != nullptr)
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
					if (auto pMonster = dynamic_cast<CMonster*>(pCollidedObject))
					{
						_vector vHitPos = XMVectorSet(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
						_vector vEffectDir = tDamageParams.vSlashVector;
						CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitBloodName)->Start_AttachPosition(this, vHitPos, vEffectDir);

						_float4 vPos = vHitPos;
						m_SoundStore.PlaySound("fx_impact_flesh", &vPos);
						CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitEffectName)->Start_AttachPosition(this, vHitPos, vEffectDir);


						if (tDamageParams.eAttackSAS == ESASType::SAS_NOT)
							CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, L"Player_Default_Sword_Particle")->Start_AttachPosition(this, vHitPos, vEffectDir);
						else if (tDamageParams.eAttackSAS == ESASType::SAS_FIRE)
						{
							CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, L"Player_Fire_Sword_Particle")->Start_AttachPosition(this, vHitPos, vEffectDir);
							CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Sas_Fire_Sword_Particle"))->Start_AttachPosition(this, vHitPos, vEffectDir);
						}
						else if (tDamageParams.eAttackSAS == ESASType::SAS_ELETRIC)
							CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Player_Elec_Sword_Particle")->Start_AttachPosition(this, vHitPos, vEffectDir);
					
					
						CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Circle_Distortion_NonFlip_Short")->Start_AttachPosition(this, vHitPos, vEffectDir);
					
					}
				}
			}
		}
	}
	else
	{
		// 타격점이 잘 나오면 해당 위치에 생성
		_vector vHitPos = tDamageParams.vHitPosition;
		_vector vEffectDir = tDamageParams.vSlashVector;
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitBloodName)->Start_AttachPosition(this, vHitPos, vEffectDir);

		_float4 vPos = vHitPos;
		m_SoundStore.PlaySound("fx_impact_flesh", &vPos);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitEffectName)->Start_AttachPosition(this, vHitPos, vEffectDir);

		if (tDamageParams.eAttackSAS == ESASType::SAS_NOT)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, L"Player_Default_Sword_Particle")->Start_AttachPosition(this, vHitPos, vEffectDir);
		else if (tDamageParams.eAttackSAS == ESASType::SAS_FIRE)
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, L"Player_Fire_Sword_Particle")->Start_AttachPosition(this, vHitPos, vEffectDir);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Sas_Fire_Sword_Particle"))->Start_AttachPosition(this, vHitPos, vEffectDir);
		}
		else if (tDamageParams.eAttackSAS == ESASType::SAS_ELETRIC)
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Player_Elec_Sword_Particle")->Start_AttachPosition(this, vHitPos, vEffectDir);

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Circle_Distortion_NonFlip_Short")->Start_AttachPosition(this, vHitPos, vEffectDir);
	}
}

HRESULT CMonster::Render_ShadowDepth()
{
	m_pModelCom->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

_float4x4 CMonster::GetBoneMatrix(const string& strBoneName, _bool bPivotapply)
{
	if (m_pModelCom->Get_BonePtr(strBoneName) == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->GetBoneMatrix(strBoneName, bPivotapply);
}

_float4x4 CMonster::GetPivotMatrix()
{
	if (m_pModelCom == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->GetPivotMatrix();
}

_bool CMonster::CheckDamagedTarget(CScarletCharacter* pTarget)
{
	const auto itr = m_DamagedTargetList.find(pTarget);
	if (itr == m_DamagedTargetList.end())
	{
		m_DamagedTargetList.insert(pTarget);
		return true;
	}

	return false;
}

void CMonster::ClearDamagedTarget()
{
	m_DamagedTargetList.clear();
}

void CMonster::HitTargets(physx::PxSweepBuffer& sweepOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff)
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

void CMonster::HitTargets(physx::PxOverlapBuffer& overlapOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff)
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

void CMonster::Update_DeadDissolve(_double TimeDelta)
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

void CMonster::DeBuff_End()
{
	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 0;
	}
}

void CMonster::DeBuff_Fire()
{
	m_fDeBuffTime = 8.f;
	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 1;
	}
}

void CMonster::DeBuff_Oil()
{
	m_fDeBuffTime = 10.f;
	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 2;
	}
}

void CMonster::MoveTransformJson(Json& jsonDest, void* pArg)
{
	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		CTransform::MoveTransformJson(jsonDest, json);
	}
}

void CMonster::SetInvisible(_bool bInvisibleOut)
{
	m_bInvisible = bInvisibleOut; 
}

_bool CMonster::GetInvisible()
{
	return m_bInvisible;
}

void CMonster::TurnEyesOut()
{
	CMonster* pMonster = this;
	if (this == nullptr) return;

	CEffectGroup* pEffectGroup = nullptr;
	pEffectGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Lockon_Find", TEXT("Layer_UI"));
	assert(pEffectGroup != nullptr);

	//TimeLine 끝나고 삭제
	pEffectGroup->Start_AttachPivot(this, m_UI_PivotMatrixes[FINDEYES], "Target", true);
}

void CMonster::SetActive()
{
	m_bActive = true;
}

void CMonster::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
