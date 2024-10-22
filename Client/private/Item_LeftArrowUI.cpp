#include "stdafx.h"
#include "..\public\Item_LeftArrowUI.h"
#include "GameInstance.h"
#include "GameUtils.h"

CItem_LeftArrowUI::CItem_LeftArrowUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_LeftArrowUI::CItem_LeftArrowUI(const CItem_LeftArrowUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_LeftArrowUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_LeftArrowUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_fStartX = m_fX;

	return S_OK;
}

void CItem_LeftArrowUI::Tick(_double TimeDelta)
{
	if (m_bMouse == true)
	{
		if (true == IsCursorOn(CGameUtils::GetClientCursor()) && CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
		{
			m_bCursorKeyDown = true;
		}
	}

	CUI::Tick(TimeDelta);

	if (false == m_bInput)	return;

	m_fMoveTimeAcc += TimeDelta;
	if (0.15 < m_fMoveTimeAcc)
	{
		m_fX += _float(TimeDelta) * 100.0f;

		if (m_fX > m_fStartX)
		{
			m_fX = m_fStartX;
			m_fMoveTimeAcc = 0.0;
			m_bInput = false;
		}
	}
	else
	{
		m_fX -= _float(TimeDelta) * 100.0f;
	}
}

CItem_LeftArrowUI * CItem_LeftArrowUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_LeftArrowUI*		pInstance = new CItem_LeftArrowUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_LeftArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_LeftArrowUI::Clone(void * pArg)
{
	CItem_LeftArrowUI*		pInstance = new CItem_LeftArrowUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_LeftArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_LeftArrowUI::Free()
{
	CUI::Free();

}
