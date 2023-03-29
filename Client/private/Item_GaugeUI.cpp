#include "stdafx.h"
#include "..\public\Item_GaugeUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

// 초록색 : 0.5f, 1.0f, 0.4f, 안씀
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
	if (false == m_bCooldownTimeStart)
		return;

	CUI::Tick(TimeDelta);

	if (true == m_fRatioDown)
	{
		m_fRatio -= _float(TimeDelta) * 0.1f;

		if (0.0f > m_fRatio)
			m_fRatioDown = false;
	}
	else
	{
		m_fRatio += _float(TimeDelta) * 0.1f;

		if (1.0f < m_fRatio)
		{
			m_fRatio = 1.0f;
			m_bCooldownTimeStart = false;
		}
	}

	m_tParams.Floats[0] = m_fRatio;
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
