#include "stdafx.h"
#include "..\public\Talk_BackGroundUI.h"

CTalk_BackGroundUI::CTalk_BackGroundUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTalk_BackGroundUI::CTalk_BackGroundUI(const CTalk_BackGroundUI& rhs)
	: CUI(rhs)
{
}

HRESULT CTalk_BackGroundUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTalk_BackGroundUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_Timeline.SetCurve("UI_LeftTalk");


	return S_OK;
}

void CTalk_BackGroundUI::Tick(_double TimeDelta)
{
	if (false == m_bUse) return;

	__super::Tick(TimeDelta);

	_float fAlpha = 0.0f;
	m_Timeline.Tick(TimeDelta, fAlpha);
	if (1.0f < fAlpha)
		fAlpha = 1.0f;
	m_tParams.Floats[0] = fAlpha;

	if (m_bTimeLine == true)
	{
		m_bTimeLine = false;
		m_Timeline.PlayFromStart();
	}
}

void CTalk_BackGroundUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CTalk_BackGroundUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CTalk_BackGroundUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	//m_Timeline.Imgui_RenderEditor();
	
}

void CTalk_BackGroundUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CTalk_BackGroundUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CTalk_BackGroundUI * CTalk_BackGroundUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTalk_BackGroundUI*		pInstance = new CTalk_BackGroundUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTalk_BackGroundUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CTalk_BackGroundUI::Clone(void * pArg)
{
	CTalk_BackGroundUI*		pInstance = new CTalk_BackGroundUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTalk_BackGroundUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTalk_BackGroundUI::Free()
{
	__super::Free();

}
