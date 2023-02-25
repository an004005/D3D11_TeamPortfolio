#include "stdafx.h"
#include "..\public\Indicator.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "RigidBody.h"

CIndicator::CIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CIndicator::CIndicator(const CIndicator& rhs)
	: CGameObject(rhs)
{
}

HRESULT CIndicator::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CIndicator::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_Collider"),
		(CComponent**)&m_pCollider));

	return S_OK;
}

void CIndicator::Late_Tick(_double TimeDelta)
{
	m_pCollider->Update_Tick(m_pTransformCom);
}

void CIndicator::AfterPhysX()
{
	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

void CIndicator::SetPosition(_float3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPos, 1.f));
}

CIndicator* CIndicator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CIndicator*		pInstance = new CIndicator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CIndicator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CIndicator::Clone(void* pArg)
{
	CIndicator*		pInstance = new CIndicator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CIndicator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CIndicator::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);
}
