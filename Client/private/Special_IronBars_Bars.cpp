#include "stdafx.h"
#include "Special_IronBars_Bars.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Material.h"

CSpecial_IronBars_Bars::CSpecial_IronBars_Bars(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_IronBars_Bars::CSpecial_IronBars_Bars(const CSpecial_IronBars_Bars& rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_IronBars_Bars::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_IronBars_Bars::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_IRONBARS_BARS;

	return S_OK;
}

void CSpecial_IronBars_Bars::BeginTick()
{
	__super::BeginTick();

	m_pModelComs.front()->SetPlayAnimation("AS_ch0100_348_AL_obj");
	m_pModelComs.front()->Play_Animation(0.f);
}

void CSpecial_IronBars_Bars::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	PlayAnim(TimeDelta);
}

void CSpecial_IronBars_Bars::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_IronBars_Bars::AfterPhysX()
{
	__super::AfterPhysX();
}

HRESULT CSpecial_IronBars_Bars::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_IronBars_Bars::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_IronBars_Bars::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_IronBars_Bars::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}

	ImGui::Checkbox("Anim Active", &m_bActive);
}

void CSpecial_IronBars_Bars::PlayAnim(_double TimeDelta)
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

void CSpecial_IronBars_Bars::Sync_Position(CTransform* pTransform)
{
	m_pTransformCom->Set_WorldMatrix(m_SyncMatrix * pTransform->Get_WorldMatrix());
}

HRESULT CSpecial_IronBars_Bars::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_IronBars_Bars");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_IronBars_Bars", L"Model_IronBars_Bars", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	return S_OK;
}

CSpecial_IronBars_Bars* CSpecial_IronBars_Bars::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_IronBars_Bars* pInstance = new CSpecial_IronBars_Bars(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_IronBars_Bars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_IronBars_Bars::Clone(void * pArg)
{
	CGameObject* pInstance = new CSpecial_IronBars_Bars(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_IronBars_Bars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_IronBars_Bars::Free()
{
	__super::Free();
}
