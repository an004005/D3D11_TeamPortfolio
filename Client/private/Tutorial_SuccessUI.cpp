#include "stdafx.h"
#include "..\public\Tutorial_SuccessUI.h"
#include "GameInstance.h"

// g_vec2_0 : 출력할 인덱스
// g_vec2_1 : [x] 가로로 자를 개수, [y] 세로로 자를 개수

CTutorial_SuccessUI::CTutorial_SuccessUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTutorial_SuccessUI::CTutorial_SuccessUI(const CTutorial_SuccessUI& rhs)
	: CUI(rhs)
{
}

HRESULT CTutorial_SuccessUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTutorial_SuccessUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CTutorial_SuccessUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (false == m_bVisible)
		return;

	m_bSuccessEnd = false;

	m_dChange_TimcAcc += TimeDelta;
	if (0.9 < m_dChange_TimcAcc)
	{
		m_tParams.Float2s[0].x = 1;
	}
	if (1.0 < m_dChange_TimcAcc)
	{
		m_dChange_TimcAcc = 0.0;
		m_tParams.Float2s[0].x = 0; 
		++m_iDeleteCount;
	}

	if (1 < m_iDeleteCount)
	{
		m_iDeleteCount = 0;
		m_dChange_TimcAcc = 0.0;
		m_tParams.Float2s[0].x = 0;
		m_bVisible = false;
		m_bSuccessEnd = true;
	}
}

void CTutorial_SuccessUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CTutorial_SuccessUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CTutorial_SuccessUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CTutorial_SuccessUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CTutorial_SuccessUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CTutorial_SuccessUI * CTutorial_SuccessUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTutorial_SuccessUI*		pInstance = new CTutorial_SuccessUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTutorial_SuccessUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CTutorial_SuccessUI::Clone(void * pArg)
{
	CTutorial_SuccessUI*		pInstance = new CTutorial_SuccessUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTutorial_SuccessUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_SuccessUI::Free()
{
	CUI::Free();

}
