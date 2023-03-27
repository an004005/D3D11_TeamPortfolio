#include "stdafx.h"
#include "..\public\Main_BrainMapIconPickUI.h"
#include "GameInstance.h"

CMain_BrainMapIconPickUI::CMain_BrainMapIconPickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_BrainMapIconPickUI::CMain_BrainMapIconPickUI(const CMain_BrainMapIconPickUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_BrainMapIconPickUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_BrainMapIconPickUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_BrainMapIconPickUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (true == m_bPick)
	{
		m_fAlpha = 1.0f;
		m_bPick = false;
	}

	if (true == m_fAlphaDown)
	{
		m_fAlpha -= _float(TimeDelta) * 0.5f;

		if (0.3f > m_fAlpha)
			m_fAlphaDown = false;
	}
	else
	{
		m_fAlpha += _float(TimeDelta) * 0.5f;

		if (0.6f < m_fAlpha)
			m_fAlphaDown = true;
	}

	m_tParams.Floats[0] = m_fAlpha;


}

void CMain_BrainMapIconPickUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

CMain_BrainMapIconPickUI * CMain_BrainMapIconPickUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_BrainMapIconPickUI*		pInstance = new CMain_BrainMapIconPickUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_BrainMapIconPickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_BrainMapIconPickUI::Clone(void * pArg)
{
	CMain_BrainMapIconPickUI*		pInstance = new CMain_BrainMapIconPickUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_BrainMapIconPickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_BrainMapIconPickUI::Free()
{
	__super::Free();

}
