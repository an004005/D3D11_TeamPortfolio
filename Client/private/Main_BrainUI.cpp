#include "stdafx.h"
#include "..\public\Main_BrainUI.h"
#include "GameInstance.h"

CMain_BrainUI::CMain_BrainUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_BrainUI::CMain_BrainUI(const CMain_BrainUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_BrainUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_BrainUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_BrainUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (true == m_bValueDwon)
	{
		m_fValue -= _float(TimeDelta) * 2.0f;

		if (0.0f > m_fValue)
			m_bValueDwon = false;
	}
	else
	{
		m_fValue += _float(TimeDelta) * 2.0f;

		if (1.1f < m_fValue)
			m_bValueDwon = true;
	}

	m_tParams.Floats[1] = m_fValue;
}

void CMain_BrainUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

CMain_BrainUI * CMain_BrainUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_BrainUI*		pInstance = new CMain_BrainUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_BrainUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_BrainUI::Clone(void * pArg)
{
	CMain_BrainUI*		pInstance = new CMain_BrainUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_BrainUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_BrainUI::Free()
{
	__super::Free();

}
