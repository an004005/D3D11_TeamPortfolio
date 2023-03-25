#include "stdafx.h"
#include "..\public\Main_BrainGaugeUI.h"
#include "GameInstance.h"

CMain_BrainGaugeUI::CMain_BrainGaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_BrainGaugeUI::CMain_BrainGaugeUI(const CMain_BrainGaugeUI& rhs)
	: CUI(rhs)
{ 
}

HRESULT CMain_BrainGaugeUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_BrainGaugeUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	return S_OK;
}

void CMain_BrainGaugeUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//IM_LOG("UI Down %i", m_bChargeGauge);
	//IM_LOG("----------------Gauge %i", m_tParams.Floats[0]);
	if (true == m_bChargeGauge)
	{
		m_tParams.Floats[0] += _float(TimeDelta) * 0.5f;

		if (0.999f < m_tParams.Floats[0])
		{
			m_bGaugeFull = true;
			m_bVisible = false;
			m_tParams.Floats[0] = 0.0f;
		}
	}
	else
	{
		m_tParams.Floats[0] -= _float(TimeDelta) * 0.5f;
	}
}

void CMain_BrainGaugeUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_BrainGaugeUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMain_BrainGaugeUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_BrainGaugeUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_BrainGaugeUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CMain_BrainGaugeUI * CMain_BrainGaugeUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_BrainGaugeUI*		pInstance = new CMain_BrainGaugeUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_BrainGaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_BrainGaugeUI::Clone(void * pArg)
{
	CMain_BrainGaugeUI*		pInstance = new CMain_BrainGaugeUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_BrainGaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_BrainGaugeUI::Free()
{
	__super::Free();

}
