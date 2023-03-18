#include "stdafx.h"
#include "..\public\MainUI.h"
#include "GameInstance.h"

CMainUI::CMainUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMainUI::CMainUI(const CMainUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMainUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMainUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
}

void CMainUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMainUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMainUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMainUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMainUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CMainUI * CMainUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMainUI*		pInstance = new CMainUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMainUI::Clone(void * pArg)
{
	CMainUI*		pInstance = new CMainUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMainUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainUI::Free()
{
	__super::Free();

}
