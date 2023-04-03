#include "stdafx.h"
#include "..\public\TriggerEx.h"
#include "GameInstance.h"
#include "RigidBody.h"

CTriggerEx::CTriggerEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTriggerEx::CTriggerEx(const CTriggerEx& rhs)
	: CGameObject(rhs)	
{
}

HRESULT CTriggerEx::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_Ridgid"),
		(CComponent**)&m_pRigidBodyCom, pArg));

	m_pRigidBodyCom->Set_Trigger(true);
	m_pRigidBodyCom->Set_ColliderType(CT_TRIGGER_FOR_PLAYER);

	m_pRigidBodyCom->SetOnTriggerIn([this](CGameObject* pObject)
	{
		if (CheckTriggeredObject(pObject))
		{
			m_TriggeredObjects.insert(pObject);
			TriggerInEvent(pObject);
		}
	});
	m_pRigidBodyCom->SetOnTriggerOut([this](CGameObject* pObject)
	{
		// CheckTriggeredObject로 통과되서 TriggeredObjects에 들어온 오브젝트가 나갈 때만 실행
		const auto itr = m_TriggeredObjects.find(pObject);
		if (itr != m_TriggeredObjects.end())
		{
			m_TriggeredObjects.erase(pObject);
			TriggerOutEvent(pObject);
		}
	});

	m_pRigidBodyCom->UpdateChange();

	return S_OK;
}

void CTriggerEx::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	m_pRigidBodyCom->Update_Tick(m_pTransformCom);
}

void CTriggerEx::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRigidBodyCom);
}
