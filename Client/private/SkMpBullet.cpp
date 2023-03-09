#include "stdafx.h"
#include "SkMpBullet.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "JsonStorage.h"

#include "SkummyPool.h"
#include "Model.h"
#include "Bone.h"
#include "RigidBody.h"

#include "Player.h"
#include "PhysX_Manager.h"

CSkMpBullet::CSkMpBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkMpBullet::CSkMpBullet(const CSkMpBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkMpBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkMpBullet::Initialize(void * pArg)
{
	Json SkMpBullet = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/SkummyPoolBullet.json");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Bullet"),
		(CComponent**)&m_pCollider, &SkMpBullet)))
		return E_FAIL;

	m_pTransformCom->SetSpeed(22.f);

	m_strObjectTag = "SkMp_Bullet";

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.OnStart([this]
				{						
					m_bCollision = false;
				})
				.Tick([this](_double TimeDelta)
				{
					m_bIdle = false;
					m_bShoot = true;
				})
				.AddTransition("Idle to Shoot", "Shoot")
					.Predicator([this]()->_bool { return !m_bIdle && m_bShoot; })

			.AddState("Shoot")	
					.OnStart([this]
				{
					m_fTimeAcc = 0.f;				
				})
				.Tick([this](_double TimeDelta) 
				{
					m_pTransformCom->Move(TimeDelta, m_vDir);

					m_fTimeAcc += _float(TimeDelta);

					if (m_bCollision)
					{
						m_bShoot = false;
						m_bDead = true;
					}

					else
					{
						if (m_fTimeAcc >= 3.5f)
						{
							m_bShoot = false;
							m_bDead = true;
							m_bCollision = true;
						}
					}
				})

				.AddTransition("Shoot to Dead", "Dead")
					.Predicator([this]()->_bool { return !m_bShoot && m_bDead && m_bCollision; })

			.AddState("Dead")			
				.Tick([this](_double TimeDelta)
				{			
					m_bDelete = true;
				})

			.Build();
	}

	{
		m_pCollider->SetOnTriggerIn([this](CGameObject* pObj)
		{			
			if (auto pPlayer = dynamic_cast<CPlayer*>(pObj))
			{
				if (!m_bCollision)
				{
					physx::PxRaycastHit hitBuffer[1];
					physx::PxRaycastBuffer rayOut(hitBuffer, 1);

					RayCastParams param;
					param.rayOut = &rayOut;
					param.vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					param.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					param.fDistance = 5.f;
					param.iTargetType = CTB_PLAYER;
					param.bSingle = true;
					param.fVisibleTime = 0.3f;
					if (CGameInstance::GetInstance()->RayCast(param))
					{
					   for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
					   {
					      auto pHit = rayOut.getAnyHit(i);
					      CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
					      if (auto pPlayerCol = dynamic_cast<CPlayer*>(pCollidedObject))
					      {
					         DAMAGE_PARAM tParam;
					         tParam.iDamage = 1;
					         tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
							 pPlayerCol->TakeDamage(tParam);
						   }
					   }
					}
					/*	상관없다.
					DAMAGE_PARAM damageParam;

					_float3 fWeaponHitPosition = { m_pCollider->GetPxWorldMatrix().m[3][0], m_pCollider->GetPxWorldMatrix().m[3][1], m_pCollider->GetPxWorldMatrix().m[3][2] };

					damageParam.eAttackSAS = ESASType::SAS_END;
					damageParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					damageParam.vHitPosition = fWeaponHitPosition;
					damageParam.pCauser = dynamic_cast<CScarletCharacter*>(this);
					damageParam.iDamage = 1;

					pPlayer->TakeDamage(damageParam);
					*/

					m_bCollision = true;
				}
				
			}			
		});

		m_pCollider->SetOnTriggerOut([this](CGameObject* pObj)
		{
			
		});
	}

	return S_OK;
}

void CSkMpBullet::BeginTick()
{	
	__super::BeginTick();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == TEXT("SkummyPool"))
		{			
			m_pSkummyPool = iter;
		}
	}
}

void CSkMpBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_bool bTest = m_bCollision;

	m_pCollider->Update_Tick(m_pTransformCom);

	m_pFSM->Tick(TimeDelta);
}

void CSkMpBullet::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkMpBullet::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CSkMpBullet::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
		
	m_pModelCom->Imgui_RenderProperty();
	m_pFSM->Imgui_RenderProperty();
}

void CSkMpBullet::AfterPhysX()
{
	__super::AfterPhysX();
	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSkMpBullet::SetUp_Components(void * pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_NOW, TEXT("BulletSkummyPool"), TEXT("Com_ModelCom"), 
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
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

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pCollider);
}
