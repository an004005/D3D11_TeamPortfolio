#include "stdafx.h"
#include "SkMpBullet.h"

#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "JsonStorage.h"
#include "PhysX_Manager.h"

#include "FSMComponent.h"

#include "SkummyPool.h"
#include "Player.h"

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
	
	m_fShootSpeed = 12.f;

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
					m_pTransformCom->Move(TimeDelta, m_vDir);

					DAMAGE_PARAM	dParams;
					dParams.eAttackType = EAttackType::ATK_LIGHT;
					dParams.eDeBuff = EDeBuffType::DEBUFF_END;
					dParams.iDamage = 1;
					dParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					dParams.pCauser = m_pCastOwner;

					CollisionCheck_Bullet(m_pTransformCom, dParams, 0.3f, ECOLLIDER_TYPE_BIT::CTB_PLAYER);

					if (m_bHitCheck == true)
					{
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

	for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == TEXT("SkummyPool"))
		{
			m_pCastOwner = dynamic_cast<CScarletCharacter*>(iter);
		}
	}
}

void CSkMpBullet::Tick(_double TimeDelta)
{
	CBullet::Tick(TimeDelta);
	m_pFSM->Tick(TimeDelta);
}

void CSkMpBullet::Late_Tick(_double TimeDelta)
{
	CBullet::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkMpBullet::Render()
{
	m_pModelCom->Render(m_pTransformCom);
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
	__super::Free();
	Safe_Release(m_pFSM);
}
