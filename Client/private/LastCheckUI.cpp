#include "stdafx.h"
#include "..\public\LastCheckUI.h"
#include "GameInstance.h"

CLastCheckUI::CLastCheckUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CLastCheckUI::CLastCheckUI(const CLastCheckUI& rhs)
	: CUI(rhs)
{
}

HRESULT CLastCheckUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLastCheckUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLastCheckUI::BeginTick()
{


}

void CLastCheckUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	Open(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
	{
		m_bInput = true;
	}

	if (true == m_bInput)
	{
		Shut(TimeDelta);

	}
}

void CLastCheckUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CLastCheckUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CLastCheckUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CLastCheckUI::Open(const _double& dTImeDelta)
{
	if (false == m_bOnTutorial)
		return;

	m_bVisible = true;

	if (true == Change(dTImeDelta, true))
	{
		m_bOnTutorial = false;
	}
}

void CLastCheckUI::Shut(const _double& dTImeDelta)
{
	if (false == m_bOffTutorial)
		return;

	if (true == Change(dTImeDelta, false))
	{
		m_bVisible = false;
		m_bOffTutorial = false;
		m_bEnd = true;
		m_bDelete = true;
		CGameObject::SetDelete();
	}
}

_bool CLastCheckUI::Change(const _double& dTImeDelta, const _bool& bTutorial)
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

CLastCheckUI * CLastCheckUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CLastCheckUI*		pInstance = new CLastCheckUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLastCheckUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CLastCheckUI::Clone(void * pArg)
{
	CLastCheckUI*		pInstance = new CLastCheckUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLastCheckUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLastCheckUI::Free()
{
	CUI::Free();

}
