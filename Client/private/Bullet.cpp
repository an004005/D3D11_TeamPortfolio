#include "stdafx.h"
#include "..\public\Bullet.h"
#include "RigidBody.h"
#include "PhysX_Manager.h"
#include "Material.h"

#include "ScarletCharacter.h"
#include "Player.h"
#include "Monster.h"

CBullet::CBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBullet::CBullet(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBullet::Initialize(void * pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));
	
	return S_OK;
}

void CBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CBullet::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
}

void CBullet::AfterPhysX()
{
	CGameObject::AfterPhysX();
}

void CBullet::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

_bool CBullet::CheckDamagedTarget(CGameObject * pTarget)
{
	const auto itr = m_DamagedTargetList.find(pTarget);
	if (itr == m_DamagedTargetList.end())
	{
		m_DamagedTargetList.insert(pTarget);
		return true;
	}

	return false;
}

void CBullet::ClearDamagedTarget()
{
	m_DamagedTargetList.clear();
}

void CBullet::CollisionCheck_Bullet(CTransform* pTransform, DAMAGE_PARAM mParam, _float fRad, _uint eColTypes)
{
	_float4 vPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	
	if (m_BeforePos.w == -1.f)
	{
		m_BeforePos = vPos;
	}

	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer sweepOut(hitBuffer, 5);

	SphereSweepParams sParam;
	sParam.sweepOut = &sweepOut;

	_float4	vBulletDir = vPos - m_BeforePos;

	sParam.vUnitDir = _float3(vBulletDir.x, vBulletDir.y, vBulletDir.z);
	sParam.fDistance = sParam.vUnitDir.Length();
	sParam.vUnitDir.Normalize();

	sParam.fRadius = fRad;
	sParam.fVisibleTime = 0.1f;
	sParam.iTargetType = eColTypes;
	sParam.vPos = vPos;

	if (CGameInstance::GetInstance()->SweepSphere(sParam))
	{
		for (int i = 0; i < sweepOut.getNbAnyHits(); ++i)
		{
			auto pTarget = dynamic_cast<CScarletCharacter*>(CPhysXUtils::GetOnwer(sweepOut.getAnyHit(i).actor));
			
			if (pTarget == nullptr)
			{
				m_bHitCheck = true;
				return;
			}
			
			memcpy(&mParam.vHitPosition, &sweepOut.getAnyHit(i).position, sizeof(_float3));
			memcpy(&mParam.vHitNormal, &sweepOut.getAnyHit(i).normal, sizeof(_float3));
			CheckHitTarget(pTarget, CPhysXUtils::GetComponent(sweepOut.getAnyHit(i).actor), mParam);
		}
	}

	m_BeforePos = vPos;
}

void CBullet::CheckHitTarget(CGameObject * pTarget, CComponent * pHitBox, DAMAGE_PARAM mParam)
{	
	 if (auto pPlayer = dynamic_cast<CPlayer*>(pTarget))
	{
		if (CheckDamagedTarget(pPlayer))
		{
			mParam.pContactComponent = pHitBox;

			auto pHitTarget = dynamic_cast<CScarletCharacter*>(pPlayer);
			pHitTarget->TakeDamage(mParam);

			m_bHitCheck = true;

			ClearDamagedTarget();
		}
		else
			m_bHitCheck = false;
	}	
}

void CBullet::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

/*if (CheckDamagedTarget(pTarget))
{
_float3 vHitPos = pHitBox->TryGetOwner()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

DAMAGE_PARAM tParam;
tParam.iDamage = mParam.iDamage;
tParam.eAttackSAS = mParam.eAttackSAS;
tParam.eDeBuff = mParam.eDeBuff;
tParam.pContactComponent = pHitBox;
tParam.vHitPosition = vHitPos;
tParam.vHitFrom = mParam.vHitFrom;

auto pHitTarget = dynamic_cast<CScarletCharacter*>(pTarget);
pHitTarget->TakeDamage(tParam);

m_bHitCheck = true;
}
else
m_bHitCheck = false;*/