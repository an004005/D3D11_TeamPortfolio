#include "stdafx.h"
#include "..\public\Item_LightUI.h"
#include "GameInstance.h"

// 초록색 : 0.4f, 1.0f, 0.4f, (0.25f ~ 0.7f)
// 주황색 : 

CItem_LightUI::CItem_LightUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_LightUI::CItem_LightUI(const CItem_LightUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_LightUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_LightUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem_LightUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bUse == false) return;

	m_dAlpha_TimeAcc += TimeDelta;
	if (1.0 < m_dAlpha_TimeAcc)
	{
		m_fAlpha = 0.25f;
		m_tParams.Float4s[0].w = m_fAlpha;
		m_bUse = false;
		return;
	}

	if (true == m_fAlphaDown)
	{
		m_fAlpha -= _float(TimeDelta) * 3.0f;

		if (0.25f > m_fAlpha)
			m_fAlphaDown = false;
	}
	else
	{
		m_fAlpha += _float(TimeDelta) * 3.0f;

		if (0.7f < m_fAlpha)
			m_fAlphaDown = true;
	}

	m_tParams.Float4s[0].w = m_fAlpha;
}

void CItem_LightUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CItem_LightUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_LightUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItem_LightUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CItem_LightUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CItem_LightUI * CItem_LightUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_LightUI*		pInstance = new CItem_LightUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_LightUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_LightUI::Clone(void * pArg)
{
	CItem_LightUI*		pInstance = new CItem_LightUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_LightUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_LightUI::Free()
{
	__super::Free();

}
