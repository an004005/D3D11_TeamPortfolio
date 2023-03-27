#include "stdafx.h"
#include "RedBullet.h"
#include <random>

#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "JsonStorage.h"
#include "PhysX_Manager.h"

#include "FSMComponent.h"

#include "SkummyPool.h"
#include "Player.h"
#include "VFX_Manager.h"

CRedBullet::CRedBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBullet(pDevice, pContext)
{
}

CRedBullet::CRedBullet(const CRedBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CRedBullet::Initialize_Prototype()
{
	return CBullet::Initialize_Prototype();
}

HRESULT CRedBullet::Initialize(void * pArg)
{
	FAILED_CHECK(CBullet::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"BulletSkummyPool", L"Model", (CComponent**)&m_pModelCom));

	m_fShootSpeed = 18.f;

	m_pTransformCom->SetSpeed(m_fShootSpeed);

	m_pTrailParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0650_Bullet_Loop");
	m_pTrailParticle->Start_NoAttach(this, true);
	Safe_AddRef(m_pTrailParticle);

	m_pBulletEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"Em0650_Bullet_Loop");
	m_pBulletEffect->Start_NoAttach(this, true);
	Safe_AddRef(m_pBulletEffect);

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Shoot")
			.AddState("Shoot")
			.Tick([this](_double TimeDelta)
		{
			if (!m_bPrePos)
			{
				m_vPrePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				m_bPrePos = true;
			}

			_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_pTransformCom->Move(TimeDelta, vDir);

			DAMAGE_PARAM	dParams;
			dParams.eAttackType = EAttackType::ATK_LIGHT;
			dParams.eDeBuff = EDeBuffType::DEBUFF_END;
			dParams.iDamage = (rand() % 50) + 25;
			dParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			_uint iColType = ECOLLIDER_TYPE_BIT::CTB_PLAYER | ECOLLIDER_TYPE_BIT::CTB_STATIC | ECOLLIDER_TYPE_BIT::CTB_PSYCHICK_OBJ;

			CollisionCheck_Bullet(m_pTransformCom, dParams, 0.3f, iColType);

			if (m_bHitCheck == true)
			{
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0650_Bullet_Dead")->Start_NoAttach(this, false);
				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0650_Bullet_Dead_Particle")->Start_NoAttach(this, false);

				m_bDelete = true;
			}
			else
			{
				_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				const _vector vDiff = m_vPrePos - vOrigin;
				const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));

				if (fDistance > 30.f)
				{
					CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0650_Bullet_Dead")->Start_NoAttach(this, false);
					CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0650_Bullet_Dead_Particle")->Start_NoAttach(this, false);

					m_bDelete = true;
				}
			}
		})

			.Build();
	}

	return S_OK;
}

void CRedBullet::BeginTick()
{
	CBullet::BeginTick();
}

void CRedBullet::Tick(_double TimeDelta)
{
	CBullet::Tick(TimeDelta);
	m_pFSM->Tick(TimeDelta);
}

void CRedBullet::Late_Tick(_double TimeDelta)
{
	CBullet::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRedBullet::Render()
{
	//m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CRedBullet::Imgui_RenderProperty()
{
	CBullet::Imgui_RenderProperty();
}

void CRedBullet::AfterPhysX()
{
	__super::AfterPhysX();
}

CRedBullet * CRedBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRedBullet* pInstance = new CRedBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRedBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRedBullet::Clone(void * pArg)
{
	CRedBullet*		pInstance = new CRedBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRedBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRedBullet::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);

	if (m_bCloned == true)
	{
		if (m_pBulletEffect != nullptr)
		{
			m_pBulletEffect->SetDelete();
			Safe_Release(m_pBulletEffect);
		}

		if (m_pTrailParticle != nullptr)
		{
			m_pTrailParticle->SetDelete();
			Safe_Release(m_pTrailParticle);
		}
	}

	//	Safe_Release(m_pCastOwner);
}
