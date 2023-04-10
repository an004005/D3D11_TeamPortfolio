#include "stdafx.h"
#include "..\public\Special_DropObject_Single.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Material.h"

CSpecial_DropObject_Single::CSpecial_DropObject_Single(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_DropObject_Single::CSpecial_DropObject_Single(const CSpecial_DropObject_Single & rhs)
	:CSpecialObject(rhs)
{
}

HRESULT CSpecial_DropObject_Single::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_DropObject_Single::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_DROPOBJECT_SINGLE;

	return S_OK;
}

void CSpecial_DropObject_Single::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Trigger(true);
	m_pCollider->UpdateChange();

	m_pCollider->Activate(false);
}

void CSpecial_DropObject_Single::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_DropObject_Single::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_DropObject_Single::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_DropObject_Single::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_DropObject_Single::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];

	//if (json.contains("OptionalMatrix"))
	//	m_OptionalMatrix = json["OptionalMatrix"];
}

void CSpecial_DropObject_Single::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;

	//json["OptionalMatrix"] = m_OptionalMatrix;
}

void CSpecial_DropObject_Single::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	//if (ImGui::CollapsingHeader("ChildGizmo"))
	//{
	//	static GUIZMO_INFO Drop;
	//	CImguiUtils::Render_Guizmo(&m_OptionalMatrix, Drop, true, true);
	//}
}

void CSpecial_DropObject_Single::Set_ChildMatrix(_float4x4 matrix)
{
	m_AssemblyPosition = matrix;
}

void CSpecial_DropObject_Single::Sync_position(CTransform * pTransform)
{
	m_pTransformCom->Set_WorldMatrix(m_AssemblyPosition * pTransform->Get_WorldMatrix());
}

void CSpecial_DropObject_Single::Set_Trigger(_bool bTrigger)
{
	m_pCollider->Set_Trigger(bTrigger);
}

void CSpecial_DropObject_Single::Activate(_bool bActivate)
{
	m_pCollider->Activate(bActivate);
}

void CSpecial_DropObject_Single::Set_Outline(_bool bOutline)
{
	m_bOutline = bOutline;
}

HRESULT CSpecial_DropObject_Single::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_HBeam_Single");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_HBeam_Single", L"Model_HBeam_Single", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_DropObject_Single * CSpecial_DropObject_Single::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_DropObject_Single* pInstance = new CSpecial_DropObject_Single(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_DropObject_Single");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_DropObject_Single::Clone(void * pArg)
{
	CSpecial_DropObject_Single* pInstance = new CSpecial_DropObject_Single(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_DropObject_Single");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_DropObject_Single::Free()
{
	__super::Free();
}
