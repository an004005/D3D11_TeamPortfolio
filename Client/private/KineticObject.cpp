#include "stdafx.h"
#include "..\public\KineticObject.h"
#include "Model.h"
#include "RigidBody.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "Monster.h"
#include "Player.h"
#include "ImguiUtils.h"
#include "GameUtils.h"
#include "Material.h"
#include "GravikenisisMouseUI.h"
#include "VFX_Manager.h"

CKineticObject::CKineticObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CKineticObject::CKineticObject(const CKineticObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CKineticObject::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CKineticObject::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

void CKineticObject::BeginTick()
{
	__super::BeginTick();
}

void CKineticObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CKineticObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

void CKineticObject::AfterPhysX()
{
	__super::AfterPhysX();
}

HRESULT CKineticObject::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CKineticObject::Render_ShadowDepth()
{
	if (m_eCurModelTag != Tag_End)
	{
		const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
		m_pModelComs[m_eCurModelTag]->Render_ShadowDepth(WorldMatrix);
	}

	return S_OK;
}

void CKineticObject::Imgui_RenderProperty()
{
}

void CKineticObject::OutlineMaker()
{
	if (m_bOutline != m_bBeforeOutline)
	{
		if (m_bOutline)
		{
			for (auto& Model : m_pModelComs)
			{
				for (auto& iter : Model->GetMaterials())
				{
					if (0 == iter->GetParam().Ints.size())
					{
						iter->GetParam().Ints.push_back(0);
					}
					else
					{
						iter->GetParam().Ints[0] = 1;
					}
				}
			}
		}
		else
		{
			for (auto& Model : m_pModelComs)
			{
				for (auto& iter : Model->GetMaterials())
				{
					if (0 == iter->GetParam().Ints.size())
					{
						iter->GetParam().Ints.push_back(0);
					}
					else
					{
						iter->GetParam().Ints[0] = 0;
					}
				}
			}
		}
	}

	m_bBeforeOutline = m_bOutline;
}

HRESULT CKineticObject::SetUp_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	return S_OK;
}

void CKineticObject::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	
	for (auto& iter : m_pModelComs)
		Safe_Release(iter);
	m_pModelComs.clear();

	m_pModelTags.clear();

	Safe_Release(m_pCollider);
}
