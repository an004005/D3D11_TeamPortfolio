#include "stdafx.h"
#include "..\public\HPlossUI.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"

CHPlossUI::CHPlossUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CHPlossUI::CHPlossUI(const CHPlossUI& rhs)
	: CUI(rhs)
{
}

HRESULT CHPlossUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHPlossUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	return S_OK;
}

void CHPlossUI::Tick(_double TimeDelta)
{
	_uint iMinHP = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP * 0.3;
	if (iMinHP >= CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP)	m_bVisible = true;
	else	m_bVisible = false;

	if (false == m_bVisible) return;

	__super::Tick(TimeDelta);

	m_fSizeX = _float(g_iWinSizeX);
	m_fSizeY = _float(g_iWinSizeY);

	_float fSpeed = 0.8f;
	if (true == m_fAlphaDown)
	{
		m_fAlpha -= _float(TimeDelta) * fSpeed;

		if (0.7f > m_fAlpha)
		{
			m_fAlphaDown = false;
		}
	}
	else
	{
		m_fAlpha += _float(TimeDelta) * fSpeed;

		if (1.0f < m_fAlpha)
		{
			m_fAlphaDown = true;
		}
	}

	m_tParams.Floats[0] = m_fAlpha;
}

CHPlossUI * CHPlossUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CHPlossUI*		pInstance = new CHPlossUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHPlossUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CHPlossUI::Clone(void * pArg)
{
	CHPlossUI*		pInstance = new CHPlossUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHPlossUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHPlossUI::Free()
{
	__super::Free();

}
