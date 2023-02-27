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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->SetSpeed(1.f);

	m_strObjectTag = "SkMp_Bullet";

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")
			.AddState("Idle")
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
			
				})
				.Tick([this](_double TimeDelta) 
				{
					m_pTransformCom->Move(0.3f, m_vDir);

					m_fTimeAcc += _float(TimeDelta * 1);

					if (m_fTimeAcc >= 5.f)
					{
						m_bShoot = false;
						m_bDead = true;
					}
				})

				.AddTransition("Shoot to Dead", "Dead")
					.Predicator([this]()->_bool { return !m_bShoot && m_bDead; })

			.AddState("Dead")
				.OnStart([this]
				{
					// °´Ã¼ »èÁ¦	
					m_bDelete = true;
				})

			.Build();
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
}
