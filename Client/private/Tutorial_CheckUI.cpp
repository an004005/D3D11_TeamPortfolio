#include "stdafx.h"
#include "..\public\Tutorial_CheckUI.h"
#include "GameInstance.h"

// m_tParams.Floats[0] : 알파값

CTutorial_CheckUI::CTutorial_CheckUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTutorial_CheckUI::CTutorial_CheckUI(const CTutorial_CheckUI& rhs)
	: CUI(rhs)
{
}

HRESULT CTutorial_CheckUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTutorial_CheckUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;
	m_fStartSizeX = m_fSizeX;

	return S_OK;
}

void CTutorial_CheckUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
		Set_OnTutorial();

	if (CGameInstance::GetInstance()->KeyDown(DIK_9))
		Set_OffTutorial();

	//Input(TimeDelta);
	
}

void CTutorial_CheckUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CTutorial_CheckUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CTutorial_CheckUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CTutorial_CheckUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CTutorial_CheckUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CTutorial_CheckUI::Input(const _double & dTImeDelta)
{
	if (true == m_bOnTutorial)
	{
		m_bVisible = true;

		if (true == Change(dTImeDelta, true))
		{
			m_bOnTutorial = false;
		}
	}

	if (true == m_bOffTutorial)
	{
		if (true == Change(dTImeDelta, false))
		{
			m_bVisible = false;
			m_bOffTutorial = false;
		}
	}
}

_bool CTutorial_CheckUI::Change(const _double & dTImeDelta, const _bool & bTutorial)
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

CTutorial_CheckUI * CTutorial_CheckUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTutorial_CheckUI*		pInstance = new CTutorial_CheckUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTutorial_CheckUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CTutorial_CheckUI::Clone(void * pArg)
{
	CTutorial_CheckUI*		pInstance = new CTutorial_CheckUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTutorial_CheckUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_CheckUI::Free()
{
	CUI::Free();

}
