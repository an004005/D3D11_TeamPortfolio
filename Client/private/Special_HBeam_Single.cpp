#include "stdafx.h"
#include "..\public\Special_HBeam_Single.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Material.h"

CSpecial_HBeam_Single::CSpecial_HBeam_Single(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_HBeam_Single::CSpecial_HBeam_Single(const CSpecial_HBeam_Single & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_HBeam_Single::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_HBeam_Single::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(1000.f) });

	m_eSpecialObejctType = SPECIAL_HBEAM_SINGLE;

	return S_OK;
}

void CSpecial_HBeam_Single::BeginTick()
{
	__super::BeginTick();

	m_pCollider->UpdateChange();

	m_pCollider->Activate(false);
}

void CSpecial_HBeam_Single::Tick(_double TimeDelta)
{
	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(1500.f) });

	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_HBeam_Single::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_HBeam_Single::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_HBeam_Single::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_HBeam_Single::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_HBeam_Single::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_HBeam_Single::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Local"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_HBeam_Single::Sync_position(CTransform * pTransform)
{
	m_pTransformCom->Set_WorldMatrix(pTransform->Get_WorldMatrix());
	m_pCollider->SetPxWorldMatrix(pTransform->Get_WorldMatrix());
}

void CSpecial_HBeam_Single::Set_Trigger(_bool bTrigger)
{
	m_pCollider->Set_Trigger(bTrigger);
}

void CSpecial_HBeam_Single::Set_Kinetic(_bool bKinetic)
{
	m_pCollider->Set_Kinetic(bKinetic);
	m_pCollider->UpdateChange();
}

void CSpecial_HBeam_Single::HBeam_Turn()
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), g_fTimeDelta);
}

void CSpecial_HBeam_Single::HBeam_Finish()
{
	if (m_bFinish) return;

	m_bFinish = true;

	m_pCollider->Set_Trigger(false);
	m_pCollider->UpdateChange();

	m_pCollider->AddVelocity({ 0.f, 1000.f * g_fTimeDelta, 0.f });
	m_pCollider->AddTorque({ 500.f, 0.f, 0.f });
}

void CSpecial_HBeam_Single::Activate(_bool bActivate)
{
	m_pCollider->Activate(bActivate);
}

HRESULT CSpecial_HBeam_Single::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_HBeam_Single");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_HBeam_Single", L"Model_HBeam_Single", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_HBeam_Single * CSpecial_HBeam_Single::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_HBeam_Single* pInstance = new CSpecial_HBeam_Single(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_HBeam_Single");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_HBeam_Single::Clone(void * pArg)
{
	CSpecial_HBeam_Single* pInstance = new CSpecial_HBeam_Single(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_HBeam_Single");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_HBeam_Single::Free()
{
	__super::Free();
}
