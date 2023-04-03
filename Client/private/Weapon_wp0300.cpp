#include "stdafx.h"
#include "Weapon_wp0300.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "RigidBody.h"
#include "TrailSystem.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "Enemy.h"

CWeapon_wp0300::CWeapon_wp0300(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CScarletWeapon(pDevice, pContext)
{
}

CWeapon_wp0300::CWeapon_wp0300(const CWeapon_wp0300& rhs)
	: CScarletWeapon(rhs)
{
}

HRESULT CWeapon_wp0300::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_wp0300::Initialize(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	m_pCollider->SetOnTriggerOut([this](CGameObject* pGameObject)
		{
			//if (auto pTarget = dynamic_cast<CMonster*>(pGameObject))
			if (auto pTarget = dynamic_cast<CEnemy*>(pGameObject))
			{
				pTarget->Set_CollisionDuplicate(false);
				//IM_LOG("Test");
			}
		});

	m_pCollider->UpdateChange();

	//Json AttackMesh = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/Trail/PlayerSwordTrail.json");
	//m_pTrail = static_cast<CTrailSystem*>(pGameInstance->Clone_GameObject_Get(L"Layer_Player", TEXT("ProtoVFX_TrailSystem"), &AttackMesh));

	return S_OK;
}

void CWeapon_wp0300::BeginTick()
{
	m_pModel->SetPlayAnimation("AS_wp0300_000_02_long_loop");
	m_pModel->Play_Animation(0.f);
}

void CWeapon_wp0300::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pCollider->Update_Tick(m_pTransformCom);

	m_pModel->Play_Animation(TimeDelta);

	//m_pTrail->GetTransform()
	//	->Set_WorldMatrix(m_pCollider->GetPxWorldMatrix());

	//m_pTrail->SetActive(/*m_bTrailOn*/true);

	_float4 vCurPos = { m_pCollider->GetPxWorldMatrix().Translation().x, m_pCollider->GetPxWorldMatrix().Translation().y, m_pCollider->GetPxWorldMatrix().Translation().z, 1.f };
	m_vSlashVector = vCurPos - m_vBeforePos;

	/*if (m_bBright)
	{
		switch (m_eSasType)
		{
		case ESASType::SAS_NOT:
			m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 1;
			break;
		case ESASType::SAS_FIRE:
			m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 2;
			break;
		case ESASType::SAS_ELETRIC:
			m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 3;
			break;
		default:
			m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 0;
			break;
		}
	}
	else
	{
		m_pModel->FindMaterial(L"MI_wp0190_SWORD")->GetParam().Ints[0] = 0;
	}*/

	//Collision_Check();

	m_vBeforePos = vCurPos;
}

void CWeapon_wp0300::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	//m_pTrail->Late_Tick(TimeDelta);
}

void CWeapon_wp0300::AfterPhysX()
{
	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CWeapon_wp0300::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

void CWeapon_wp0300::SetFire()
{
}

void CWeapon_wp0300::ReleaseFire()
{
}

HRESULT CWeapon_wp0300::SetUp_Components()
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
	}

	return S_OK;
}

CWeapon_wp0300* CWeapon_wp0300::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_wp0300* pInstance = new CWeapon_wp0300(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_wp0300");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWeapon_wp0300::Clone(void* pArg)
{
	CWeapon_wp0300* pInstance = new CWeapon_wp0300(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_wp0300");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWeapon_wp0300::Free()
{
	__super::Free();
}
