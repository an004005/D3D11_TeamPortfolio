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
		L"Collider", (CComponent**)&m_pCollider, pArg));

	m_bActiveGravity = true;
	return S_OK;
}

void CScarletCharacter::Late_Tick(_double TimeDelta)
{
	// 점프하고 싶을 때는 m_fYSpeed에 + 값을 넣어준다.
	// if (m_pGameInstance->KeyDown(DIK_SPACE))
	// {
	// 	m_fYSpeed = 10.f;
	// }

	CGameObject::Late_Tick(TimeDelta);
	const _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	const _vector vPrePos = m_vPrePos;
	const _vector vMoveDelta = vPos - vPrePos;

	// transform 변동사항 적용
	physx::PxControllerCollisionFlags flags = m_pCollider->MoveDisp(vMoveDelta, (_float)TimeDelta);
	m_bOnSide = flags & physx::PxControllerCollisionFlag::eCOLLISION_SIDES;

	// 중력 이동
	flags = m_pCollider->Move(_float4{0.f, m_fYSpeed, 0.f, 0.f}, (_float)TimeDelta);
	m_bOnFloor = flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
	if (m_bOnFloor || !m_bActiveGravity)
	{
		m_fYSpeed = 0.f;
	}
	else
	{
		m_fYSpeed -= m_fGravity * (_float)TimeDelta;
	}
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
