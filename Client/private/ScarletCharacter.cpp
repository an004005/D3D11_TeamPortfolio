#include "stdafx.h"
#include "..\public\ScarletCharacter.h"
#include "ControlledRigidBody.h"

CScarletCharacter::CScarletCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CScarletCharacter::CScarletCharacter(const CScarletCharacter& rhs)
	: CGameObject(rhs)
{
}

HRESULT CScarletCharacter::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_ControlledRigidBody", 
		L"Collider", (CComponent**)m_pCollider, pArg));

	return S_OK;
}

void CScarletCharacter::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	const _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	const _vector vPrePos = m_vPrePos;
	const _vector vMoveDelta = vPos - vPrePos;
	m_pCollider->MoveDisp(vMoveDelta, (_float)TimeDelta);
}

void CScarletCharacter::AfterPhysX()
{
	CGameObject::AfterPhysX();
	const _float4 vColliderFootPos = m_pCollider->GetFootPosition();
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vColliderFootPos);
	m_vPrePos = vColliderFootPos;
}

void CScarletCharacter::Free()
{
	CGameObject::Free();
	Safe_Release(m_pCollider);
	Safe_Release(m_pGameInstance);
}
