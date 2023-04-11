#include "stdafx.h"
#include "..\public\Special_Container.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Enemy.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "PlayerInfoManager.h"
#include "MapObject.h"

CSpecial_Container::CSpecial_Container(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_Container::CSpecial_Container(const CSpecial_Container & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_Container::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_Container::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECAIL_CONTAINER;

	m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
	{
		if (auto pState = dynamic_cast<CMapObject*>(pGameObject))
		{
			m_bCollision = true;

			//m_iModelIndex += 1;
		}

		if (auto pMonster = dynamic_cast<CEnemy*>(pGameObject))
		{
			if (m_bCollision) return;

			m_bCollision = true;
			m_bAddAble = true;

			m_iModelIndex += 1;

			DAMAGE_PARAM tParam;
			ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
			tParam.eAttackSAS = ESASType::SAS_END;
			tParam.eAttackType = EAttackType::ATK_SPECIAL_END;
			tParam.eDeBuff = EDeBuffType::DEBUFF_END;
			tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
			tParam.iDamage = 500;
			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			Container_Input_Damage(tParam);

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Special_G_HBeam")
				->Start_AttachOnlyPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), false);

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Large_Hit_Particle")->
				Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - XMVectorSet(0.f, 2.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f), false);

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, m_vecRandomParticle[CMathUtils::RandomUInt(m_vecRandomParticle.size() - 1)])
				->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 3.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f), false);

			CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.3f);
			//CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack_Special");
		}
	});

	return S_OK;
}

void CSpecial_Container::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Kinetic(true);
	m_pCollider->UpdateChange();
}

void CSpecial_Container::Tick(_double TimeDelta)
{
	if (m_bDeadCheck)
	{
		SpecialRimLightFix(false);

		m_fDeadTime -= (_float)TimeDelta;

		if (1.f >= m_fDeadTime)
		{
			Set_Dissolve(true);
		}

		if (0.f >= m_fDeadTime)
			this->SetDelete();
	}

	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_Container::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_Container::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_Container::Render()
{
	//FAILED_CHECK(__super::Render());

	const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
	FAILED_CHECK(m_pModelComs[min(m_iModelIndex, 5)]->Render(WorldMatrix));

	return S_OK;
}

void CSpecial_Container::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_Container::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_Container::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Local"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}

	ImGui::InputInt("ModelIndex", &m_iModelIndex);
}

void CSpecial_Container::Container_ChangeIndex(_uint iIndex)
{
	if (m_iModelIndex == iIndex) return;

	m_iModelIndex = iIndex;

	DAMAGE_PARAM tParam;
	ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
	tParam.eAttackSAS = ESASType::SAS_END;
	tParam.eAttackType = EAttackType::ATK_SPECIAL_LOOP;
	tParam.eDeBuff = EDeBuffType::DEBUFF_END;
	tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
	tParam.iDamage = 300;
	tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	Container_Input_Damage(tParam);

	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, L"Default_Kinetic_Dead_Effect_00")->Start_AttachPosition(this, tParam.vHitFrom, { 0.f, 1.f, 0.f, 0.f });
}

void CSpecial_Container::Lerp_to_TargetPoint(_float4 vTargetPos, _float fRatio)
{
	_float4 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer rayOut(hitBuffer, 1);

	RayCastParams param;
	param.rayOut = &rayOut;
	param.vOrigin = vTargetPos + _float4(0.f, 5.f, 0.f, 0.f);
	param.vDir = _float4(0.f, -1.f, 0.f, 0.f);
	param.fDistance = 10.f;
	param.iTargetType = CTB_MONSTER | CTB_MONSTER_PART;
	param.bSingle = true;
	param.fVisibleTime = 1.f;

	if (CGameInstance::GetInstance()->RayCast(param))
	{
		for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
		{
			auto pHit = rayOut.getAnyHit(i);

			_float4 vTargetHeight = _float4(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
			vTargetHeight += _float4(0.f, 5.f, 0.f, 0.f);

			_float4 vLerpPos = XMVectorLerp(vCurPos, vTargetHeight, min(fRatio, 1.f));

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
		}
	}
}

void CSpecial_Container::Container_Press()
{
	if (m_pCollider->IsKinematic())
	{
		m_pCollider->Set_Kinetic(false);
		m_pCollider->UpdateChange();
	}
	else
	{
		return;
	}

	m_pCollider->AddVelocity({ 0.f, -20.f, 0.f });
}

void CSpecial_Container::Container_Press_Maintain(_float4 vTargetPos, _double TimeDelta)
{
	if (!m_pCollider->IsTrigger())
	{
		m_pCollider->Set_Trigger(true);
		//m_pCollider->UpdateChange();
	}

	_float4 vSourPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	// 레이캐스트로 콜라이더 위에 올리기

	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer rayOut(hitBuffer, 1);

	RayCastParams param;
	param.rayOut = &rayOut;
	param.vOrigin = vTargetPos + _float4(0.f, 5.f, 0.f, 0.f);
	param.vDir = _float4(0.f, -1.f, 0.f, 0.f);
	param.fDistance = 10.f;
	param.iTargetType = CTB_MONSTER | CTB_MONSTER_PART;
	param.bSingle = true;
	param.fVisibleTime = 1.f;

	if (CGameInstance::GetInstance()->RayCast(param))
	{
		for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
		{
			auto pHit = rayOut.getAnyHit(i);

			_float4 vTargetHeight = _float4(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
			//vTargetHeight += _float4(0.f, .f, 0.f, 0.f);

			_float4 vLerpPos = XMVectorLerp(vSourPos, vTargetHeight, min(m_fLerpTime += TimeDelta, 1.f));

			_float4 vRandomShakeDir = _float4(CMathUtils::RandomFloat(-0.1f, 0.1f), CMathUtils::RandomFloat(-0.1f, 0.1f), CMathUtils::RandomFloat(-0.1f, 0.1f), 0.f);

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos + vRandomShakeDir);
		}
	}
	// ~레이캐스트로 콜라이더 위에 올리기
}

void CSpecial_Container::Container_Press_Finish()
{
	if (m_pCollider->IsTrigger())
	{
		m_pCollider->Set_Trigger(false);
		//m_pCollider->UpdateChange();
		m_bCollision = false;
	}
	else
	{
		return;
	}

	m_pCollider->AddVelocity({ 0.f, -50.f, 0.f });

	m_bDeadCheck = true;
	m_fDeadTime = 5.f;
}

void CSpecial_Container::Container_Reposition(CTransform* pTransform, _float fRatio, _float fForce)
{
	if (!m_pCollider->IsTrigger())
	{
		m_pCollider->Set_Trigger(true);
		//m_pCollider->UpdateChange();
	}


	_vector vSourScale, vSourRotation, vSourTrans;
	XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourTrans, m_pTransformCom->Get_WorldMatrix());

	_vector vDestScale, vDestRotation, vDestTrans;
	XMMatrixDecompose(&vDestScale, &vDestRotation, &vDestTrans, pTransform->Get_WorldMatrix());

	_vector vLerpRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, min(fRatio, 1.f));
	_matrix matResult = XMMatrixAffineTransformation(vSourScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLerpRotation, vSourTrans + XMVectorSet(0.f, fForce * 0.1f, 0.f, 0.f));

	m_pTransformCom->Set_WorldMatrix(matResult);
}

void CSpecial_Container::Container_SetDead()
{
	m_bDeadCheck = true;
	m_fDeadTime = 5.f;
}

void CSpecial_Container::Container_Input_Damage(DAMAGE_PARAM tParam)
{
	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer rayOut(hitBuffer, 1);

	RayCastParams param;
	param.rayOut = &rayOut;
	param.vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	param.vDir = _float4(0.f, -1.f, 0.f, 0.f);
	param.fDistance = 10.f;
	param.iTargetType = CTB_MONSTER | CTB_MONSTER_PART;
	param.bSingle = true;
	param.fVisibleTime = 1.f;

	if (CGameInstance::GetInstance()->RayCast(param))
	{
		for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
		{
			auto pHit = rayOut.getAnyHit(i);

			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);

			if (auto pMonster = dynamic_cast<CEnemy*>(pCollidedObject))
			{
				pMonster->TakeDamage(tParam);
			}
		}
	}
}

HRESULT CSpecial_Container::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;

	m_pModelTags.push_back(L"Model_Container_01");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Container_01", L"Model_Container_01", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_Container_02");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Container_02", L"Model_Container_02", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_Container_03");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Container_03", L"Model_Container_03", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_Container_04");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Container_04", L"Model_Container_04", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_Container_05");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Container_05", L"Model_Container_05", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_Container_06");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Container_06", L"Model_Container_06", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_Container * CSpecial_Container::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_Container* pInstance = new CSpecial_Container(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_Container");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_Container::Clone(void * pArg)
{
	CSpecial_Container* pInstance = new CSpecial_Container(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_Container");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_Container::Free()
{
	__super::Free();
}
