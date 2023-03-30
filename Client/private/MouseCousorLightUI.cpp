#include "stdafx.h"
#include "..\public\MouseCousorLightUI.h"

CMouseCousorLightUI::CMouseCousorLightUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMouseCousorLightUI::CMouseCousorLightUI(const CMouseCousorLightUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMouseCousorLightUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMouseCousorLightUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	return S_OK;
}

void CMouseCousorLightUI::Tick(_double TimeDelta)
{
	if (false == m_fMouseLight) return;

	__super::Tick(TimeDelta);

	if (true == m_fAlphaDown)
	{
		m_fAlpha -= _float(TimeDelta);

		if (0.5f > m_fAlpha)
		{
			m_fAlphaDown = false;
		}
	}
	else
	{
		m_fAlpha += _float(TimeDelta);

		if (1.0f < m_fAlpha)
		{
			m_fAlphaDown = true;
		}
	}

	m_tParams.Floats[0] = m_fAlpha;
}

void CMouseCousorLightUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

CMouseCousorLightUI * CMouseCousorLightUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMouseCousorLightUI*		pInstance = new CMouseCousorLightUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMouseCousorLightUI::Clone(void * pArg)
{
	CMouseCousorLightUI*		pInstance = new CMouseCousorLightUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMouseCousorLightUI::Free()
{
	__super::Free();

}
