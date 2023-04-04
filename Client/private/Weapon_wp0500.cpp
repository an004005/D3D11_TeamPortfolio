#include "stdafx.h"
#include "Weapon_wp0500.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "RigidBody.h"
#include "TrailSystem.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "Enemy.h"

CWeapon_wp0500::CWeapon_wp0500(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CScarletWeapon(pDevice, pContext)
{
}

CWeapon_wp0500::CWeapon_wp0500(const CWeapon_wp0500& rhs)
	:CScarletWeapon(rhs)
{
}

HRESULT CWeapon_wp0500::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_wp0500::Initialize(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

void CWeapon_wp0500::BeginTick()
{
}

void CWeapon_wp0500::Tick(_double TimeDelta)
{
	m_fTimeDelta = TimeDelta;

	__super::Tick(TimeDelta);

	//m_pModel->SetPivot(XMMatrixScaling(0.01f, 0.01f, 0.01f));

//	_float4 vCurPos = { m_pCollider->GetPxWorldMatrix().Translation().x, m_pCollider->GetPxWorldMatrix().Translation().y, m_pCollider->GetPxWorldMatrix().Translation().z, 1.f };
//	m_vSlashVector = vCurPos - m_vBeforePos;

//	m_vBeforePos = vCurPos;
}

void CWeapon_wp0500::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CWeapon_wp0500::AfterPhysX()
{
}

HRESULT CWeapon_wp0500::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

HRESULT CWeapon_wp0500::SetUp_Components()
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRenderer));

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
	}

	return S_OK;
}

CWeapon_wp0500* CWeapon_wp0500::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_wp0500* pInstance = new CWeapon_wp0500(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_wp0500");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWeapon_wp0500::Clone(void* pArg)
{
	CWeapon_wp0500* pInstance = new CWeapon_wp0500(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_wp0500");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWeapon_wp0500::Free()
{
	__super::Free();
}
