#include "stdafx.h"
#include "Weapon_wp0190.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "RigidBody.h"
#include "TrailSystem.h"
#include "Monster.h"
#include "PhysX_Manager.h"
#include "Material.h"

CWeapon_wp0190::CWeapon_wp0190(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CScarletWeapon(pDevice, pContext)
{
}

CWeapon_wp0190::CWeapon_wp0190(const CWeapon_wp0190 & rhs)
	:CScarletWeapon(rhs)
{
}

HRESULT CWeapon_wp0190::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_wp0190::Initialize(void * pArg)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	m_pCollider->SetOnTriggerOut([this](CGameObject* pGameObject)
	{
		if (auto pTarget = dynamic_cast<CMonster*>(pGameObject))
		{
			pTarget->Set_CollisionDuplicate(false);
			IM_LOG("Test");
		}
	});

	m_pCollider->UpdateChange();

	Json AttackMesh = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/Trail/PlayerSwordTrail.json");
	m_pTrail = static_cast<CTrailSystem*>(pGameInstance->Clone_GameObject_Get(L"Layer_Player", TEXT("ProtoVFX_TrailSystem"), &AttackMesh));

	return S_OK;
}

void CWeapon_wp0190::BeginTick()
{
	//CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Player")->GetGameObjects())
	//{
	//	if (iter->GetPrototypeTag() == TEXT("CamSpot"))
	//	{
	//		m_pCamSpot = iter;
	//	}
	//}
}

void CWeapon_wp0190::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pCollider->Update_Tick(m_pTransformCom);

	m_pTrail->GetTransform()
		->Set_WorldMatrix(m_pCollider->GetPxWorldMatrix());

	m_pTrail->SetActive(m_bTrailOn);
	
	//m_pTrail->Tick(TimeDelta);

	if (m_bBright)
	{
		switch (m_eSasType)
		{
		case ESASType::SAS_NOT:
			m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 1;
			break;
		case ESASType::SAS_FIRE:
			m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 2;
			break;
		default:
			m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 0;
			break;
		}
	}
	else
	{
		m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 0;
	}
	
	//Collision_Check();
}

void CWeapon_wp0190::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pTrail->Late_Tick(TimeDelta);
}

void CWeapon_wp0190::AfterPhysX()
{
	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CWeapon_wp0190::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

//	m_pTrail->Render();

	return S_OK;
}

HRESULT CWeapon_wp0190::SetUp_Components()
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	if (m_Desc.m_pJson)
	{
		Json& json = *static_cast<Json*>(m_Desc.m_pJson);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModel));

		}

		Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Trigger", (CComponent**)&m_pCollider, (void*)m_Desc.m_pJson);

		//m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
		//{
		//	if (auto pMonster = dynamic_cast<CMonster*>(pGameObject))
		//	{
		//		DAMAGE_PARAM tParam;
		//		tParam.iDamage = 1;
		//		tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		//		pMonster->TakeDamage(tParam);
		//	}
		//});
	}

	return S_OK;
}

CWeapon_wp0190 * CWeapon_wp0190::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeapon_wp0190* pInstance = new CWeapon_wp0190(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_wp0190");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CWeapon_wp0190::Clone(void * pArg)
{
	CWeapon_wp0190* pInstance = new CWeapon_wp0190(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_wp0190");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWeapon_wp0190::Free()
{
	__super::Free();
}
