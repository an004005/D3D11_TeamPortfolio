#include "stdafx.h"
#include "..\public\TestTarget.h"
#include "ControlledRigidBody.h"


CTestTarget::CTestTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CScarletCharacter(pDevice, pContext)
{
}

CTestTarget::CTestTarget(const CScarletCharacter& rhs)
	:CScarletCharacter (rhs)
{
}

void CTestTarget::BeginTick()
{
	CScarletCharacter::BeginTick();
	m_pCollider->SetActive(false);
}

void CTestTarget::Tick(_double TimeDelta)
{
	CScarletCharacter::Tick(TimeDelta);
}

void CTestTarget::AfterPhysX()
{
	CScarletCharacter::AfterPhysX();
}

CTestTarget* CTestTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestTarget* pInstance = new CTestTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTestTarget::Clone(void* pArg)
{
	CTestTarget*		pInstance = new CTestTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestTarget::Free()
{
	CScarletCharacter::Free();
}
