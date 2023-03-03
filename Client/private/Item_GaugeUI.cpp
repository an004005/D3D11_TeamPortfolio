#include "stdafx.h"
#include "..\public\Item_GaugeUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

// m_tParams.Floats[0] 을 이용해서 게이지를 조절한다.

CItem_GaugeUI::CItem_GaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_GaugeUI::CItem_GaugeUI(const CItem_GaugeUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_GaugeUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_GaugeUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem_GaugeUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (false == m_bCooldownTimeStart)
		return;

	m_tParams.Floats[0] = 1.0f - (_float(TimeDelta) / 10.0f);	// UITODO : 여기서 Item 의 쿨타임을 조절할 수 있습니다.

	if (0.0f > m_tParams.Floats[0])
	{
		m_tParams.Floats[0] = 1.0f;
		m_bCooldownTimeStart = false;
	}
}

void CItem_GaugeUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CItem_GaugeUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_GaugeUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItem_GaugeUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CItem_GaugeUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CItem_GaugeUI::Gauge_Tick(const _double & dTimeDelta)
{

}

CItem_GaugeUI * CItem_GaugeUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_GaugeUI*		pInstance = new CItem_GaugeUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_GaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_GaugeUI::Clone(void * pArg)
{
	CItem_GaugeUI*		pInstance = new CItem_GaugeUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_GaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_GaugeUI::Free()
{
	CUI::Free();

}
