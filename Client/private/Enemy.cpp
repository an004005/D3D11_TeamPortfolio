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
#include "Enemy_AnimInstance.h"
#include "TestTarget.h"
#include "PlayerInfoManager.h"
#include "GameManager.h"
#include "EMUI.h"
#include "MonsterHpUI.h"
#include "MonsterShildUI.h"
#include "ImguiUtils.h"
#include "UI_Manager.h"

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

vector<wstring>			CEnemy::s_vecDefaultDecal{
	L"Default_Hit_Decal_A",
	L"Default_Hit_Decal_B",
	L"Default_Hit_Decal_C"
};

vector<wstring>			CEnemy::s_vecFireDecal{
	L"Fire_Hit_Decal_A",
	L"Fire_Hit_Decal_B",
	L"Fire_Hit_Decal_C"
};

vector<wstring>			CEnemy::s_vecElecDecal{
	L"Elec_Hit_Decal_A",
	L"Elec_Hit_Decal_B",
	L"Elec_Hit_Decal_C"
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
		SetDelete();
	});
	m_DeathTimeline.SetCurve("Simple_Increase");

	m_FireTick.Initialize(1.0, false);

	SetUpComponents(pArg);
	SetUpSound();
	SetUpAnimationEvent();
	SetUpFSM();
	SetUpUI();

	return S_OK;
}

void CEnemy::BeginTick()
{
	CScarletCharacter::BeginTick();
	if (m_bSpawnEffect)
		CreateSpawnEffect();
}

void CEnemy::Tick(_double TimeDelta)
{
	CScarletCharacter::Tick(TimeDelta);
	FindTarget(TimeDelta);
	Update_DeadDissolve(TimeDelta);

	if(m_pEMUI != nullptr)
		m_pEMUI->Update_UIInfo();

	m_pModelCom->Tick(TimeDelta);

	//if (GetKeyState('K') & 0x8000)
	//	SetDead();
	
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

	ImGui::InputInt("HP", &m_iHP);

	if (ImGui::Button("KBRecompiler"))
		SetUpFSM();

	if (ImGui::CollapsingHeader("SpawnEffectEdit"))
	{
		if (ImGui::Button("SpawnEffect"))
		{
			CreateSpawnEffect();
		}

		ImGui::InputFloat("SpawnDistance", &m_fSpawnDistortionDistancePivot);
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&m_SpawnEffectPivot, tInfo, true, true);
	}

	ImGui::Checkbox("Use TestTarget", &m_bFindTestTarget);

	if (ImGui::CollapsingHeader("Edit Stat"))
	{
		ImGui::InputInt("MaxHP", &m_iMaxHP);
		ImGui::InputInt("MaxCrushGage", &m_iMaxCrushGauge);
		ImGui::Checkbox("HasCrushGage", &m_bHasCrushGauge);
		_int iLevel = iEemeyLevel;
		ImGui::InputInt("Level", &iLevel);
		iEemeyLevel = iLevel;
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
		{
			CAnimation* pAnim = m_pModelCom->Get_AnimList()[szMonsterAnimName];
			if(pAnim != nullptr)
				pAnim->Imgui_RenderProperty();
		}
		 
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

void CEnemy::SetUpFSM()
{
	Safe_Release(m_pFSM);
}

void CEnemy::SetUpUI()
{
	m_pEMUI = CEMUI::Create(this);
	assert(m_pEMUI != nullptr);
}

void CEnemy::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	if (m_bDead)
		return;

	// ex) 데미지 100 => 90 ~ 110 랜덤으로 변경
	const _int iDamageRandomRange = tDamageParams.iDamage / 5;
	const _int iDamageRandomize = (_int)CMathUtils::RandomUInt((_uint)iDamageRandomRange);
	tDamageParams.iDamage += iDamageRandomize - iDamageRandomRange / 2;

	m_eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom, &m_eSimpleHitFrom);
	m_eCurAttackType = tDamageParams.eAttackType;
	m_bHitWeak = IsWeak(dynamic_cast<CRigidBody*>(tDamageParams.pContactComponent));
	if (tDamageParams.eAttackType == EAttackType::ATK_DOWN)
	{
		tDamageParams.eAttackType = EAttackType::ATK_MIDDLE;
		m_bAirToDown = true;
	}

	CheckHitPositoin(tDamageParams);
	CheckDeBuff(tDamageParams.eDeBuff);
	HitEffect(tDamageParams);
	CheckCrushGage(tDamageParams);
	CheckHP(tDamageParams);
	
	if (m_pEMUI != nullptr)
		m_pEMUI->Create_DamageFont(tDamageParams);

	if (dynamic_cast<CPlayer*>(tDamageParams.pCauser) != nullptr)
		m_dCombetTime = 0.0;

	ENEMY_DAMAGE_REPORT tReport;
	tReport.pCauser = tDamageParams.pCauser;
	tReport.pTaker = this;
	tReport.eName = m_eEnemyName;
	tReport.eStat = GetEnemyBatchDataStat();
	tReport.iTakeDamage = tDamageParams.iDamage;
	tReport.eAttackSAS = tDamageParams.eAttackSAS;
	if (m_ePreDeBuff != m_eDeBuff)
		tReport.eBeDeBuff = m_eDeBuff;
	tReport.eKineticAtkType = tDamageParams.eKineticAtkType;
	tReport.eAttackType = tDamageParams.eAttackType;
	tReport.bDead = false;
	tReport.bHitWeak = m_bHitWeak;

	CGameManager::GetInstance()->ConsumeEnemyDamageReport(tReport);
}


void CEnemy::SetEnemyBatchDataStat(ENEMY_STAT tStat)
{
	//m_iMaxHP = tStat.iMaxHP;
	//m_iHP = m_iMaxHP;
	//m_iMaxCrushGauge = tStat.iMaxCrushGage;
	//m_iCrushGauge = m_iMaxCrushGauge;
	//m_bHasCrushGauge = tStat.bHasCrushGage;
	//m_iAtkDamage = tStat.iAtkDamage;
	//iEemeyLevel = tStat.iLevel;
}

ENEMY_STAT CEnemy::GetEnemyBatchDataStat()
{
	ENEMY_STAT tStat;
	tStat.iMaxHP = m_iMaxHP;
	tStat.iMaxCrushGage = m_iMaxCrushGauge;
	tStat.bHasCrushGage = m_bHasCrushGauge;
	tStat.iAtkDamage = m_iAtkDamage;
	tStat.iLevel = iEemeyLevel;
	return tStat;
}

void CEnemy::SetDelete()
{
	CScarletCharacter::SetDelete();

	ENEMY_DAMAGE_REPORT tReport;
	tReport.pCauser = m_pTarget;
	tReport.pTaker = this;
	tReport.eName = m_eEnemyName;
	tReport.eStat = GetEnemyBatchDataStat();
	tReport.iTakeDamage = 1;
	tReport.eAttackSAS = ESASType::SAS_END;
	tReport.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_END;
	tReport.eAttackType = EAttackType::ATK_END;
	tReport.bDead = true;
	tReport.bHitWeak = false;

	CGameManager::GetInstance()->ConsumeEnemyDamageReport(tReport);
}

void CEnemy::Add_RigidBody(const string & KeyName, void * pArg)
{
	CRigidBody* pRigidBody = nullptr;

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), s2ws(KeyName).c_str(),
		(CComponent**)&pRigidBody, pArg));

	m_pRigidBodies.emplace(KeyName, pRigidBody);
}

CRigidBody * CEnemy::GetRigidBody(const string & KeyName)
{
	auto pRigidBody = m_pRigidBodies.find(KeyName);
	assert(pRigidBody != m_pRigidBodies.end() && "Wrong RigidBody KeyName!");

	return (*pRigidBody).second;
}

void CEnemy::CreateSpawnEffect()
{
	CUI_Manager::GetInstance()->PlaySound("EM_Spawn");

	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"Enemy_Spawn_A")
		->Start_NoAttachPivot(this, m_SpawnEffectPivot, false, false);

	_matrix	SocketMatrix = m_SpawnEffectPivot * m_pTransformCom->Get_WorldMatrix();
	_vector vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_vector vToCam = XMVector3NormalizeEst(vCamPos - SocketMatrix.r[3]) * m_fSpawnDistortionDistancePivot;
	vToCam = XMVectorSetW(vToCam, 0.f);
	SocketMatrix.r[3] += vToCam;

	_matrix DistortionPivotMatrix = SocketMatrix * m_pTransformCom->Get_WorldMatrix_Inverse();

	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"Enemy_Spawn_B")
		->Start_NoAttachPivot(this, DistortionPivotMatrix, false, false);
}

_bool CEnemy::InCombet_WithPlayer()
{
	return	m_bBoss || m_dCombetTime < 5.0;
}


_bool CEnemy::IsTargetFront(_float fAngle)
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fLookRadian = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyLook), XMVector3Normalize(vTargetPos - vMyPos)));
	
	if (fLookRadian > cosf(XMConvertToRadians(fAngle)))
		return true;

	return false;
}



_bool CEnemy::IsTargetRight(_float fAngle)
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	_float fRightRadian = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyRight), XMVector3Normalize(vTargetPos - vMyPos)));

	if (fRightRadian > cosf(XMConvertToRadians(fAngle)))
		return true;

	return false;
}


void CEnemy::SetDead()
{
	if (m_bDead)
		return;

	m_bDead = true;
	m_DeathTimeline.PlayFromStart();
	
	//UI 삭제
	Safe_Release(m_pEMUI);

}

void CEnemy::FindTarget(_double TimeDelta)
{
	m_dCombetTime += TimeDelta;

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
		auto pPlayer = CGameInstance::GetInstance()->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pObj)
		{
				return dynamic_cast<CPlayer*>(pObj) != nullptr;
		}, PLATERTEST_LAYER_PLAYER);

		m_pTarget = dynamic_cast<CScarletCharacter*>(pPlayer);

		if (InCombet_WithPlayer()) return;

		CLayer* pAILayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, LAYER_AI);
		
		if (pAILayer == nullptr) return;
	
		_vector vMyPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		auto AIList = pAILayer->GetGameObjects();

		_float fMinDist = XMVectorGetX(XMVector4LengthEst(vMyPosition - pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

		for (auto AI : AIList)
		{
			_float fAIDist = XMVectorGetX(XMVector4LengthEst(vMyPosition - AI->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

			if (fAIDist < fMinDist)
			{
				m_pTarget = dynamic_cast<CScarletCharacter*>(AI);
				fMinDist = fAIDist;
			}
		}
	}
}

void CEnemy::PlayBC()
{
	m_bBrainCrush = true;

	//떠있는 모든 유아이 삭제
	Safe_Release(m_pEMUI);
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

void CEnemy::CheckHitPositoin(DAMAGE_PARAM& tDamageParams)
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

}

void CEnemy::HitEffect(DAMAGE_PARAM& tDamageParams)
{
	wstring HitBloodName;
	wstring HitEffectName;
	wstring HitDecalName;

	switch (tDamageParams.eAttackSAS)
	{
	case ESASType::SAS_FIRE:
		HitBloodName = s_vecFireBlood[CMathUtils::RandomUInt(s_vecFireBlood.size() - 1)];
		HitEffectName = s_vecFireHit[CMathUtils::RandomUInt(s_vecFireHit.size() - 1)];
		HitDecalName = s_vecFireDecal[CMathUtils::RandomUInt(s_vecFireDecal.size() - 1)];
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, L"Player_Fire_Sword_Particle")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
//		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Player_Sas_Fire_Sword_Particle"))->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
		break;

	case ESASType::SAS_ELETRIC:
		HitBloodName = s_vecElecBlood[CMathUtils::RandomUInt(s_vecElecBlood.size() - 1)];
		HitEffectName = s_vecElecHit[CMathUtils::RandomUInt(s_vecElecHit.size() - 1)];
		HitDecalName = s_vecElecDecal[CMathUtils::RandomUInt(s_vecElecDecal.size() - 1)];
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ELEC_ATTACK, L"Player_Elec_Sword_Particle")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
		break;

	case ESASType::SAS_NOT:
		HitBloodName = s_vecDefaultBlood[CMathUtils::RandomUInt(s_vecDefaultBlood.size() - 1)];
		HitEffectName = s_vecDefaultHit[CMathUtils::RandomUInt(s_vecDefaultHit.size() - 1)];
		HitDecalName = s_vecDefaultDecal[CMathUtils::RandomUInt(s_vecDefaultDecal.size() - 1)];
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, L"Player_Default_Sword_Particle")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
		break;
	}

	if(m_bHitWeak)
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0220_Weak_Hit_EF")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
	if (HitBloodName.empty() == false)
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitBloodName)->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
	if (HitEffectName.empty() == false)
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitEffectName)->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);
	if (HitDecalName.empty() == false)
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitDecalName)->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);

	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Circle_Distortion_NonFlip_Short")->Start_AttachPosition(this, tDamageParams.vHitPosition, tDamageParams.vSlashVector);

	if (tDamageParams.eAttackType == EAttackType::ATK_HEAVY ||
		tDamageParams.eAttackType == EAttackType::ATK_SPECIAL_LOOP ||
		tDamageParams.eAttackType == EAttackType::ATK_SPECIAL_END)
	{
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, L"Default_Kinetic_Dead_Effect_00")->Start_AttachPosition(this, GetColliderPosition(), {0.f, 0.1f, 0.f, 0.f});
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 fPos = vPos;

	if (m_strImpactTag.empty() == false)
		m_SoundStore.PlaySound(m_strImpactTag, &fPos);
	if (m_strImpactVoiceTag.empty() == false)
		m_SoundStore.PlaySound(m_strImpactVoiceTag, &fPos);
}

void CEnemy::CheckDeBuff(EDeBuffType eDeBuff)
{

	switch (eDeBuff)
	{
	case EDeBuffType::DEBUFF_FIRE:
		if (m_eDeBuff == EDeBuffType::DEBUFF_OIL
			|| CGameUtils::GetRandFloat() > m_fFireResist)
			m_eDeBuff = EDeBuffType::DEBUFF_FIRE;
		break;
	case EDeBuffType::DEBUFF_OIL:
		m_eDeBuff = EDeBuffType::DEBUFF_OIL;
		break;
	case EDeBuffType::DEBUFF_THUNDER:
		if (m_eDeBuff == EDeBuffType::DEBUFF_WATER
			|| CGameUtils::GetRandFloat() > m_fThunderResist)
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
	if (m_bHasCrushGauge)
	{
		_int iDamage = tDamageParams.iDamage;
		// if (m_bHitWeak)
			// iDamage *= 2;

		if (tDamageParams.eAttackSAS == ESASType::SAS_HARDBODY)
			iDamage *= 3;

		switch (m_eCurAttackType)
		{
		case EAttackType::ATK_LIGHT: break;
		case EAttackType::ATK_SPECIAL_LOOP: break;
		case EAttackType::ATK_MIDDLE: break;

		case EAttackType::ATK_HEAVY:
			FALLTHROUGH;
		case EAttackType::ATK_SPECIAL_END:
			iDamage *= 1.3;
			break;
		case EAttackType::ATK_TO_AIR: break;
		case EAttackType::ATK_DOWN: break;
		case EAttackType::ATK_END: break;
		default: 
			NODEFAULT;
		}

		m_iCrushGauge -= iDamage*1.2;
		if (m_iCrushGauge < 0)
		{
			if (m_bCrushStart == false)
				m_pEMUI->Create_CGUI();
		
			m_iCrushGauge = 0;
			m_bCrushStart = true;
		}
			
	}	
}

void CEnemy::CheckHP(DAMAGE_PARAM& tDamageParams)
{
	//true가 됐다는건 플레이어가 G키를 눌러 브레인 크러쉬를 실행했다는거.
	//브레인 크러쉬 애니메이션에 맞춰서 SetDead 함수를 실행시켜 줌
	if (m_bCrushStart == true) return;

	_int iDamage = tDamageParams.iDamage;

	 if (m_bHitWeak)
	 	iDamage *= 1.2f;

	m_iHP -= iDamage;
	if (m_iHP < 0)
	{
		//if (m_iCrushGauge > 0)
		SetDead();
		m_iHP = 0;
	}
}

_bool CEnemy::CheckSASType(ESASType eSASType)
{
	auto lsSAS = CPlayerInfoManager::GetInstance()->Get_PlayerSasList();

	for (auto SAS : lsSAS)
	{
		if (eSASType == SAS)
			return true;
	}

	return false;
}

void CEnemy::ResetHitData()
{
	m_eCurAttackType = EAttackType::ATK_END;
	m_eHitFrom = EBaseAxis::AXIS_END;
	m_eSimpleHitFrom = ESimpleAxis::AXIS_END;
	m_bHitWeak = false;
	m_bAirToDown = false;
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
	if (m_pEMUI != nullptr)
		m_pEMUI->Update_NoticeNeon();

	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 0;
	}

}

void CEnemy::DeBuff_Fire()
{
	m_fDeBuffTime = 8.f;
	
	if(m_pEMUI != nullptr)
		m_pEMUI->Update_NoticeNeon();

	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 1;
	}
}

void CEnemy::DeBuff_Oil()
{
	m_fDeBuffTime = 10.f;

	if (m_pEMUI != nullptr)
		m_pEMUI->Update_NoticeNeon();

	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		pMtrl->GetParam().Ints[0] = 2;
	}
}

void CEnemy::DeBuff_Thunder()
{
	m_fDeBuffTime = 6.f;
	if (m_pEMUI != nullptr)
		m_pEMUI->Update_NoticeNeon();
}

void CEnemy::DeBuff_Water()
{
	m_fDeBuffTime = 10.f;

	if (m_pEMUI != nullptr)
		m_pEMUI->Update_NoticeNeon();
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

void CEnemy::SocketLocalMove(CEnemy_AnimInstance * pASM)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vLocalMove = m_pModelCom->GetLocalMove(WorldMatrix, pASM->GetCurSocketAnimName());
	m_pTransformCom->LocalMove(vLocalMove);
}

void CEnemy::SocketLocalMove_Range(CEnemy_AnimInstance* pASM, _float fRange)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vLocalMove = m_pModelCom->GetLocalMove(WorldMatrix, pASM->GetCurSocketAnimName());
	m_pTransformCom->LocalMove(vLocalMove, fRange);
}

void CEnemy::Free()
{
	CScarletCharacter::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);

	for (auto it : m_pRigidBodies)
		Safe_Release(it.second);

	m_pRigidBodies.clear();

	Safe_Release(m_pEMUI);	
}
