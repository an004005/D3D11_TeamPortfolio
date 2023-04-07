#include "stdafx.h"
#include "HS_TeleportableWall.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "RigidBody.h"
#include "GameInstance.h"
#include "ControlledRigidBody.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "PhysXStaticModel.h"
#include "PlayerInfoManager.h"
#include "ImguiUtils.h"

CHS_TeleportableWall::CHS_TeleportableWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CHS_TeleportableWall::CHS_TeleportableWall(const CHS_TeleportableWall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CHS_TeleportableWall::Initialize_Prototype()
{
	return CGameObject::Initialize_Prototype();
}

HRESULT CHS_TeleportableWall::Initialize(void* pArg)
{
	Json TWallRigidBody = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/TeleportWallRigid.json");	
	pArg = &TWallRigidBody;

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("WallRigidBody"),
		(CComponent**)&m_pRigidBody, &TWallRigidBody)))
		return E_FAIL;

	FAILED_CHECK(__super::Initialize(pArg));
	
	FAILED_CHECK(SetUp_Components());
	
	return S_OK;
}

void CHS_TeleportableWall::BeginTick()
{
	__super::BeginTick();
}

void CHS_TeleportableWall::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pModelCom->Tick(TimeDelta);
}

void CHS_TeleportableWall::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible && (nullptr != m_pRendererCom))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
	}
}

HRESULT CHS_TeleportableWall::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
			
	const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
	FAILED_CHECK(m_pModelCom->Render(m_pTransformCom));

 	return S_OK;
}

HRESULT CHS_TeleportableWall::Render_ShadowDepth()
{
	const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
	m_pModelCom->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CHS_TeleportableWall::LoadFromJson(const Json& json)
{
	__super::LoadFromJson(json);	

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CHS_TeleportableWall::SaveToJson(Json& json)
{
	__super::SaveToJson(json);

	json["LocalMatrix"] = m_LocalMatrix;
}

void CHS_TeleportableWall::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Local Matrix Edit"))
	{
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, tInfo, true, true);
	}
}

HRESULT CHS_TeleportableWall::SetUp_Components()
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TWall", L"Model",
		(CComponent**)&m_pModelCom));

	return S_OK;
}

CHS_TeleportableWall* CHS_TeleportableWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHS_TeleportableWall* pInstance = new CHS_TeleportableWall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHS_TeleportableWall");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CHS_TeleportableWall::Clone(void* pArg)
{
	CHS_TeleportableWall* pInstance = new CHS_TeleportableWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHS_TeleportableWall");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CHS_TeleportableWall::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigidBody);
}
