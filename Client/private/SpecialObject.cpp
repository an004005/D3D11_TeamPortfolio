#include "stdafx.h"
#include "..\public\SpecialObject.h"
#include "Model.h"
#include "RigidBody.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "Monster.h"
#include "Player.h"
#include "ImguiUtils.h"
#include "GameUtils.h"
#include "Material.h"
#include "GravikenisisMouseUI.h"
#include "VFX_Manager.h"
#include "PhysX_Manager.h"
#include "Enemy.h"
#include "ScarletCharacter.h"
#include "PlayerInfoManager.h"

CSpecialObject::CSpecialObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CSpecialObject::CSpecialObject(const CSpecialObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpecialObject::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecialObject::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("InitPos"))
		{
			_float4 InitPos = json["InitPos"];
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&InitPos));
		}
	}

	FAILED_CHECK(SetUp_Components(pArg));

	return S_OK;
}

void CSpecialObject::BeginTick()
{
	__super::BeginTick();

	/*for (auto& Model : m_pModelComs)
	{
		for (auto& iter : Model->GetMaterials())
		{
			iter->GetParam().Floats[0] = 0.f;
		}
	}*/

	if (!m_pModelComs.empty())
	{
		for (auto& iter : m_pModelComs)
		{
			for (auto& pMtrl : iter->GetMaterials())
			{
				if (pMtrl->GetParam().Floats.empty())
				{
					pMtrl->GetParam().Floats.push_back(0.f);
				}

				if (1 == pMtrl->GetParam().Floats.size())
				{
					pMtrl->GetParam().Floats.push_back(0.f);
				}
			}
		}
	}

//	m_pCollider->UpdateChange();
}

void CSpecialObject::Tick(_double TimeDelta)
{
	m_fTimeDelta = (_float)TimeDelta;

	__super::Tick(TimeDelta);

	m_fParticleCoolTime += TimeDelta;
	if (2.f <= m_fParticleCoolTime)
	{
		m_ParticleMakeable.Reset();
		m_fParticleCoolTime = 0.f;
	}

	if (false == m_bUseCheck)
	{
		OutlineMaker();
	}
	else
	{
		m_bOutline = false;

		for (auto& Model : m_pModelComs)
		{
			for (auto& iter : Model->GetMaterials())
			{
				if (0 == iter->GetParam().Ints.size())
				{
					iter->GetParam().Ints.push_back(0);
				}
				else
				{
					iter->GetParam().Ints[0] = 0;
				}
			}
		}
	}
	BrightChecker();
	DissolveChecker();

	if (!m_pModelComs.empty())
	{
		for (auto& iter : m_pModelComs)
		{
			for (auto pMtrl : iter->GetMaterials())
			{
				pMtrl->GetParam().Floats[0] = m_fBright;
				pMtrl->GetParam().Floats[1] = m_fDissolve;
			}
		}
	}
}

void CSpecialObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		for (auto pModel : m_pModelComs)
			pModel->Tick(TimeDelta);
	}
}

void CSpecialObject::AfterPhysX()
{
	__super::AfterPhysX();

//	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecialObject::Render()
{
	FAILED_CHECK(__super::Render());

	//if(m_eCurModelTag != Tag_End)
	//{
		const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
		//FAILED_CHECK(m_pModelComs[m_eCurModelTag]->Render(WorldMatrix));
		FAILED_CHECK(m_pModelComs[0]->Render(WorldMatrix));
	//}

	return S_OK;
}

HRESULT CSpecialObject::Render_ShadowDepth()
{
	//if (m_eCurModelTag != Tag_End)
	//{
		const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
		//m_pModelComs[m_eCurModelTag]->Render_ShadowDepth(WorldMatrix);
		m_pModelComs[0]->Render_ShadowDepth(WorldMatrix);
	//}

	return S_OK;
}

void CSpecialObject::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
}

void CSpecialObject::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
}

void CSpecialObject::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	//if (ImGui::CollapsingHeader("Local Matrix Edit"))
	//{
	//	static GUIZMO_INFO tInfo;
	//	CImguiUtils::Render_Guizmo(&m_LocalMatrix, tInfo, true, true);
	//}


	//if (ImGui::Button("Kinetic Object Reset"))
	//{
	//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -30.f, 0.f, 1.f));
	//	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(180.f) });

	//	m_pCollider->Activate(true);
	//	m_pCollider->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
	//	m_pCollider->Set_Kinetic(true);
	//	m_pCollider->UpdateChange();
	//}

	//if (ImGui::BeginListBox("ModelTagList"))
	//{
	//	_uint i = 0;
	//	for (auto tag : m_pModelTags)
	//	{
	//		const bool bSelected = (tag == m_strModelTag);

	//		if (bSelected)
	//			ImGui::SetItemDefaultFocus();

	//		char pStr[MAX_PATH];
	//		strcpy(pStr, CGameUtils::GetFileName(ws2s(tag)).c_str());

	//		if (ImGui::Selectable(pStr, bSelected))
	//		{
	//			m_strModelTag = tag;
	//			//m_eCurModelTag = (KineticModeltag)i;
	//		}

	//		++i;
	//	}
	//	ImGui::EndListBox();
	//}
}

_float4 CSpecialObject::GetPxPostion()
{
	auto pxPos = m_pCollider->Get_PxTransform().p;

	return _float4{ pxPos.x, pxPos.y, pxPos.z, 1.f };
}

void CSpecialObject::CreateKineticParticle(_float4 vPos, _float4 vScale)
{
	if (m_ParticleMakeable.IsNotDo())
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, m_vecRandomParticle[CMathUtils::RandomUInt(m_vecRandomParticle.size() - 1)])
			->Start_AttachPosition_Scaling(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMLoadFloat4(&vPos), _float4(0.f, 1.f, 0.f, 0.f), vScale);
	}
}

void CSpecialObject::CreateKineticParticle_Mini(_float4 vPos, _float4 vScale)
{
	if (m_ParticleMakeable.IsNotDo())
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, m_vecRandomParticle_Small[CMathUtils::RandomUInt(m_vecRandomParticle_Small.size() - 1)])
			->Start_AttachPosition_Scaling(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMLoadFloat4(&vPos), _float4(0.f, 1.f, 0.f, 0.f), vScale);
	}
}

_bool CSpecialObject::Collision_Check_Capsule(CRigidBody * AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollisionCheck)
{
	if (!bCollisionCheck)
	{
		m_DamagedObjectList.clear();
		return false;
	}

	Matrix ColliderWorld = AttackTrigger->GetPxWorldMatrix();
	_float4 vPos = _float4(ColliderWorld.Translation().x, ColliderWorld.Translation().y, ColliderWorld.Translation().z, 1.f);
	_float3 vLook = _float3(ColliderWorld.Up().x, ColliderWorld.Up().y, ColliderWorld.Up().z);

	physx::PxSweepHit hitBuffer[4];
	physx::PxSweepBuffer overlapOut(hitBuffer, 4);
	PxCapsuleSweepParams param;
	param.sweepOut = &overlapOut;
	param.CapsuleGeo = AttackTrigger->Get_CapsuleGeometry();
	param.pxTransform = AttackTrigger->Get_PxTransform();

	_float4	vWeaponDir = vPos - m_BeforePos;

	param.vUnitDir = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);
	param.fDistance = param.vUnitDir.Length();
	param.iTargetType = CTB_MONSTER | CTB_MONSTER_PART | CTB_MONSTER_RANGE;
	param.fVisibleTime = 0.1f;

	_bool	bCollisionResult = false;	// 충돌이 일어났는가?

	if (CGameInstance::GetInstance()->PxSweepCapsule(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				_bool bDamagedTarget = true; // 충돌이 가능한 타겟인가?
				for (auto Dupliciation = m_DamagedObjectList.begin(); Dupliciation != m_DamagedObjectList.end();)
				{
					if ((*Dupliciation) == pTarget)
					{
						if (false == (*Dupliciation)->Get_CollisionDuplicate())
						{
							Dupliciation = m_DamagedObjectList.erase(Dupliciation);
						}
						else
						{
							Dupliciation++;
							bDamagedTarget = false;
						}
					}
					else
					{
						Dupliciation++;
					}
				}

				if (bDamagedTarget)
				{
					bCollisionResult = true;

					DAMAGE_PARAM tParam;
					ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
					memcpy(&tParam, &DamageParam, sizeof(DAMAGE_PARAM));

					// 내부에서 자체적으로 계산할 값
					tParam.pCauser = nullptr;
					tParam.pContactComponent = CPhysXUtils::GetComponent(pHit.actor);
					tParam.vHitNormal = _float4(pHit.normal.x, pHit.normal.y, pHit.normal.z, 0.f);
					tParam.vHitPosition = _float4(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					tParam.vSlashVector = _float4(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z, 0.f);


					// 플레이어일 경우 타격 이펙트 생성하도록
					pTarget->TakeDamage(tParam);
					pTarget->Set_CollisionDuplicate(true, ECOPYCOLTYPE::COPYCOL_MAIN);

					IM_LOG(ws2s(pTarget->GetPrototypeTag()).c_str());

					// 이미 충돌했던 대상을 리스트에 추가
					m_DamagedObjectList.push_back(pTarget);

					//플레이어가 공격했을때 몬스터에 target ui 생성
					/*if (dynamic_cast<CPlayer*>(tParam.pCauser) != nullptr)
					dynamic_cast<CPlayer*>(tParam.pCauser)->Create_TargetInfoBar(pTarget);*/
				}
			}
		}
	}

	m_BeforePos = vPos;

	return bCollisionResult;
}

void CSpecialObject::OutlineMaker()
{
	if (m_bOutline)
	{
		for (auto& Model : m_pModelComs)
		{
			for (auto& iter : Model->GetMaterials())
			{
				if (0 == iter->GetParam().Ints.size())
				{
					iter->GetParam().Ints.push_back(0);
				}
				else
				{
					iter->GetParam().Ints[0] = 1;
				}
			}
		}
	}
	else
	{
		for (auto& Model : m_pModelComs)
		{
			for (auto& iter : Model->GetMaterials())
			{
				if (0 == iter->GetParam().Ints.size())
				{
					iter->GetParam().Ints.push_back(0);
				}
				else
				{
					iter->GetParam().Ints[0] = 0;
				}
			}
		}
	}
}

void CSpecialObject::BrightChecker()
{
	if (m_bRimFix)
	{
		m_fBright = 1.f;
	}
	else if (CPlayerInfoManager::GetInstance()->Get_SpecialObject() == this)
	{
		_float fCharge = CPlayerInfoManager::GetInstance()->Get_SpecialCharge();

		if (m_fBright <= fCharge)
		{
			m_fBright = fCharge;
		}
		else if (m_bBrightChange)
		{
			m_bBrightChange = false;
		}
		else
		{
			m_fBright = 0.f;
		}
	}
	else
	{
		if (false == m_bRimFix && 0.f < m_fBright && false == m_bBrightChange)
		{
			m_fBright = 0.f;
		}
		else
		{
			m_bBrightChange = false;
		}
	}
}

void CSpecialObject::DissolveChecker()
{
	if (m_bDissolve)
		m_fDissolve = min(m_fDissolve + m_fTimeDelta, 1.f);
}

HRESULT CSpecialObject::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	//m_eCurModelTag = Tag_default;

	return S_OK;
}

void CSpecialObject::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	
	for (auto& iter : m_pModelComs)
		Safe_Release(iter);
	m_pModelComs.clear();

	m_pModelTags.clear();

	Safe_Release(m_pCollider);
}
