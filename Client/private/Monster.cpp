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

	//HP UI
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CMonsterHpUI* pUI_HP = nullptr;
	pUI_HP = dynamic_cast<CMonsterHpUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterHP")));

	assert(pUI_HP != nullptr);
	pUI_HP->Set_Owner(this);
	pUI_HP->SetPivotMatrix(m_UI_PivotMatrixes[INFO_BAR]);

	return S_OK;
}

void CMonster::Tick(_double TimeDelta)
{
	CScarletCharacter::Tick(TimeDelta);
	Update_DeadDissolve(TimeDelta);
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

void CMonster::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	m_UI_PivotMatrixes.fill(XMMatrixIdentity());

	if (json.contains("UIPivotMatrixes"))
	{
		m_UI_PivotMatrixes = json["UIPivotMatrixes"];
	}
}

void CMonster::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	//if (tDamageParams.vHitPosition.Length() == 0.f)
	//{
	//	// 타격점이 원점으로 찍히면 레이캐스트를 쏴서 위치를 판단
	//	if (tDamageParams.pCauser != nullptr)
	//	{
	//		_vector vRayPos = static_cast<CScarletCharacter*>(tDamageParams.pCauser)->GetColliderPosition();
	//		vRayPos = XMVectorSetW(vRayPos, 1.f);
	//		_vector vRayDir = GetColliderPosition() - static_cast<CScarletCharacter*>(tDamageParams.pCauser)->GetColliderPosition();
	//		vRayDir = XMVectorSetW(vRayDir, 0.f);

	//		physx::PxRaycastHit hitBuffer[1];
	//		physx::PxRaycastBuffer rayOut(hitBuffer, 1);

	//		RayCastParams param;
	//		param.rayOut = &rayOut;
	//		param.vOrigin = vRayPos;
	//		param.vDir = vRayDir;
	//		param.fDistance = 10.f;
	//		param.iTargetType = CTB_MONSTER;
	//		param.fVisibleTime = 5.f;
	//		param.bSingle = true;
	//		if (CGameInstance::GetInstance()->RayCast(param))
	//		{
	//			for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
	//			{
	//				auto pHit = rayOut.getAnyHit(i);
	//				CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
	//				if (auto pMonster = dynamic_cast<CMonster*>(pCollidedObject))
	//				{
	//					_vector vHitPos = XMVectorSet(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
	//					_vector vEffectDir = tDamageParams.vSlashVector;
	//					CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, L"Default_Blood_00")->Start_AttachPosition(this, vHitPos, vEffectDir);
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		// 타격점이 잘 나오면 해당 위치에 생성
	//		_vector vHitPos = tDamageParams.vHitPosition;
	//		_vector vEffectDir = tDamageParams.vSlashVector;
	//		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, L"Default_Blood_00")->Start_AttachPosition(this, vHitPos, vEffectDir);
	//	}
	//}
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
