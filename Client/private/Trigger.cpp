#include "stdafx.h"
#include "Trigger.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "SkummyPandou.h"
#include "SkummyPool.h"
#include "FlowerLeg.h"
#include "BuddyLumi.h"
#include "BronJon.h"
#include "ImGuizmo.h"

CTrigger::CTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrigger::CTrigger(const CTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT Client::CTrigger::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT Client::CTrigger::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));
		
	FAILED_CHECK(SetUp_Components(pArg));
	
	return S_OK;
}

void Client::CTrigger::BeginTick()
{
	__super::BeginTick();
}

void Client::CTrigger::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pRigidBodyCom->Update_Tick(m_pTransformCom);
}

void Client::CTrigger::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CTrigger::AfterPhysX()
{
	__super::AfterPhysX();
	m_pRigidBodyCom->Update_AfterPhysX(m_pTransformCom);
}

HRESULT Client::CTrigger::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void Client::CTrigger::Imgui_RenderProperty()
{

}

void CTrigger::SetMonster(const _tchar* ProtoTag, _fmatrix WorldMatrix)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

	m_pRigidBodyCom->SetOnTriggerIn([&](CGameObject* pGameObject) {

		pGameObject = pGameInstance->Clone_GameObject_Get(TEXT("Layer_Monster"), ProtoTag);
		pGameObject->GetTransform()->Set_WorldMatrix(WorldMatrix);

	});

}

HRESULT CTrigger::SetUp_Components(void * pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_Ridgid"),
		(CComponent**)&m_pRigidBodyCom, pArg));

	m_pRigidBodyCom->Set_Trigger();

	return S_OK;
}

CTrigger * CTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrigger* pInstance = new CTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* Client::CTrigger::Clone(void* pArg /*= nullptr*/)
{
	CTrigger* pInstance = new CTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void Client::CTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBodyCom);
}
