#include "stdafx.h"
#include "..\public\Canvas_ItemWindow.h"
#include "GameInstance.h"
#include "Item_Manager.h"

#include "ItemIconUI.h"
#include "Main_PickUI.h"

CCanvas_ItemWindow::CCanvas_ItemWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_ItemWindow::CCanvas_ItemWindow(const CCanvas_ItemWindow& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_ItemWindow::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_ItemWindow::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_ItemWindow::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);
}

void CCanvas_ItemWindow::Late_Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_ItemWindow::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.45f, 0.45f };
	_float4 vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pair<wstring, CItem_Manager::ITEMINFO> ItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo()[m_iItemInfoIndex];

	// 아이템 이름
	_float2 vPosition = Find_ChildUI(L"Itme_B")->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", ItemInfo.first.c_str(), vPosition + _float2(100.0f, 17.0f), 0.f, vFontSize, vColor);

	// 아이템 개수
	_float	fMinusPosition;
	if (ItemInfo.second.iCount == ItemInfo.second.iMaxCount)
	{
		fMinusPosition = 7.0f;
		vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
	}
	else
	{
		fMinusPosition = 0.0f;
		vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
	}
	
	_tchar szText[MAX_PATH] = TEXT("");
	wsprintf(szText, TEXT("%d"), ItemInfo.second.iCount);
	if(CItem_Manager::MAINITEM::BATTLE == ItemInfo.second.eType)
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(580.0f - fMinusPosition, 17.0f), 0.f, vFontSize, vColor);
	else
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(580.0f, 17.0f), 0.f, vFontSize, vColor);

	return S_OK;
}

void CCanvas_ItemWindow::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_ItemWindow::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_ItemWindow::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

_bool CCanvas_ItemWindow::Get_OnButton()
{
	return 	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Itme_B"))->Get_OnButton();
}

void CCanvas_ItemWindow::Set_OnButton()
{
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Itme_B"))->Set_OnButton();
}

void CCanvas_ItemWindow::Set_OnAlpha()
{
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Itme_B"))->Set_OnAlpha();
}

void CCanvas_ItemWindow::Set_ItemWindow(const _float2 & fItemPos, const _float2 & fIconPos, const size_t iIndex)
{
	m_bVisible = true;
	m_iItemInfoIndex = iIndex;
	Find_ChildUI(L"Itme")->Set_Position(fItemPos);
	Find_ChildUI(L"Itme_B")->Set_Position(fItemPos);
	Find_ChildUI(L"Icon")->Set_Position(fIconPos);
	_float2	vIconIndex = CItem_Manager::GetInstance()->Get_ItmeInfo()[m_iItemInfoIndex].second.vIconIndex;
	dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Icon"))->Set_IconIndex(vIconIndex);
}

CCanvas_ItemWindow * CCanvas_ItemWindow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_ItemWindow*		pInstance = new CCanvas_ItemWindow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_ItemWindow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_ItemWindow::Clone(void * pArg)
{
	CCanvas_ItemWindow*		pInstance = new CCanvas_ItemWindow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_ItemWindow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_ItemWindow::Free()
{
	CCanvas::Free();

}
