#include "stdafx.h"
#include "..\public\Main_PickUI.h"
#include "GameInstance.h"
#include "GameUtils.h"

CMain_PickUI::CMain_PickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_PickUI::CMain_PickUI(const CMain_PickUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_PickUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_PickUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_PickUI::Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	__super::Tick(TimeDelta);

	if (true == IsCursorOn(CGameUtils::GetClientCursor()) && CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		m_bOnButton = true;
		m_bOnAlpha = true;
	}

	if (2 == m_iColorType)	// 무색 (Pass 4번 사용!)
		return;

	CurrentPick(TimeDelta);
}

void CMain_PickUI::CurrentPick(const _double & TimeDelta)
{
	if (false == m_bOnAlpha)
	{
		m_fAlpha = 0.1f;

		//if (0 == m_iColorType)
		//	m_tParams.Float4s[0] = { 0.0f, 0.0f, 0.0f, m_fAlpha };
		//else if (1 == m_iColorType)
		//	m_tParams.Float4s[0] = { 0.27f, 0.25f, 0.2f, m_fAlpha };

		return;
	}

	if (true == m_fAlphaDown)
	{
		m_fAlpha -= _float(TimeDelta) * 0.15f;

		if (0.05f > m_fAlpha)
		{
			m_fAlphaDown = false;
		}
	}
	else
	{
		m_fAlpha += _float(TimeDelta) * 0.15f;

		if (0.1f < m_fAlpha)
		{
			m_fAlphaDown = true;
		}
	}

	//if (0 == m_iColorType)
	//	m_tParams.Float4s[0] = { 1.0f, 1.0f, 1.0f, m_fAlpha };
	//else if (1 == m_iColorType)
	//	m_tParams.Float4s[0] = { 0.27f, 0.25f, 0.2f, m_fAlpha };
}

CMain_PickUI * CMain_PickUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_PickUI*		pInstance = new CMain_PickUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_PickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_PickUI::Clone(void * pArg)
{
	CMain_PickUI*		pInstance = new CMain_PickUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_PickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_PickUI::Free()
{
	__super::Free();

}
