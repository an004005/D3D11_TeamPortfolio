#include "stdafx.h"
#include "..\public\Special_HBeam_Bundle.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Special_HBeam_Single.h"
#include "MapObject.h"
#include "Enemy.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "PlayerInfoManager.h"

CSpecial_HBeam_Bundle::CSpecial_HBeam_Bundle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_HBeam_Bundle::CSpecial_HBeam_Bundle(const CSpecial_HBeam_Bundle & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_HBeam_Bundle::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_HBeam_Bundle::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_HBEAM_BUNDLE;

	// 태어날 때 부터 8개 들고 태어남
	Create_ChildBeam();

	m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
	{
		if (!m_bThrow)
			return;

		if (!m_bRenderOption)
			return;

		if (auto pStatic = dynamic_cast<CMapObject*>(pGameObject))
		{
			HBeam_Decompose();
		}

		if (auto pMonster = dynamic_cast<CEnemy*>(pGameObject))
		{
			m_bAddAble = true;

			DAMAGE_PARAM tParam;
			ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
			tParam.eAttackSAS = ESASType::SAS_END;
			tParam.eAttackType = EAttackType::ATK_SPECIAL_END;
			tParam.eDeBuff = EDeBuffType::DEBUFF_END;
			tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
			tParam.iDamage = 500;
			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			static_cast<CEnemy*>(pMonster)->TakeDamage(tParam);

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Special_G_HBeam")
				->Start_AttachOnlyPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), false);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_HBeam_Particles")->
				Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), XMVectorSet(0.f, 1.f, 0.f, 0.f), false);

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, m_vecRandomHitEffect[CMathUtils::RandomUInt(m_vecRandomHitEffect.size() - 1)])
				->Start_AttachOnlyPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 1.f, 0.f, 0.f), false);

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_DEFAULT_ATTACK, m_vecRandomParticle[CMathUtils::RandomUInt(m_vecRandomParticle.size() - 1)])
				->Start_AttachPosition_Scaling(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), _float4(0.f, 1.f, 0.f, 0.f), { 1.f, 1.f ,1.f, 0.f });

			HBeam_Decompose();

			CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.3f);
			CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack_Special");
		}
	});

	m_SoundStore.CloneSound("fx_kine_super_jail_exp");
	m_SoundStore.CloneSound("fx_kine_super_pole_multi_imp");

	return S_OK;
}

void CSpecial_HBeam_Bundle::BeginTick()
{
	__super::BeginTick();

	m_pCollider->UpdateChange();
}

void CSpecial_HBeam_Bundle::Tick(_double TimeDelta)
{
	if (m_bUseCheck)
	{
		for (auto& iter : m_pHBeam_Single)
			static_cast<CSpecialObject*>(iter)->Set_Used();
	}

	if (m_bDeadCheck)
	{
		m_fDeadTime -= (_float)TimeDelta;

		for (auto& iter : m_pHBeam_Single)
			static_cast<CSpecialObject*>(iter)->SpecialRimLightFix(false);

		if (1.f >= m_fDeadTime)
		{
			for (auto& iter : m_pHBeam_Single)
				static_cast<CSpecialObject*>(iter)->Set_Dissolve(true);
		}

		if (0.f >= m_fDeadTime)
		{
			for (auto& iter : m_pHBeam_Single)
				iter->SetDelete();

			this->SetDelete();
		}
	}

	//CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Special_G_HBeam")
	//	->Start_AttachOnlyPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), false);
	//CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_HBeam_Particles")->
	//	Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), XMVectorSet(0.f, 1.f, 0.f, 0.f), false);
//	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_HBeam_Particles")->
//		Start_AttachPosition_Scaling(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), XMVectorSet(0.f, 1.f, 0.f, 0.f), _float4(10.f, 10.f, 10.f, 0.f), false);

	if (m_bRenderOption)
	{
		__super::Tick(TimeDelta);
		m_pCollider->Update_Tick(m_pTransformCom);

		for (auto& iter : m_pHBeam_Single)
		{
			static_cast<CSpecial_HBeam_Single*>(iter)->Sync_position(m_pTransformCom);
			static_cast<CSpecial_HBeam_Single*>(iter)->Set_Bright(m_fBright);
			static_cast<CSpecial_HBeam_Single*>(iter)->SpecialRimLightFix(m_bRimFix);
		}
	}
	else
	{
		if (tt.IsNotDo())
		{
			//Safe_Release(m_pCollider);

			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			for (auto& iter : m_pHBeam_Single)
			{
				static_cast<CSpecial_HBeam_Single*>(iter)->Activate(true);
				static_cast<CSpecial_HBeam_Single*>(iter)->Set_Bright(m_fBright);
				static_cast<CSpecial_HBeam_Single*>(iter)->SpecialRimLightFix(m_bRimFix);
			}
		}

		m_fCollisionTime += (_float)TimeDelta;

		m_pCollider->Activate(false);
		for (auto& iter : m_pHBeam_Single)
			iter->Tick(TimeDelta);
	}

	if (m_bAddAble && !m_bDeadCheck)
	{
		for (auto& iter : m_pHBeam_Single)
		{
			_float4 vDir = static_cast<CSpecial_HBeam_Single*>(iter)->GetOriginDir();
			_float fLength = vDir.Length();

			if (fLength > 0.f)
			{
				vDir.Normalize();
				vDir.w = 0.f;
				vDir *= -2.f;
				static_cast<CSpecial_HBeam_Single*>(iter)->AddVelocity({ vDir.x, vDir.y - 0.1f, vDir.z });
			}
		}
	}
}

void CSpecial_HBeam_Bundle::Late_Tick(_double TimeDelta)
{
	if (m_bRenderOption)
	{
		__super::Late_Tick(TimeDelta);
	}
	else
	{
		for (auto& iter : m_pHBeam_Single)
			iter->Late_Tick(TimeDelta);
	}
}

void CSpecial_HBeam_Bundle::AfterPhysX()
{
	if (m_bRenderOption)
	{
		__super::AfterPhysX();
		m_pCollider->Update_AfterPhysX(m_pTransformCom);
	}
	else
	{
		for (auto& iter : m_pHBeam_Single)
			iter->AfterPhysX();
	}
}

HRESULT CSpecial_HBeam_Bundle::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_HBeam_Bundle::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_HBeam_Bundle::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_HBeam_Bundle::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::InputFloat("ThrowPower", &m_fThrowPower);
	ImGui::InputFloat("FloatPower", &m_fFloatPower);

	if (ImGui::CollapsingHeader("Local"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_HBeam_Bundle::HBeam_Drift()
{
	_float3 vForce = _float3(0.f, m_fFloatPower * g_fTimeDelta, 0.f);
	_float3 vTorque = _float3(CMathUtils::RandomFloat(-1.f, 1.f), CMathUtils::RandomFloat(-1.f, 1.f), CMathUtils::RandomFloat(-1.f, 1.f));
	m_pCollider->AddVelocity(vForce);
	m_pCollider->AddTorque(vTorque);
}

void CSpecial_HBeam_Bundle::HBeam_Throw(_float4 vDir)
{
	if (m_bThrow) return;

	m_bThrow = true;

	_float3 vForce = { vDir.x, vDir.y, vDir.z };
	_float3 vTorque = { 0.f, 2000.f, 0.f };
	vForce.Normalize();

	vForce *= m_fThrowPower;

	m_pCollider->AddVelocity(vForce);
	m_pCollider->AddTorque(vTorque);
}

void CSpecial_HBeam_Bundle::HBeam_Decompose()
{
	m_bRenderOption = false;

	m_SoundStore.PlaySound("fx_kine_super_jail_exp", m_pTransformCom);

	//for (auto& iter : m_pHBeam_Single)
	//	static_cast<CSpecial_HBeam_Single*>(iter)->Set_Kinetic(false);
}

void CSpecial_HBeam_Bundle::HBeam_Single_Catch()
{
	for (auto& iter : m_pHBeam_Single)
		static_cast<CSpecial_HBeam_Single*>(iter)->Set_Trigger(true);
}

void CSpecial_HBeam_Bundle::HBeam_Single_Turn()
{
	for (auto& iter : m_pHBeam_Single)
	{
		static_cast<CSpecial_HBeam_Single*>(iter)->HBeam_Turn();
		//static_cast<CSpecial_HBeam_Single*>(iter)->CreateKineticParticle();
	}
}

void CSpecial_HBeam_Bundle::HBeam_Single_Finish()
{
	for (auto& iter : m_pHBeam_Single)
		static_cast<CSpecial_HBeam_Single*>(iter)->HBeam_Finish();

	HBeam_Explosion();
	HBeam_SetDeadTimer();
}

void CSpecial_HBeam_Bundle::HBeam_Single_SetTrigger(_bool bKinetic)
{
	for (auto& iter : m_pHBeam_Single)
		static_cast<CSpecial_HBeam_Single*>(iter)->Set_Trigger(bKinetic);
}

void CSpecial_HBeam_Bundle::HBeam_Slower()
{
	for (auto& iter : m_pHBeam_Single)
	{
		static_cast<CSpecial_HBeam_Single*>(iter)->Set_Trigger(false);
		_float4 vDir = static_cast<CSpecial_HBeam_Single*>(iter)->GetOriginDir();
		vDir.Normalize();
		vDir.w = 0.f;
		static_cast<CSpecial_HBeam_Single*>(iter)->AddVelocity({vDir.x, vDir.y, vDir.z});
	}
}

void CSpecial_HBeam_Bundle::HBeam_Collision()
{
	if (m_fCollisionTime >= 0.5f)
	{
		m_SoundStore.PlaySound("fx_kine_super_pole_multi_imp", m_pTransformCom);

		physx::PxSweepHit hitBuffer[4];
		physx::PxSweepBuffer overlapOut(hitBuffer, 4);
		SphereSweepParams params2;
		params2.sweepOut = &overlapOut;
		params2.fRadius = 10.f;
		params2.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 20.f, 0.f, 0.f);
		params2.vUnitDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		params2.fDistance = 20.f;
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
					tParam.eAttackSAS = ESASType::SAS_END;
					tParam.eAttackType = EAttackType::ATK_SPECIAL_LOOP;
					tParam.eDeBuff = EDeBuffType::DEBUFF_END;
					tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
					tParam.iDamage = 50;

					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					pMonster->TakeDamage(tParam);
				}
			}
		}

		m_fCollisionTime = 0.f;
	}
}

void CSpecial_HBeam_Bundle::HBeam_Explosion()
{
	m_SoundStore.PlaySound("fx_kine_super_pole_multi_imp", m_pTransformCom);

	physx::PxSweepHit hitBuffer[4];
	physx::PxSweepBuffer overlapOut(hitBuffer, 4);
	SphereSweepParams params2;
	params2.sweepOut = &overlapOut;
	params2.fRadius = 10.f;
	params2.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 20.f, 0.f, 0.f);
	params2.vUnitDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	params2.fDistance = 20.f;
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
				tParam.eAttackSAS = ESASType::SAS_END;
				tParam.eAttackType = EAttackType::ATK_SPECIAL_END;
				tParam.eDeBuff = EDeBuffType::DEBUFF_END;
				tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
				tParam.iDamage = 250;

				tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				pMonster->TakeDamage(tParam);
			}
		}
	}
}

void CSpecial_HBeam_Bundle::HBeam_SetDeadTimer()
{
	for (auto& iter : m_pHBeam_Single)
	{
		static_cast<CSpecial_HBeam_Single*>(iter)->Set_Trigger(false);
	}

	m_bDeadCheck = true;
	m_fDeadTime = 3.f;
}

HRESULT CSpecial_HBeam_Bundle::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_HBeam_Bundle");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_HBeam_Bundle", L"Model_HBeam_Bundle", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

void CSpecial_HBeam_Bundle::Create_ChildBeam()
{
	wstring wstrLayer = L"Layer_Kinetic";
	string strJsonPath = "../Bin/Resources/Objects/KineticSpecial_HBeam_Single.json";
	wstring wstrProtoTag = L"Prototype_GameObject_Special_HBeam_Single";

	auto pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, wstrProtoTag.c_str());
	if (pProtoType == nullptr)
		pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_STATIC, wstrProtoTag.c_str());

	if (pProtoType == nullptr)
	{
		MSG_BOX("ProtoType is not exist");
	}
	else if (wstrLayer.empty())
	{
		MSG_BOX("Layer is empty");
	}
	else if (strJsonPath.empty() == false && CGameUtils::FileExist(s2ws(strJsonPath).c_str()) == false)
	{
		MSG_BOX("Json file is not exist");
	}
	else
	{
		for (auto& iter : m_pHBeam_Single)
		{
			if (strJsonPath.empty() == false)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
				iter = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
			}
			else
			{
				iter = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
			}
			iter->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		}
	}
}

CSpecial_HBeam_Bundle * CSpecial_HBeam_Bundle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_HBeam_Bundle* pInstance = new CSpecial_HBeam_Bundle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_HBeam_Bundle");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_HBeam_Bundle::Clone(void * pArg)
{
	CSpecial_HBeam_Bundle* pInstance = new CSpecial_HBeam_Bundle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_HBeam_Bundle");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_HBeam_Bundle::Free()
{
	__super::Free();

	for (auto& iter : m_pHBeam_Single)
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(iter))
			Safe_Release(iter);
	}
}