#include "stdafx.h"
#include "Special_IronBars_SingleBars.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"

CSpecial_IronBars_SingleBars::CSpecial_IronBars_SingleBars(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_IronBars_SingleBars::CSpecial_IronBars_SingleBars(const CSpecial_IronBars_SingleBars& rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_IronBars_SingleBars::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_IronBars_SingleBars::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_IRONBARS_SINGLEBARS;

	return S_OK;
}

void CSpecial_IronBars_SingleBars::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Kinetic(true);
	m_pCollider->UpdateChange();
}

void CSpecial_IronBars_SingleBars::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_IronBars_SingleBars::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_IronBars_SingleBars::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_IronBars_SingleBars::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_IronBars_SingleBars::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_IronBars_SingleBars::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_IronBars_SingleBars::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_IronBars_SingleBars::Attach_BoneMatrix(CModel* pModel, CTransform* pTransform, const string& strBoneName)
{
	_matrix	SocketMatrix = XMMatrixInverse(nullptr, m_LocalMatrix) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * pModel->GetBoneMatrix(strBoneName) * pTransform->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CSpecial_IronBars_SingleBars::Set_Kinetic(_bool bKinetic)
{
	m_pCollider->Set_Kinetic(bKinetic);
	m_pCollider->UpdateChange();
}

HRESULT CSpecial_IronBars_SingleBars::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_IronBars_SingleBars");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_IronBars_SingleBars", L"Model_IronBars_SingleBars", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_IronBars_SingleBars* CSpecial_IronBars_SingleBars::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_IronBars_SingleBars* pInstance = new CSpecial_IronBars_SingleBars(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_IronBars_SingleBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_IronBars_SingleBars::Clone(void * pArg)
{
	CGameObject* pInstance = new CSpecial_IronBars_SingleBars(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_IronBars_SingleBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_IronBars_SingleBars::Free()
{
	__super::Free();
}
