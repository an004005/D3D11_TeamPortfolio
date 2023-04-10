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

	if (false == m_bChargeGauge) return;

	if (CGameInstance::GetInstance()->KeyPressing(CInput_Device::MOUSEKEYSTATE::DIM_LB))
		m_tParams.Floats[0] += _float(TimeDelta);
	else
	{
		if (0.0f >= m_tParams.Floats[0])
			m_tParams.Floats[0] = 0.0f;
		else
			m_tParams.Floats[0] -= _float(TimeDelta);
	}

	if (1.0f < m_tParams.Floats[0])
	{
		m_bGaugeFull = true;
	}
}

void CMain_BrainGaugeUI::Set_ChargeGauge(const _bool bGauge)
{
	m_bVisible = bGauge;
	m_bChargeGauge = bGauge;

	if (false == bGauge)
	{
		m_bGaugeFull = false;
		m_tParams.Floats[0] = 0.0f;
	}
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
