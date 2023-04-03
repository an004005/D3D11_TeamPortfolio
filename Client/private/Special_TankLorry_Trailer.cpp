#include "stdafx.h"
#include "..\public\Special_TankLorry_Trailer.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"

#include "PhysX_Manager.h"
#include "Enemy.h"

#include "MathUtils.h"
#include "ParticleGroup.h"

CSpecial_TankLorry_Trailer::CSpecial_TankLorry_Trailer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_TankLorry_Trailer::CSpecial_TankLorry_Trailer(const CSpecial_TankLorry_Trailer & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_TankLorry_Trailer::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_TankLorry_Trailer::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_TANKLORRY_TRAILER;

	return S_OK;
}

void CSpecial_TankLorry_Trailer::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Kinetic(true);
	m_pCollider->UpdateChange();
}

void CSpecial_TankLorry_Trailer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_TankLorry_Trailer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_TankLorry_Trailer::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_TankLorry_Trailer::Render()
{
	FAILED_CHECK(__super::Render());

	const _matrix TankMatrix = m_TankMatrix * m_pTransformCom->Get_WorldMatrix();
	FAILED_CHECK(m_pModelComs[m_iTankIdx]->Render(TankMatrix));

	return S_OK;
}

void CSpecial_TankLorry_Trailer::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];

	if (json.contains("TankMatrix"))
		m_TankMatrix = json["TankMatrix"];
}

void CSpecial_TankLorry_Trailer::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;

	json["TankMatrix"] = m_TankMatrix;
}

void CSpecial_TankLorry_Trailer::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("TankMatrix"))
	{
		static GUIZMO_INFO Tank;
		CImguiUtils::Render_Guizmo(&m_TankMatrix, Tank, true, true);
	}

	if (ImGui::CollapsingHeader("LocalMatrix"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_TankLorry_Trailer::Set_ChildMatrix(_float4x4 matrix)
{
	m_SyncMatrix = matrix;
}

void CSpecial_TankLorry_Trailer::Sync_position(CTransform * pTransform)
{
	m_pTransformCom->Set_WorldMatrix(m_SyncMatrix * pTransform->Get_WorldMatrix());
}

void CSpecial_TankLorry_Trailer::Set_Kinetic(_bool bKinetic)
{
	m_pCollider->Set_Kinetic(bKinetic);
	m_pCollider->UpdateChange();
}

void CSpecial_TankLorry_Trailer::Activate(_bool bActivate)
{
	m_pCollider->Activate(bActivate);
}

void CSpecial_TankLorry_Trailer::Shake(_float fRange)
{
	_float3 vTorque = {
		CMathUtils::RandomFloat(-fRange, fRange),
		CMathUtils::RandomFloat(-fRange, fRange),
		CMathUtils::RandomFloat(-fRange, fRange) };

	m_pCollider->AddTorque(vTorque);
}

void CSpecial_TankLorry_Trailer::Bounce(_float fForce)
{
	_float3 vVelocity = { 0.f, fForce * g_fTimeDelta, 0.f };

	m_pCollider->AddVelocity(vVelocity);
}

void CSpecial_TankLorry_Trailer::Exploision()
{
	_float3 vVelocity = { 0.f, 300.f * g_fTimeDelta, 0.f };

	m_pCollider->AddVelocity(vVelocity);

	physx::PxSweepHit hitBuffer[4];
	physx::PxSweepBuffer overlapOut(hitBuffer, 4);
	SphereSweepParams params2;
	params2.sweepOut = &overlapOut;
	params2.fRadius = 20.f;
	params2.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	params2.vUnitDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	params2.fDistance = 0.f;
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
				tParam.eAttackType = EAttackType::ATK_SPECIAL_END;
				tParam.eDeBuff = EDeBuffType::DEBUFF_FIRE;
				tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
				tParam.iDamage = 9999;

				tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				pMonster->TakeDamage(tParam);
			}
		}
	}

	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Truck_Explode_Rect")
		->Start_AttachPosition_Scale(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 2.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f), { 3.f ,3.f ,3.f, 0.f });
}

void CSpecial_TankLorry_Trailer::Change_Tank(_uint iIdx)
{
	m_iTankIdx = iIdx;
}

void CSpecial_TankLorry_Trailer::Create_Oil_Particle()
{
	//Truck_Oil_Particle_Loop
	for (_uint i = 0; i < 5; ++i)
	{
		if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pOil[i]))
		{
			m_pOil[i] = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Truck_Oil_Particle_Loop");
			m_pOil[i]->Start_AttachPosition_Scaling(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 
				XMVectorSet(CMathUtils::RandomFloat(-1.f, 1.f),  1.f, CMathUtils::RandomFloat(-1.f, 1.f), 0.f), _float4(1.f, 1.f, 1.f, 0.f), false);
		}
	}
}

void CSpecial_TankLorry_Trailer::Release_Oil_Particle()
{
	for (_uint i = 0; i < 5; ++i)
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pOil[i]))
		{
			m_pOil[i]->Delete_Particles();
			m_pOil[i] = nullptr;
		}
	}
}

HRESULT CSpecial_TankLorry_Trailer::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_TankLorry_Trailer");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Trailer", L"Model_TankLorry_Trailer", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_TankLorry_Tank_0");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Tank_0", L"Model_TankLorry_Tank_0", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_TankLorry_Tank_1");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Tank_1", L"Model_TankLorry_Tank_1", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_TankLorry_Tank_2");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Tank_2", L"Model_TankLorry_Tank_2", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_TankLorry_Tank_3");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Tank_3", L"Model_TankLorry_Tank_3", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_TankLorry_Trailer * CSpecial_TankLorry_Trailer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_TankLorry_Trailer* pInstance = new CSpecial_TankLorry_Trailer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_TankLorry_Trailer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_TankLorry_Trailer::Clone(void * pArg)
{
	CSpecial_TankLorry_Trailer* pInstance = new CSpecial_TankLorry_Trailer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_TankLorry_Trailer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_TankLorry_Trailer::Free()
{
	__super::Free();
}
