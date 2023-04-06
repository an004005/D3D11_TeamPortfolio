#include "stdafx.h"
#include "..\public\FullUI.h"
#include "GameInstance.h"

CFullUI::CFullUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CFullUI::CFullUI(const CFullUI& rhs)
	: CUI(rhs)
{
}

HRESULT CFullUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFullUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CFullUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fSizeX = _float(g_iWinSizeX);
	m_fSizeY = _float(g_iWinSizeY);

	if (m_bAlpha == false) 
		return;

	if (m_bReverse == false && m_fAlpha >= 0.5f)
		m_bReverse = true;
	else if (m_bReverse == true && m_fAlpha <= 0.f)
	{
		m_bReverse = false;
		m_bAlpha = false;
	}

	_float fSpeed = 0.7f;
	if (m_bReverse == false)
		m_fAlpha += _float(TimeDelta) * fSpeed;
	else
		m_fAlpha -= _float(TimeDelta) * fSpeed;

	m_tParams.Float4s[0].w = m_fAlpha;
}

CFullUI * CFullUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CFullUI*		pInstance = new CFullUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFullUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CFullUI::Clone(void * pArg)
{
	CFullUI*		pInstance = new CFullUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFullUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFullUI::Free()
{
	__super::Free();

}
