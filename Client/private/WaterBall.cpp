#include "stdafx.h"
#include "WaterBall.h"

#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "JsonStorage.h"
#include <PhysX_Manager.h>

#include "FSMComponent.h"

#include "Boss1.h"
#include "Player.h"
#include "VFX_Manager.h"

CWaterBall::CWaterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBullet(pDevice, pContext)
{
}

CWaterBall::CWaterBall(const CWaterBall & rhs)
	: CBullet(rhs)
{
}

HRESULT CWaterBall::Initialize_Prototype()
{
	return CBullet::Initialize_Prototype();
}

HRESULT CWaterBall::Initialize(void * pArg)
{
	FAILED_CHECK(CBullet::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	
	m_fShootSpeed = 29.f;

	m_pTransformCom->SetSpeed(m_fShootSpeed);
	
	m_pBulletEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Bullet");
	m_pBulletEffect->Start_NoAttach(this, true);
	Safe_AddRef(m_pBulletEffect);

	m_pTrailParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Bullet_Trail_Particle");
	m_pTrailParticle->Start_NoAttach(this, true);
	Safe_AddRef(m_pTrailParticle);


	return S_OK;
}

void CWaterBall::BeginTick()
{
	CBullet::BeginTick();
}

void CWaterBall::Tick(_double TimeDelta)
{
	CBullet::Tick(TimeDelta);
	if (m_fLife > 0.f)
		m_fLife -= (_float)TimeDelta;
	else
		m_bDelete = true;

	m_BeforePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	m_pTransformCom->Go_Straight(TimeDelta);

	DAMAGE_PARAM	dParams;
	dParams.eAttackType = EAttackType::ATK_HEAVY;
	dParams.eDeBuff = EDeBuffType::DEBUFF_OIL;
	dParams.iDamage = m_iDamage;
	dParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	dParams.pCauser = nullptr;
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pOwner))
	{
		if (auto pScarletCharacter = dynamic_cast<CScarletCharacter*>(m_pOwner))
			dParams.pCauser = pScarletCharacter;
	}

	_uint iColType = ECOLLIDER_TYPE_BIT::CTB_PLAYER | ECOLLIDER_TYPE_BIT::CTB_STATIC | ECOLLIDER_TYPE_BIT::CTB_PSYCHICK_OBJ;

	CollisionCheck_Bullet(m_pTransformCom, dParams, 0.5f, iColType);


	if (m_bHitCheck == true || m_bDelete == true) // hit 체크 및 life 다 떨어진거 체크
	{
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Bullet_Dead_1")->Start_NoAttach(this, false);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Bullet_Dead_2")->Start_NoAttach(this, false);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Bullet_Dead_3")->Start_NoAttach(this, false);

		m_bDelete = true;
	}
}

CWaterBall * CWaterBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterBall* pInstance = new CWaterBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWaterBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWaterBall::Clone(void * pArg)
{
	CWaterBall*		pInstance = new CWaterBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWaterBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWaterBall::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{
		if (m_pBulletEffect != nullptr)
		{
			m_pBulletEffect->SetDelete();
			Safe_Release(m_pBulletEffect);
			m_pBulletEffect = nullptr;
		}

		if (m_pTrailParticle != nullptr)
		{
			m_pTrailParticle->SetDelete();
			Safe_Release(m_pTrailParticle);
			m_pTrailParticle = nullptr;
		}
	}
}
