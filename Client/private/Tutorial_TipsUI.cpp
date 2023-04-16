#include "stdafx.h"
#include "..\public\Tutorial_TipsUI.h"
#include "GameInstance.h"
#include "UI_Manager.h"

// m_tParams.Floats[0] : 알파값

CTutorial_TipsUI::CTutorial_TipsUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTutorial_TipsUI::CTutorial_TipsUI(const CTutorial_TipsUI& rhs)
	: CUI(rhs)
{
}

HRESULT CTutorial_TipsUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTutorial_TipsUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_fStartSizeX = m_fSizeX;

	CUI_Manager::GetInstance()->PlaySound("TIP");

	return S_OK;
}

void CTutorial_TipsUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	Open(TimeDelta);
	Shut(TimeDelta);
}

void CTutorial_TipsUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CTutorial_TipsUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CTutorial_TipsUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CTutorial_TipsUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CTutorial_TipsUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CTutorial_TipsUI::Open(const _double & dTImeDelta)
{
	if (false == m_bOnTutorial)
		return;

	m_bVisible = true;

	if (true == Change(dTImeDelta, true))
	{
		m_bOnTutorial = false;
	}
}

void CTutorial_TipsUI::Shut(const _double & dTImeDelta)
{
	if (false == m_bOffTutorial)
		return;

	if (true == Change(dTImeDelta, false))
	{
		m_bVisible = false;
		m_bOffTutorial = false;
		m_bEnd = true;
		m_bDelete = true;
	}
}

_bool CTutorial_TipsUI::Change(const _double & dTImeDelta, const _bool & bTutorial)
{
	// 알파값 변화
	if (true == bTutorial) // 증가
	{
		if (1.0f > m_fAlpha)
		{
			m_fAlpha += _float(dTImeDelta) * 1.5f;
			m_tParams.Floats[0] = m_fAlpha;
		}
		else
		{
			m_fAlpha = 1.0f;
			m_tParams.Floats[0] = 1.0f;
		}
	}
	else	// 감소
	{
		if (0.0f < m_fAlpha)
		{
			m_fAlpha -= _float(dTImeDelta) * 1.5f;
			m_tParams.Floats[0] = m_fAlpha;
		}
		else
		{
			m_fAlpha = 0.0f;
			m_tParams.Floats[0] = 0.0f;
		}
	}

	// 사이즈 변화
	if (false == m_bSizeGrowLess)
	{
		if (1.25f > m_fChangeSizeX)
		{
			m_fChangeSizeX += _float(dTImeDelta) * 3.0f;
			m_fSizeX += m_fChangeSizeX;
		}
		else	// 2.0f 보다 커지면 줄어들어야 한다.
		{
			m_bSizeGrowLess = true;
		}
	}
	else // true
	{
		if (0.0f < m_fChangeSizeX)
		{
			m_fChangeSizeX -= _float(dTImeDelta) * 3.0f;
			m_fSizeX -= m_fChangeSizeX;
		}
		else
		{
			m_fSizeX = m_fStartSizeX;	// 원래 값 으로 정확히 초기화

			m_fChangeSizeX = 0.0f;
			m_bSizeGrowLess = false;
			return true;
		}
	}

	return false;
}

CTutorial_TipsUI * CTutorial_TipsUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTutorial_TipsUI*		pInstance = new CTutorial_TipsUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTutorial_TipsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CTutorial_TipsUI::Clone(void * pArg)
{
	CTutorial_TipsUI*		pInstance = new CTutorial_TipsUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTutorial_TipsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_TipsUI::Free()
{
	CUI::Free();

}
