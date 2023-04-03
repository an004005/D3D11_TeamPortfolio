#include "stdafx.h"
#include "..\public\Canvas_ShopListBar.h"
#include "GameInstance.h"
#include "Item_Manager.h"

#include "ItemIconUI.h"
#include "Main_PickUI.h"

CCanvas_ShopListBar::CCanvas_ShopListBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_ShopListBar::CCanvas_ShopListBar(const CCanvas_ShopListBar& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_ShopListBar::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_ShopListBar::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_WondowB"))->Set_ColorType(2);

	return S_OK;
}

void CCanvas_ShopListBar::Tick(_double TimeDelta)
{
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	if (false == m_bVisible) return;

	CCanvas::Tick(TimeDelta);
}

void CCanvas_ShopListBar::Late_Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_ShopListBar::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.45f, 0.45f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pair<wstring, CItem_Manager::ITEMINFO> tItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo()[m_iItemInfoIndex];

	// 아이템 이름
	_float2 vPosition = Find_ChildUI(L"Shop_WondowB")->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", tItemInfo.first.c_str(), vPosition + _float2(50.0f, 13.0f), 0.f, vFontSize, vColor);

	// 소지수 (if 빨간색, else 일반색)
	_float	fMinusPosition;
	if (tItemInfo.second.iCount == tItemInfo.second.iMaxCount)
	{
		fMinusPosition = 5.0f;
		vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
	}
	else
	{
		fMinusPosition = 0.0f;
		vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	_tchar szText[MAX_PATH] = TEXT("");
	wsprintf(szText, TEXT("%d"), tItemInfo.second.iCount);
	if (CItem_Manager::MAINITEM::BATTLE == tItemInfo.second.eType)
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(510.0f - fMinusPosition, 13.0f), 0.f, vFontSize, vColor);
	else
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(510.0f, 13.0f), 0.f, vFontSize, vColor);
	
	vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	// 금액 m_iPrice
	if(false == m_bSale)
		wsprintf(szText, TEXT("%u"), tItemInfo.second.iPrice);
	else
		wsprintf(szText, TEXT("%u"), tItemInfo.second.iSellingPrice);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(580.0f, 13.0f), 0.f, vFontSize, vColor);
	
	// 개수
	wsprintf(szText, TEXT("%u"), m_iNmberPurchases);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(692.0f, 13.0f), 0.f, vFontSize, vColor);

	return S_OK;
}

_uint CCanvas_ShopListBar::Get_ItemCount()
{
	return CItem_Manager::GetInstance()->Get_ItmeInfo()[m_iItemInfoIndex].second.iCount;
}

_float2 CCanvas_ShopListBar::Get_ListPosititon()
{
	return Find_ChildUI(L"Shop_WondowB")->Get_Position();
}

_bool CCanvas_ShopListBar::Get_OnButton()
{
	return 	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_WondowB"))->Get_OnButton();
}

void CCanvas_ShopListBar::Set_OnButton()
{
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_WondowB"))->Set_OnButton();
}

void CCanvas_ShopListBar::Set_OnAlpha()
{
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_WondowB"))->Set_OnAlpha();
}

void CCanvas_ShopListBar::Set_ShopListBar(const _float2& fItemPos, const _float2& fIconPos, const size_t iIndex)
{
	m_bVisible = true;
	m_iItemInfoIndex = iIndex;
	Find_ChildUI(L"Shop_Wondow")->Set_Position(fItemPos);
	Find_ChildUI(L"Shop_WondowB")->Set_Position(fItemPos);
	Find_ChildUI(L"Shop_WindowIcon")->Set_Position(fIconPos);
	_float2	vIconIndex = CItem_Manager::GetInstance()->Get_ItmeInfo()[m_iItemInfoIndex].second.vIconIndex;
	dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Shop_WindowIcon"))->Set_IconIndex(vIconIndex);

	wstring wsItemName = CItem_Manager::GetInstance()->Get_ItmeInfo()[m_iItemInfoIndex].first;
}

CCanvas_ShopListBar * CCanvas_ShopListBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_ShopListBar*		pInstance = new CCanvas_ShopListBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_ShopListBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_ShopListBar::Clone(void * pArg)
{
	CCanvas_ShopListBar*		pInstance = new CCanvas_ShopListBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_ShopListBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_ShopListBar::Free()
{
	CCanvas::Free();

}
