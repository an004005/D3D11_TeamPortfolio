#include "stdafx.h"
#include "..\public\SpecialObject.h"
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

CSpecialObject::CSpecialObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CSpecialObject::CSpecialObject(const CSpecialObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpecialObject::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecialObject::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("InitPos"))
		{
			_float4 InitPos = json["InitPos"];
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&InitPos));
		}
	}

	FAILED_CHECK(SetUp_Components(pArg));

	return S_OK;
}

void CSpecialObject::BeginTick()
{
	__super::BeginTick();

//	m_pCollider->UpdateChange();
}

void CSpecialObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	OutlineMaker();

//	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecialObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

void CSpecialObject::AfterPhysX()
{
	__super::AfterPhysX();

//	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecialObject::Render()
{
	FAILED_CHECK(__super::Render());

	//if(m_eCurModelTag != Tag_End)
	//{
		const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
		//FAILED_CHECK(m_pModelComs[m_eCurModelTag]->Render(WorldMatrix));
		FAILED_CHECK(m_pModelComs[0]->Render(WorldMatrix));
	//}

	return S_OK;
}

HRESULT CSpecialObject::Render_ShadowDepth()
{
	//if (m_eCurModelTag != Tag_End)
	//{
		const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
		//m_pModelComs[m_eCurModelTag]->Render_ShadowDepth(WorldMatrix);
		m_pModelComs[0]->Render_ShadowDepth(WorldMatrix);
	//}

	return S_OK;
}

void CSpecialObject::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
}

void CSpecialObject::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
}

void CSpecialObject::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	//if (ImGui::CollapsingHeader("Local Matrix Edit"))
	//{
	//	static GUIZMO_INFO tInfo;
	//	CImguiUtils::Render_Guizmo(&m_LocalMatrix, tInfo, true, true);
	//}


	//if (ImGui::Button("Kinetic Object Reset"))
	//{
	//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -30.f, 0.f, 1.f));
	//	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(180.f) });

	//	m_pCollider->Activate(true);
	//	m_pCollider->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
	//	m_pCollider->Set_Kinetic(true);
	//	m_pCollider->UpdateChange();
	//}

	//if (ImGui::BeginListBox("ModelTagList"))
	//{
	//	_uint i = 0;
	//	for (auto tag : m_pModelTags)
	//	{
	//		const bool bSelected = (tag == m_strModelTag);

	//		if (bSelected)
	//			ImGui::SetItemDefaultFocus();

	//		char pStr[MAX_PATH];
	//		strcpy(pStr, CGameUtils::GetFileName(ws2s(tag)).c_str());

	//		if (ImGui::Selectable(pStr, bSelected))
	//		{
	//			m_strModelTag = tag;
	//			//m_eCurModelTag = (KineticModeltag)i;
	//		}

	//		++i;
	//	}
	//	ImGui::EndListBox();
	//}
}

void CSpecialObject::OutlineMaker()
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

HRESULT CSpecialObject::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	//m_eCurModelTag = Tag_default;

	return S_OK;
}

void CSpecialObject::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	
	for (auto& iter : m_pModelComs)
		Safe_Release(iter);
	m_pModelComs.clear();

	m_pModelTags.clear();

	Safe_Release(m_pCollider);
}
