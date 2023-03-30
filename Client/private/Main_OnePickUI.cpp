#include "stdafx.h"
#include "..\public\Main_OnePickUI.h"
#include "GameInstance.h"
#include "GameUtils.h"

CMain_OnePickUI::CMain_OnePickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_OnePickUI::CMain_OnePickUI(const CMain_OnePickUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_OnePickUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_OnePickUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_OnePickUI::Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	__super::Tick(TimeDelta);

	if (true == IsCursorOn(CGameUtils::GetClientCursor()) && CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		m_bOnButton = !m_bOnButton;
	}

	CurrentPick(TimeDelta);
}

void CMain_OnePickUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_OnePickUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMain_OnePickUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_OnePickUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_OnePickUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CMain_OnePickUI::CurrentPick(const _double & TimeDelta)
{
	if (false == m_bOnButton)
	{
		m_fAlpha = 0.1f;
		m_tParams.Float4s[0] = { 0.0f, 0.0f, 0.0f, m_fAlpha };

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

	m_tParams.Float4s[0] = { 1.0f, 1.0f, 1.0f, m_fAlpha };
}

CMain_OnePickUI * CMain_OnePickUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_OnePickUI*		pInstance = new CMain_OnePickUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_OnePickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_OnePickUI::Clone(void * pArg)
{
	CMain_OnePickUI*		pInstance = new CMain_OnePickUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_OnePickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_OnePickUI::Free()
{
	__super::Free();

}
