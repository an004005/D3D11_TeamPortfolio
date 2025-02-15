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

void CScarletCharacter::BeginTick()
{
	CGameObject::BeginTick();
	m_vPrePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
}

void CScarletCharacter::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	Update_DeBuff(TimeDelta);

	for (auto& iter : m_vecDuplicationCool)
	{
		iter -= (_float)TimeDelta;
	}
}

void CScarletCharacter::Late_Tick(_double TimeDelta)
{
	// 점프하고 싶을 때는 m_fYSpeed에 + 값을 넣어준다.
	// if (m_pGameInstance->KeyDown(DIK_SPACE))
	// {
	// 	m_fYSpeed = 10.f;
	// }

	CGameObject::Late_Tick(TimeDelta);
	m_bPreOnFloor = m_bOnFloor;

	if (m_pCollider->IsOnPhysX())
	{
		const _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector vPushBackOffset = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		if (m_bAwayFromBelowObject)
		{
			physx::PxCapsuleGeometry CapsuleGeo;
			physx::PxTransform pxTransform;
			m_pCollider->GetShapeInfo(CapsuleGeo, pxTransform);

			physx::PxOverlapHit hitBuffer[3];
			physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

			SphereOverlapParams tParams;
			tParams.overlapOut = &overlapOut;
			tParams.fRadius = CapsuleGeo.radius * 1.5f;
			tParams.vPos = vPos;
			tParams.vPos.y -= CapsuleGeo.radius;
			tParams.fVisibleTime = 0.01f;
			tParams.iTargetType = CTB_PLAYER | CTB_MONSTER | CTB_MONSTER_PART;
			tParams.queryFlags = physx::PxQueryFlag::eDYNAMIC;

			if (CGameInstance::GetInstance()->OverlapSphere(tParams))
			{
				for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
				{
					if (auto pTarget = dynamic_cast<CScarletCharacter*>(CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor)))
					{
						if (pTarget == this)
							continue;
						const _vector vBelowObjectPos = pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
						const _vector vPushBackVector = XMVector3NormalizeEst(XMVectorSetY(vPos - vBelowObjectPos, 0.f));
						vPushBackOffset += vPushBackVector * TimeDelta * 15.f;
					}
				}
			}
		}


		const _vector vPrePos = m_vPrePos;
		const _vector vMoveDelta = vPos - vPrePos + vPushBackOffset;

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
}

void CScarletCharacter::AfterPhysX()
{
	CGameObject::AfterPhysX();
	if (m_pCollider->IsOnPhysX())
	{
		const _float4 vColliderFootPos = m_pCollider->GetFootPosition();
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vColliderFootPos);
		m_vPrePos = vColliderFootPos;
	}
	else
	{
		m_vPrePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	}
}

_float4 CScarletCharacter::GetColliderPosition()
{
	if (nullptr == m_pCollider)
		return _float4(0.f, 0.f, 0.f, 1.f);

	return m_pCollider->GetPosition();
}

void CScarletCharacter::SetWorldMatrix(_float4x4 WorldMatrix)
{
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	if (m_pCollider && m_pCollider->IsOnPhysX())
		m_pCollider->SetFootPosition(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

void CScarletCharacter::SetForcePos(_float4 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	if (m_pCollider && m_pCollider->IsOnPhysX())
		m_pCollider->SetFootPosition(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
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
			DeBuff_Thunder();
			break;
		case EDeBuffType::DEBUFF_WATER:
			DeBuff_Water();
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
	// 1인자 : 리지드바디
	// 2인자 : 데미지 정보
	// 3인자 : 충돌 여부, 함수는 계속 돌리되 충돌판정하지 않을거면 false대입
	// 4인자 : 타겟 타입

	if (!bCollision && !bContinuity)
	{
		m_DamagedObjectList.clear();
		return;
	}

	// 공격자의 트리거와 공격 파라미터를 전달받아서 진행

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

					// 내부에서 자체적으로 계산할 값
					tParam.pCauser = this;
					tParam.pContactComponent = CPhysXUtils::GetComponent(pHit.actor);
					tParam.vHitNormal = _float4(pHit.normal.x, pHit.normal.y, pHit.normal.z, 0.f);
					tParam.vHitPosition = _float4(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				
					pTarget->TakeDamage(tParam);
					++m_iHitTargetCount;
					
					// 이미 충돌했던 대상을 리스트에 추가
					m_DamagedObjectList.push_back(pTarget);
				}
			}
		}
	}

	m_BeforePos = vPos;
}

_bool CScarletCharacter::Collision_Check_Capsule_Improved(CRigidBody * AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollisionCheck, ECOLLIDER_TYPE_BIT ColType, ECOPYCOLTYPE eCopyType)
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
	param.fVisibleTime = 0.1f;

	// 중복 충돌 판단
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

	_bool	bCollisionResult = false;	// 충돌이 일어났는가?

	if (CGameInstance::GetInstance()->PxSweepCapsule(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				_bool bDamagedTarget = true; // 충돌이 가능한 타겟인가?
				for (auto Dupliciation = m_DamagedObjectList.begin(); Dupliciation != m_DamagedObjectList.end();)
				{
					if (m_DamagedObjectList.empty())
						break;

					if (Dupliciation == m_DamagedObjectList.end())
						return false;

					if (false == CGameInstance::GetInstance()->Check_ObjectAlive(*Dupliciation))
					{
						++Dupliciation;
						continue;
					}

					if ((*Dupliciation) == pTarget)
					{
						// 원본
						if (false == (*Dupliciation)->Get_CollisionDuplicate(eCopyType))
						{
							Dupliciation = m_DamagedObjectList.erase(Dupliciation);
						}
						// 내부쿨
						//if (/*false == (*Dupliciation)->Get_CollisionDuplicate(eCopyType) || */0.f >= (*Dupliciation)->Get_CollisionDuplicate(eCopyType))
						//{
						//	(*Dupliciation)->Set_CollisionDuplicate(false, eCopyType);
						//	(*Dupliciation)->Set_CollisionDuplicateCool(0.f, eCopyType);

						//	Dupliciation = m_DamagedObjectList.erase(Dupliciation);
						//}
						else
						{
							++Dupliciation;
							bDamagedTarget = false;
						}
					}
					else
					{
						++Dupliciation;
					}
				}
				
				if (bDamagedTarget)
				{
					bCollisionResult = true;

					DAMAGE_PARAM tParam;
					ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
					memcpy(&tParam, &DamageParam, sizeof(DAMAGE_PARAM));

					// 내부에서 자체적으로 계산할 값
					tParam.pCauser = this;
					tParam.pContactComponent = CPhysXUtils::GetComponent(pHit.actor);
					tParam.vHitNormal = _float4(pHit.normal.x, pHit.normal.y, pHit.normal.z, 0.f);
					tParam.vHitPosition = _float4(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					tParam.vSlashVector = _float4(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z, 0.f);


					// 플레이어일 경우 타격 이펙트 생성하도록
					pTarget->TakeDamage(tParam);
					pTarget->Set_CollisionDuplicate(true, eCopyType);
					pTarget->Set_CollisionDuplicateCool(0.05f, eCopyType);	// 내부쿨

					IM_LOG(ws2s(pTarget->GetPrototypeTag()).c_str());
					
					// 이미 충돌했던 대상을 리스트에 추가
					m_DamagedObjectList.push_back(pTarget);

					//플레이어가 공격했을때 몬스터에 target ui 생성
					/*if (dynamic_cast<CPlayer*>(tParam.pCauser) != nullptr)
						dynamic_cast<CPlayer*>(tParam.pCauser)->Create_TargetInfoBar(pTarget);*/
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
