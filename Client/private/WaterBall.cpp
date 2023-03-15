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

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"BulletSkummyPool", L"Model", (CComponent**)&m_pModelCom));
	
	m_fShootSpeed = 30.f;

	m_pTransformCom->SetSpeed(m_fShootSpeed);
	
	// FSM 세팅
	{
		m_pFSMCom = CFSMComponentBuilder()
			.InitState("Shoot")
			.AddState("Shoot")
				.OnStart([this] 
				{			
					m_BeforePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					// Effect 생성
				})
				.Tick([this](_double TimeDelta) 
				{
					/*if (nullptr == m_pCastOwner)
						return;

					if (m_pCastOwner->IsDeleted())
						SetDelete();*/

					// 날아가면서 붙어있을 Effect 관리
					m_pTransformCom->Move(TimeDelta, m_vDir);

					DAMAGE_PARAM	dParams;
					dParams.eAttackType = EAttackType::ATK_HEAVY;
					dParams.eDeBuff = EDeBuffType::DEBUFF_OIL;
					dParams.iDamage = 1;
					dParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//					dParams.pCauser = m_pCastOwner;

					CollisionCheck_Bullet(m_pTransformCom, dParams, 0.5f);//, ECOLLIDER_TYPE_BIT::CTB_PLAYER);
					if (m_bHitCheck == true)
					{
						m_bDelete = true;
					}
					else
					{
						_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
						_float4 fOrigin;
						XMStoreFloat4(&fOrigin, vOrigin);
						_vector vDest = m_BeforePos;//m_pCastOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

						const _vector vDiff = vDest - vOrigin;
						const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));

						if (abs(fDistance > 70.f))// || fOrigin.y <= 0.f)
						{					
							m_bDelete = true;
						}
					}				
				})
			
			.Build();
	}

	return S_OK;
}

void CWaterBall::BeginTick()
{
	CBullet::BeginTick();

	/*for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == TEXT("Prototype_MonsterBoss1"))
		{
			m_pCastOwner = dynamic_cast<CScarletCharacter*>(iter);
		}
	}*/
}

void CWaterBall::Tick(_double TimeDelta)
{
	CBullet::Tick(TimeDelta);
	m_pFSMCom->Tick(TimeDelta);
	/*if (m_pCastOwner->IsDeleted())
		SetDelete();*/
}

void CWaterBall::Late_Tick(_double TimeDelta)
{
	CBullet::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CWaterBall::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CWaterBall::Imgui_RenderProperty()
{
	CBullet::Imgui_RenderProperty();
}

void CWaterBall::AfterPhysX()
{
	__super::AfterPhysX();
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
	Safe_Release(m_pFSMCom);
}
