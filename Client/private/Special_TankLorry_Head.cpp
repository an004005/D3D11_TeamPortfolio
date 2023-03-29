#include "stdafx.h"
#include "..\public\Special_TankLorry_Head.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"

CSpecial_TankLorry_Head::CSpecial_TankLorry_Head(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_TankLorry_Head::CSpecial_TankLorry_Head(const CSpecial_TankLorry_Head & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_TankLorry_Head::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_TankLorry_Head::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_TANKLORRY_HEAD;

	return S_OK;
}

void CSpecial_TankLorry_Head::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Kinetic(true);
	m_pCollider->UpdateChange();
}

void CSpecial_TankLorry_Head::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_TankLorry_Head::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_TankLorry_Head::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_TankLorry_Head::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_TankLorry_Head::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_TankLorry_Head::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_TankLorry_Head::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_TankLorry_Head::Set_ChildMatrix(_float4x4 matrix)
{
	m_SyncMatrix = matrix;
}

void CSpecial_TankLorry_Head::Sync_position(CTransform * pTransform)
{
	m_pTransformCom->Set_WorldMatrix(m_SyncMatrix * pTransform->Get_WorldMatrix());
}

void CSpecial_TankLorry_Head::Set_Kinetic(_bool bKinetic)
{
	m_pCollider->Set_Kinetic(bKinetic);
	m_pCollider->UpdateChange();
}

void CSpecial_TankLorry_Head::Activate(_bool bActivate)
{
	m_pCollider->Activate(bActivate);
}

void CSpecial_TankLorry_Head::Shake(_float fRange)
{
	_float3 vTorque = {
		CMathUtils::RandomFloat(-fRange, fRange),
		CMathUtils::RandomFloat(-fRange, fRange),
		CMathUtils::RandomFloat(-fRange, fRange) };

	m_pCollider->AddTorque(vTorque);
}

void CSpecial_TankLorry_Head::Bounce(_float fForce)
{
	_float3 vVelocity = { 0.f, fForce * g_fTimeDelta, 0.f };

	m_pCollider->AddVelocity(vVelocity);
}

void CSpecial_TankLorry_Head::Exploision()
{
	_float3 vVelocity = { 0.f, 300.f * g_fTimeDelta, 0.f };

	m_pCollider->AddVelocity(vVelocity);
}

HRESULT CSpecial_TankLorry_Head::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_TankLorry_Head");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TankLorry_Head", L"Model_TankLorry_Head", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_TankLorry_Head * CSpecial_TankLorry_Head::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_TankLorry_Head* pInstance = new CSpecial_TankLorry_Head(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_TankLorry_Head");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_TankLorry_Head::Clone(void * pArg)
{
	CSpecial_TankLorry_Head* pInstance = new CSpecial_TankLorry_Head(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_TankLorry_Head");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_TankLorry_Head::Free()
{
	__super::Free();
}
