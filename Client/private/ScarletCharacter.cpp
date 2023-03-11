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

void CScarletCharacter::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	Update_DeBuff(TimeDelta);
}

void CScarletCharacter::Late_Tick(_double TimeDelta)
{
	// �����ϰ� ���� ���� m_fYSpeed�� + ���� �־��ش�.
	// if (m_pGameInstance->KeyDown(DIK_SPACE))
	// {
	// 	m_fYSpeed = 10.f;
	// }

	CGameObject::Late_Tick(TimeDelta);
	const _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	const _vector vPrePos = m_vPrePos;
	const _vector vMoveDelta = vPos - vPrePos;

	// transform �������� ����
	physx::PxControllerCollisionFlags flags = m_pCollider->MoveDisp(vMoveDelta, (_float)TimeDelta);
	m_bOnSide = flags & physx::PxControllerCollisionFlag::eCOLLISION_SIDES;

	// �߷� �̵�
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

void CScarletCharacter::Update_DeBuff(_double TimeDelta)
{
	if (m_fDeBuffTime > 0.f)
	{
		m_fDeBuffTime -= (_float)TimeDelta;
		if (m_fDeBuffTime <= 0.f)
			m_eDeBuff = EDeBuffType::DEBUFF_END;
	}

	if (m_ePreDeBuff != m_eDeBuff)
	{
		switch(m_eDeBuff)
		{
		case EDeBuffType::DEBUFF_FIRE:
			DeBuff_Fire();
			break;
		case EDeBuffType::DEBUFF_OIL:
			DeBuff_Oil();
			break;
		case EDeBuffType::DEBUFF_THUNDER: 
			break;
		case EDeBuffType::DEBUFF_WATER:
			break;
		case EDeBuffType::DEBUFF_END:
			DeBuff_End();
			break;
		default:
			NODEFAULT;
		}
	}

	m_ePreDeBuff = m_eDeBuff;
}

void CScarletCharacter::Collision_Check_Capsule(CRigidBody * AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollision ,ECOLLIDER_TYPE_BIT ColType, _bool bContinuity)
{
	// 1���� : ������ٵ�
	// 2���� : ������ ����
	// 3���� : �浹 ����, �Լ��� ��� ������ �浹�������� �����Ÿ� false����
	// 4���� : Ÿ�� Ÿ��

	if (!bCollision && !bContinuity)
	{
		m_DamagedObjectList.clear();
		return;
	}

	// �������� Ʈ���ſ� ���� �Ķ���͸� ���޹޾Ƽ� ����

	Matrix ColliderWorld = AttackTrigger->GetPxWorldMatrix();
	_float4 vPos = _float4(ColliderWorld.Translation().x, ColliderWorld.Translation().y, ColliderWorld.Translation().z, 1.f);
	_float3 vLook = _float3(ColliderWorld.Up().x, ColliderWorld.Up().y, ColliderWorld.Up().z);

	physx::PxSweepHit hitBuffer[4];
	physx::PxSweepBuffer overlapOut(hitBuffer, 4);
	PxCapsuleSweepParams param;
	param.sweepOut = &overlapOut;
	param.CapsuleGeo = AttackTrigger->Get_CapsuleGeometry();
	param.pxTransform = AttackTrigger->Get_PxTransform();

	_float4	vWeaponDir = vPos - m_BeforePos;

	param.vUnitDir = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);
	param.fDistance = param.vUnitDir.Length();
	param.iTargetType = ColType;
	param.fVisibleTime = 0.f;

	m_iHitTargetCount = 0.;

	if (CGameInstance::GetInstance()->PxSweepCapsule(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				_bool bDamagedTarget = true;

				if (bContinuity == false)
				{
					for (auto& iter : m_DamagedObjectList)
					{
						if (iter == pTarget)
						{
							bDamagedTarget = false;
							break;
						}
					}
				}

				if (bDamagedTarget)
				{
					DAMAGE_PARAM tParam;
					memcpy(&tParam, &DamageParam, sizeof(DAMAGE_PARAM));

					// ���ο��� ��ü������ ����� ��
					tParam.pCauser = this;
					tParam.pContactComponent = CPhysXUtils::GetComponent(pHit.actor);
					tParam.vHitNormal = _float3(pHit.normal.x, pHit.normal.y, pHit.normal.z);
					tParam.vHitPosition = _float3(pHit.position.x, pHit.position.y, pHit.position.z);
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					
					pTarget->TakeDamage(tParam);
					++m_iHitTargetCount;
					
					// �̹� �浹�ߴ� ����� ����Ʈ�� �߰�
					m_DamagedObjectList.push_back(pTarget);
				}
			}
		}
	}

	m_BeforePos = vPos;
}

_bool CScarletCharacter::Collision_Check_Capsule_Improved(CRigidBody * AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollisionCheck, ECOLLIDER_TYPE_BIT ColType)
{
	if (!bCollisionCheck)
	{
		m_DamagedObjectList.clear();
		return false;
	}

	Matrix ColliderWorld = AttackTrigger->GetPxWorldMatrix();
	_float4 vPos = _float4(ColliderWorld.Translation().x, ColliderWorld.Translation().y, ColliderWorld.Translation().z, 1.f);
	_float3 vLook = _float3(ColliderWorld.Up().x, ColliderWorld.Up().y, ColliderWorld.Up().z);

	physx::PxSweepHit hitBuffer[4];
	physx::PxSweepBuffer overlapOut(hitBuffer, 4);
	PxCapsuleSweepParams param;
	param.sweepOut = &overlapOut;
	param.CapsuleGeo = AttackTrigger->Get_CapsuleGeometry();
	param.pxTransform = AttackTrigger->Get_PxTransform();

	_float4	vWeaponDir = vPos - m_BeforePos;

	param.vUnitDir = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);
	param.fDistance = param.vUnitDir.Length();
	param.iTargetType = ColType;
	param.fVisibleTime = 0.f;

	// �ߺ� �浹 �Ǵ�
	/*for (auto Dupliciation = m_DamagedObjectList.begin(); Dupliciation != m_DamagedObjectList.end(); )
	{
		if (false == (*Dupliciation)->Get_CollisionDuplicate())
		{
			Dupliciation = m_DamagedObjectList.erase(Dupliciation);
		}
		else
		{
			Dupliciation++;
		}
	}*/

	_bool	bCollisionResult = false;	// �浹�� �Ͼ�°�?

	if (CGameInstance::GetInstance()->PxSweepCapsule(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				_bool bDamagedTarget = true; // �浹�� ������ Ÿ���ΰ�?
				for (auto Dupliciation = m_DamagedObjectList.begin(); Dupliciation != m_DamagedObjectList.end();)
				{
					if ((*Dupliciation) == pTarget)
					{
						if (false == (*Dupliciation)->Get_CollisionDuplicate())
						{
							Dupliciation = m_DamagedObjectList.erase(Dupliciation);
						}
						else
						{
							Dupliciation++;
							bDamagedTarget = false;
						}
					}
					else
					{
						Dupliciation++;
					}
				}
				
				if (bDamagedTarget)
				{
					bCollisionResult = true;

					DAMAGE_PARAM tParam;
					memcpy(&tParam, &DamageParam, sizeof(DAMAGE_PARAM));

					// ���ο��� ��ü������ ����� ��
					tParam.pCauser = this;
					tParam.pContactComponent = CPhysXUtils::GetComponent(pHit.actor);
					tParam.vHitNormal = _float3(pHit.normal.x, pHit.normal.y, pHit.normal.z);
					tParam.vHitPosition = _float3(pHit.position.x, pHit.position.y, pHit.position.z);
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					tParam.vSlashVector = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);

					pTarget->TakeDamage(tParam);
					pTarget->Set_CollisionDuplicate(true);

					// �̹� �浹�ߴ� ����� ����Ʈ�� �߰�
					m_DamagedObjectList.push_back(pTarget);
				}
			}
		}
	}

	m_BeforePos = vPos;

	return bCollisionResult;
}

void CScarletCharacter::Free()
{
	CGameObject::Free();
	Safe_Release(m_pCollider);
	Safe_Release(m_pGameInstance);
}
