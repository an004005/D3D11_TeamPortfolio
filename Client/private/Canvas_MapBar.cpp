#include "stdafx.h"
#include "..\public\Canvas_MapBar.h"
#include "GameInstance.h"

#include "Main_PickUI.h"

CCanvas_MapBar::CCanvas_MapBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MapBar::CCanvas_MapBar(const CCanvas_MapBar& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MapBar::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MapBar::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_MapBar::Tick(_double TimeDelta)
{
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	if (false == m_bVisible) return;

	CCanvas::Tick(TimeDelta);
}

void CCanvas_MapBar::Late_Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_MapBar::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	//_float2 vFontSize = { 0.45f, 0.45f };
	//_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	//CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//_tchar szText[MAX_PATH] = TEXT("");
	//wsprintf(szText, TEXT("%d"), tItemInfo.second.iCount);
	//pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(510.0f - fMinusPosition, 13.0f), 0.f, vFontSize, vColor);

	return S_OK;
}

CCanvas_MapBar * CCanvas_MapBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MapBar*		pInstance = new CCanvas_MapBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MapBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MapBar::Clone(void * pArg)
{
	CCanvas_MapBar*		pInstance = new CCanvas_MapBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MapBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MapBar::Free()
{
	CCanvas::Free();

}
