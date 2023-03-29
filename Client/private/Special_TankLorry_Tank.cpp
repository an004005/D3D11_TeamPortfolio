#include "stdafx.h"
#include "..\public\Special_TankLorry_Tank.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"

CSpecial_TankLorry_Tank::CSpecial_TankLorry_Tank(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_TankLorry_Tank::CSpecial_TankLorry_Tank(const CSpecial_TankLorry_Tank & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_TankLorry_Tank::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_TankLorry_Tank::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_TANKLORRY_TANK;

	return S_OK;
}

void CSpecial_TankLorry_Tank::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Kinetic(false);
	m_pCollider->UpdateChange();
}

void CSpecial_TankLorry_Tank::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_TankLorry_Tank::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_TankLorry_Tank::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_TankLorry_Tank::Render()
{
	const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();
	FAILED_CHECK(m_pModelComs[m_iCurModelIdx]->Render(WorldMatrix));

	return S_OK;
}

void CSpecial_TankLorry_Tank::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_TankLorry_Tank::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_TankLorry_Tank::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::InputInt("ModelType", &m_iCurModelIdx);

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

HRESULT CSpecial_TankLorry_Tank::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_TankLorry_Tank_0");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Tank_0", L"Model_TankLorry_Tank_0", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_TankLorry_Tank_1");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Tank_1", L"Model_TankLorry_Tank_1", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_TankLorry_Tank_2");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Tank_2", L"Model_TankLorry_Tank_2", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	m_pModelTags.push_back(L"Model_TankLorry_Tank_3");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Tank_3", L"Model_TankLorry_Tank_3", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_TankLorry_Tank * CSpecial_TankLorry_Tank::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_TankLorry_Tank* pInstance = new CSpecial_TankLorry_Tank(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_TankLorry_Tank");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_TankLorry_Tank::Clone(void * pArg)
{
	CSpecial_TankLorry_Tank* pInstance = new CSpecial_TankLorry_Tank(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_TankLorry_Tank");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_TankLorry_Tank::Free()
{
	__super::Free();
}
