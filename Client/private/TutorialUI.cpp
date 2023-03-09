#include "stdafx.h"
#include "..\public\TutorialUI.h"
#include "GameInstance.h"

// m_tParams.Floats[0] : Alpha

CTutorialUI::CTutorialUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTutorialUI::CTutorialUI(const CTutorialUI& rhs)
	: CUI(rhs)
{
}

HRESULT CTutorialUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTutorialUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_fStartSizeX = m_fSizeX;

	return S_OK;
}

void CTutorialUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	Open(TimeDelta);
	Shut(TimeDelta);
}

void CTutorialUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CTutorialUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CTutorialUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CTutorialUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CTutorialUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CTutorialUI::Open(const _double & dTImeDelta)
{
	if (false == m_bOnTutorial)
		return;

	m_bVisible = true;

	if (true == Change(dTImeDelta, true))
	{
		m_bOnTutorial = false;
	}
}

void CTutorialUI::Shut(const _double & dTImeDelta)
{
	if (true == m_bOffTutorial)
	{
		if (true == Change(dTImeDelta, false))
		{
			m_bVisible = false;
			m_bOffTutorial = false;
			m_bEnd = true;
		}
	}
}

_bool CTutorialUI::Change(const _double & dTImeDelta, const _bool & bTutorial)
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

CTutorialUI * CTutorialUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTutorialUI*		pInstance = new CTutorialUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTutorialUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CTutorialUI::Clone(void * pArg)
{
	CTutorialUI*		pInstance = new CTutorialUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTutorialUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorialUI::Free()
{
	CUI::Free();

}
