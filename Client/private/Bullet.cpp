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
	
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	return S_OK;
}

void CBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_fLife > 0.f)
		m_fLife -= (_float)TimeDelta;
	else
		m_bDelete = true;

	m_BeforePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (m_pTarget != nullptr) //추적
	{
		//_matrix TargetWorldMatrix = m_pTarget->GetBoneMatrix("Mask") * m_pTarget->GetTransform()->Get_WorldMatrix();
		//TargetWorldMatrix.r[3] += XMVectorSet(0.f, 1.f, 0.f, 0.f) * 1.f;
		_matrix TargetWorldMatrix = m_pTarget->GetTransform()->Get_WorldMatrix();
		TargetWorldMatrix.r[3] += XMVectorSet(0.f, 1.f, 0.f, 0.f) * 2.5f;
		m_pTransformCom->Chase(TargetWorldMatrix.r[3], TimeDelta, 0.01f);
	}
	else
		m_pTransformCom->Go_Straight(TimeDelta);

	DAMAGE_PARAM	dParams;
	dParams.eAttackType = m_eDamageParam.eAttackType;
	dParams.eDeBuff = m_eDamageParam.eDeBuff;
	dParams.iDamage = m_eDamageParam.iDamage;
	dParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	dParams.pCauser = nullptr;

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pOwner))
	{
		if (auto pScarletCharacter = dynamic_cast<CScarletCharacter*>(m_pOwner))
			dParams.pCauser = pScarletCharacter;
	}

	_uint iColType = ECOLLIDER_TYPE_BIT::CTB_PLAYER | ECOLLIDER_TYPE_BIT::CTB_STATIC | ECOLLIDER_TYPE_BIT::CTB_PSYCHICK_OBJ;

	CollisionCheck_Bullet(m_pTransformCom, dParams, 1.f, iColType);


	if (m_bHitCheck == true || m_bDelete == true) // hit 체크 및 life 다 떨어진거 체크
	{
		Create_DeadEffects();
		Create_DeadParticle();

		//CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Bullet_Dead_1")->Start_NoAttach(this, false);
		//CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Bullet_Dead_2")->Start_NoAttach(this, false);
		//CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Bullet_Dead_3")->Start_NoAttach(this, false);

		m_bDelete = true;
	}
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

void CBullet::Create_InitEffects(vector<wstring>& InitEffects, _float4x4 pivot)
{
	if (InitEffects.empty()) return;

	for (auto it : InitEffects)
	{
		CEffectGroup* pEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, it);
		assert(pEffect != nullptr);
		pEffect->Start_NoAttachPivot(this, pivot, true);
		Safe_AddRef(pEffect);

		m_pInitEffects.emplace_back(pEffect);
	}
}

void CBullet::Create_InitParticle(wstring& InitParticle)
{
	if (InitParticle == L"") return;

	CParticleGroup* pParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, InitParticle);
	assert(pParticle != nullptr);
	pParticle->Start_NoAttach(this, true);
	Safe_AddRef(pParticle);

	m_pInitParticle = pParticle;
}

void CBullet::Create_DeadEffects()
{
	if (m_pDeadEffects.empty()) return;

	for (auto it : m_pDeadEffects)
	{
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, it)->Start_NoAttach(this, false);
	}
}

void CBullet::Create_DeadParticle()
{
	if (m_pDeadParticle == L"") return;

	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, m_pDeadParticle)->Start_NoAttach(this, false);
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

			if (pTarget == nullptr) // 건물, 염력이 적용되는 물체 등에 대한 예외 처리
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

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet* pInstance = new CBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
	CBullet* pInstance = new CBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBullet::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);

	if (m_bCloned == true)
	{
		for (auto iter : m_pInitEffects)
		{
			if (iter != nullptr)
			{
				iter->SetDelete();
				Safe_Release(iter);
				iter = nullptr;
			}
		}

		m_pInitEffects.clear();
		

		if (m_pInitParticle != nullptr)
		{
			m_pInitParticle->SetDelete();
			Safe_Release(m_pInitParticle);
			m_pInitParticle = nullptr;
		}
	}
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