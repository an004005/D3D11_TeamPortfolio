#include "stdafx.h"
#include "..\public\SASInfoRightHpBothEndsUI.h"
#include "GameInstance.h"

// m_tParams.Floats[0] : ∞‘¿Ã¡ˆ

CSASInfoRightHpBothEndsUI::CSASInfoRightHpBothEndsUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoRightHpBothEndsUI::CSASInfoRightHpBothEndsUI(const CSASInfoRightHpBothEndsUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoRightHpBothEndsUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoRightHpBothEndsUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	// 0: EndHP, 1:  EndHpBack, 2: StartHp, 3: StartHpBack
	static _int iObjectCount;
	m_iObjectNumber = iObjectCount;
	++iObjectCount;

	if (4 == iObjectCount) iObjectCount = 0;

	m_fHp = 1.0f;
	m_fCurrentHp = 1.0f;
	m_tParams.Floats[0] = 1.0f;

	return S_OK;
}

void CSASInfoRightHpBothEndsUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (0 == m_iObjectNumber || 2 == m_iObjectNumber)
	{
		if (m_fCurrentHp < m_fHp)
			m_fCurrentHp += _float(TimeDelta) * 0.1f;
		else
			m_fCurrentHp = m_fHp;
	}
	else
	{
		if (m_fCurrentHp < m_fHp)
			m_fCurrentHp += _float(TimeDelta) * 0.1f;
		else
			m_fCurrentHp -= _float(TimeDelta) * 0.1f;
	}


	if (2 == m_iObjectNumber || 3 == m_iObjectNumber)
		Start_Tick(TimeDelta);
	else
		End_Tick(TimeDelta);
}

void CSASInfoRightHpBothEndsUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoRightHpBothEndsUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoRightHpBothEndsUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoRightHpBothEndsUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoRightHpBothEndsUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CSASInfoRightHpBothEndsUI::RendomHpImage(const _int & iImageNumber)
{
	m_bVisible = true;
	m_tParams.Floats[0] = 1.0f;
}

void CSASInfoRightHpBothEndsUI::Start_Tick(const _double & dTimeDetla)
{
	_float fCurHp = Clamp<_float>(m_fCurrentHp, 0.0f, 0.05f);
	m_tParams.Floats[0] = Remap<float>(fCurHp, 0.0f, 0.05f, 0.0f, 1.0f);
}

void CSASInfoRightHpBothEndsUI::End_Tick(const _double & dTimeDetla)
{
	_float fCurrentHp = m_fCurrentHp - 0.95f;
	m_tParams.Floats[0] = _float(fCurrentHp / 0.05f);
}

CSASInfoRightHpBothEndsUI * CSASInfoRightHpBothEndsUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoRightHpBothEndsUI*		pInstance = new CSASInfoRightHpBothEndsUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoRightHpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoRightHpBothEndsUI::Clone(void * pArg)
{
	CSASInfoRightHpBothEndsUI*		pInstance = new CSASInfoRightHpBothEndsUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoRightHpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoRightHpBothEndsUI::Free()
{
	CUI::Free();

}
