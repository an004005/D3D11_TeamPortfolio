#include "stdafx.h"
#include "..\public\ScarletCharacter.h"
#include "ControlledRigidBody.h"
#include "RigidBody.h"
#include "PhysX_Manager.h"

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

void CScarletCharacter::Collision_Check(CRigidBody * AttackTrigger, DAMAGE_PARAM DamageParam, ECOLLIDER_TYPE_BIT eColType)
{
	// 공격자의 트리거와 공격 파라미터를 전달받아서 진행
	static Vector4 BeforePos;

	Matrix ColliderWorld = AttackTrigger->GetPxWorldMatrix();
	_float4 vPos = _float4(ColliderWorld.Translation().x, ColliderWorld.Translation().y, ColliderWorld.Translation().z, 1.f);
	_float3 vLook = _float3(ColliderWorld.Up().x, ColliderWorld.Up().y, ColliderWorld.Up().z);

	physx::PxSweepHit hitBuffer[4];
	physx::PxSweepBuffer overlapOut(hitBuffer, 4);
	PxCapsuleSweepParams param;
	param.sweepOut = &overlapOut;
	param.CapsuleGeo = AttackTrigger->Get_CapsuleGeometry();
	param.pxTransform = AttackTrigger->Get_PxTransform();

	_float4	vWeaponDir = vPos - BeforePos;

	param.vUnitDir = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);
	param.fDistance = param.vUnitDir.Length();
	param.iTargetType = eColType;
	param.fVisibleTime = 0.f;

	if (CGameInstance::GetInstance()->PxSweepCapsule(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				DAMAGE_PARAM tParam;
				tParam.iDamage = DamageParam.iDamage;
				tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				tParam.eAttackType = DamageParam.eAttackType;
				pTarget->TakeDamage(DamageParam);
			}
		}
	}

	BeforePos = vPos;
}

void CScarletCharacter::Free()
{
	CGameObject::Free();
	Safe_Release(m_pCollider);
	Safe_Release(m_pGameInstance);
}
