#include "stdafx.h"
#include "Special_IronBars_Door.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Material.h"

CSpecial_IronBars_Door::CSpecial_IronBars_Door(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_IronBars_Door::CSpecial_IronBars_Door(const CSpecial_IronBars_Door& rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_IronBars_Door::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_IronBars_Door::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECAIL_IRONBARS_DOOR;

	return S_OK;
}

void CSpecial_IronBars_Door::BeginTick()
{
	__super::BeginTick();

	m_pModelComs.front()->SetPlayAnimation("AS_ch0100_348_AL_obj");
	m_pModelComs.front()->Play_Animation(0.f);

	m_pCollider->Set_Kinetic(true);
	m_pCollider->UpdateChange();
}

void CSpecial_IronBars_Door::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	PlayAnim(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_IronBars_Door::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_IronBars_Door::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_IronBars_Door::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_IronBars_Door::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_IronBars_Door::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_IronBars_Door::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_IronBars_Door::PlayAnim(_double TimeDelta)
{
	_float fRatio = m_pModelComs.front()->GetPlayAnimation()->GetPlayRatio();

	if (m_bActive)
	{
		if (0.95f >= fRatio)
		{
			m_pModelComs.front()->Play_Animation(TimeDelta);
		}
	}
	else
	{
		if ((_float)TimeDelta <= fRatio)
		{
			m_pModelComs.front()->Play_Animation(-TimeDelta);
		}
		else if (0.f < fRatio)
		{
			m_pModelComs.front()->Play_Animation(_double(fRatio * -1.f));
		}
	}
}

void CSpecial_IronBars_Door::Sync_Position(CTransform* pTransform)
{
	m_pTransformCom->Set_WorldMatrix(m_SyncMatrix * pTransform->Get_WorldMatrix());
}

HRESULT CSpecial_IronBars_Door::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_IronBars_Door");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_IronBars_Door", L"Model_IronBars_Door", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_IronBars_Door* CSpecial_IronBars_Door::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_IronBars_Door* pInstance = new CSpecial_IronBars_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_IronBars_Door");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_IronBars_Door::Clone(void * pArg)
{
	CGameObject* pInstance = new CSpecial_IronBars_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_IronBars_Door");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_IronBars_Door::Free()
{
	__super::Free();
}
