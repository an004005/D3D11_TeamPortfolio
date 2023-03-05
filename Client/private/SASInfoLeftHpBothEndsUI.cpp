#include "stdafx.h"
#include "..\public\SASInfoLeftHpBothEndsUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CSASInfoLeftHpBothEndsUI::CSASInfoLeftHpBothEndsUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoLeftHpBothEndsUI::CSASInfoLeftHpBothEndsUI(const CSASInfoLeftHpBothEndsUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoLeftHpBothEndsUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoLeftHpBothEndsUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	// 0: EndHP, 1:  EndHpBack, 2: StartHp, 3: StartHpBack
	static _int iObjectCount;
	m_iObjectNumber = iObjectCount;
	++iObjectCount;

	m_tParams.Floats[0] = 0.0f;

	return S_OK;
}

void CSASInfoLeftHpBothEndsUI::Tick(_double TimeDelta)
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

void CSASInfoLeftHpBothEndsUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoLeftHpBothEndsUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpBothEndsUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoLeftHpBothEndsUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoLeftHpBothEndsUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CSASInfoLeftHpBothEndsUI::RendomHpImage(const _int & iImageNumber)
{
	m_bVisible = true;
	m_tParams.Floats[0] = 1.0f;
}

void CSASInfoLeftHpBothEndsUI::Start_Tick(const _double & dTimeDetla)
{
	_float fCurHp = Clamp<_float>(m_fCurrentHp, 0.0f, 0.05f);
	m_tParams.Floats[0] = Remap<float>(fCurHp, 0.0f, 0.05f, 0.0f, 1.0f);
}

void CSASInfoLeftHpBothEndsUI::End_Tick(const _double & dTimeDetla)
{
	_float fCurrentHp = m_fCurrentHp - 0.95f;
	m_tParams.Floats[0] = _float(fCurrentHp / 0.05f);
}

CSASInfoLeftHpBothEndsUI * CSASInfoLeftHpBothEndsUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoLeftHpBothEndsUI*		pInstance = new CSASInfoLeftHpBothEndsUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoLeftHpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoLeftHpBothEndsUI::Clone(void * pArg)
{
	CSASInfoLeftHpBothEndsUI*		pInstance = new CSASInfoLeftHpBothEndsUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoLeftHpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoLeftHpBothEndsUI::Free()
{
	CUI::Free();

}
