#include "stdafx.h"
#include "SkMpBullet.h"
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

CSkMpBullet::CSkMpBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBullet(pDevice, pContext)
{
}

CSkMpBullet::CSkMpBullet(const CSkMpBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CSkMpBullet::Initialize_Prototype()
{
	return CBullet::Initialize_Prototype();
}

HRESULT CSkMpBullet::Initialize(void * pArg)
{
	FAILED_CHECK(CBullet::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"BulletSkummyPool", L"Model", (CComponent**)&m_pModelCom));
	
	m_fShootSpeed = 18.f;

	m_pTransformCom->SetSpeed(m_fShootSpeed);

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Shoot")			
			.AddState("Shoot")	
				.OnStart([this]
				{					
					m_BeforePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				})
				.Tick([this](_double TimeDelta) 
				{
					if (nullptr == m_pCastOwner)
						return;

					if (m_pCastOwner->IsDeleted())
						SetDelete();

					m_pTransformCom->Move(TimeDelta, m_vDir);

					DAMAGE_PARAM	dParams;
					dParams.eAttackType = EAttackType::ATK_LIGHT;
					dParams.eDeBuff = EDeBuffType::DEBUFF_END;
					dParams.iDamage = (rand() % 50) + 25;
					dParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					dParams.pCauser = m_pCastOwner;

					_uint iColType = ECOLLIDER_TYPE_BIT::CTB_PLAYER | ECOLLIDER_TYPE_BIT::CTB_STATIC | ECOLLIDER_TYPE_BIT::CTB_PSYCHICK_OBJ;

					CollisionCheck_Bullet(m_pTransformCom, dParams, 0.3f, iColType);

					if (m_bHitCheck == true)
					{
						CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0650_Bullet_Dead")->Start_NoAttach(this, false);
	//					CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0650_Bullet_Dead_Particle")->Start_NoAttach(this, false);
									
						m_bDelete = true;
					}
					else
					{																						
						_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
						_vector vDest = m_pCastOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

						const _vector vDiff = vDest - vOrigin;
						const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));

						if (fDistance > 30.f)
						{
							CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0650_Bullet_Dead")->Start_NoAttach(this, false);
	//						CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0650_Bullet_Dead_Particle")->Start_NoAttach(this, false);
						   
							m_bDelete = true;
						}
					}
				})

			.Build();
	}

	return S_OK;
}

void CSkMpBullet::BeginTick()
{	
	CBullet::BeginTick();

	//for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
	//{
	//	if (iter->GetPrototypeTag() == TEXT("SkummyPool"))
	//	{
	//		m_pCastOwner = dynamic_cast<CScarletCharacter*>(iter);
	//	}
	//}
	m_pCastOwner = dynamic_cast<CScarletCharacter*>(m_pOwner);
	Safe_AddRef(m_pCastOwner);
}

void CSkMpBullet::Tick(_double TimeDelta)
{
	CBullet::Tick(TimeDelta);
	m_pFSM->Tick(TimeDelta);
	if (m_pCastOwner->IsDeleted())
		SetDelete();
}

void CSkMpBullet::Late_Tick(_double TimeDelta)
{
	CBullet::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkMpBullet::Render()
{
	//m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CSkMpBullet::Imgui_RenderProperty()
{
	CBullet::Imgui_RenderProperty();
}

void CSkMpBullet::AfterPhysX()
{
	__super::AfterPhysX();	
}

void CSkMpBullet::Set_TrailParticle(CParticleGroup * pTrail)
{
	 m_pTrailParticle = pTrail; 
}

CParticleGroup * CSkMpBullet::Get_TrailParticle()
{
	if (m_pTrailParticle == nullptr)
		return nullptr;

	 return m_pTrailParticle; 
}

void CSkMpBullet::Set_BulletEffect(CEffectGroup * pBulletEffect)
{
	m_pBulletEffect = pBulletEffect;
}

CEffectGroup * CSkMpBullet::Get_BulletEffect()
{
	if (m_pBulletEffect == nullptr)
		return nullptr;

	return m_pBulletEffect;
}

CSkMpBullet * CSkMpBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkMpBullet* pInstance = new CSkMpBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkMpBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkMpBullet::Clone(void * pArg)
{
	CSkMpBullet*		pInstance = new CSkMpBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkMpBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkMpBullet::Free()
{
	if (m_bCloned == true)
	{
		if (m_pTrailParticle->IsDeleted() == false)
			m_pTrailParticle->SetDelete();

		if (m_pBulletEffect->IsDeleted() == false)
			m_pBulletEffect->SetDelete();
			
		Safe_Release(m_pTrailParticle);
		Safe_Release(m_pBulletEffect);
	}
	__super::Free();

	Safe_Release(m_pFSM);
	Safe_Release(m_pCastOwner);
}
