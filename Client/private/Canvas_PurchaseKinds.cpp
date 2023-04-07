#include "stdafx.h"
#include "..\public\Canvas_PurchaseKinds.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "Item_Manager.h"
#include "PlayerInfoManager.h"

#include "Canvas_Shortage.h"
#include "Canvas_ListCheck.h"
#include "Canvas_ShopListBar.h"
#include "Main_PickUI.h"
#include "DefaultUI.h"
#include "ItemIconUI.h"
#include "Item_LeftArrowUI.h"
#include "Item_RightArrowUI.h"
#include "ButtonUI.h"

CCanvas_PurchaseKinds::CCanvas_PurchaseKinds(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_PurchaseKinds::CCanvas_PurchaseKinds(const CCanvas_PurchaseKinds& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_PurchaseKinds::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_PurchaseKinds::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	Add_ShopListCanvas();
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ListCheck.json");
	m_pCanvas_ListCheck = dynamic_cast<CCanvas_ListCheck*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_ListCheck", &json));
	assert(m_pCanvas_ListCheck != nullptr && "Failed to Cloned : CCanvas_ListCheck");
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Shortage.json");
	m_pCanvas_Shortage = dynamic_cast<CCanvas_Shortage*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_Shortage", &json));
	assert(m_pCanvas_Shortage != nullptr && "Failed to Cloned : CCanvas_Shortage");

	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_CurrentWondowB"))->Set_PickInitialize();

	dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Shop_ItemIcon"))->Set_IconIndex(m_vecItemInfo[m_iItemIndex].second.vIconIndex);
	Find_ChildUI(L"Shop_CurrentCount")->Set_Position(m_vecShopCanvass[0].second->Get_ListPosititon());
	Find_ChildUI(L"Shop_CurrentWondowB")->Set_Position(m_vecShopCanvass[0].second->Get_ListPosititon());
	Find_ChildUI(L"Shop_LeftArrow")->Set_Position({ Find_ChildUI(L"Shop_LeftArrow")->Get_Position().x, m_vecShopCanvass[0].second->Get_ListPosititon().y });
	Find_ChildUI(L"Shop_RightArrow")->Set_Position({ Find_ChildUI(L"Shop_RightArrow")->Get_Position().x, m_vecShopCanvass[0].second->Get_ListPosititon().y });

	//m_vecShopCanvass.erase(remove(m_vecShopCanvass.begin(), m_vecShopCanvass.end(), 3), m_vecShopCanvass.end());

	dynamic_cast<CItem_LeftArrowUI*>(Find_ChildUI(L"Shop_LeftArrow"))->Set_MouseInput();
	dynamic_cast<CItem_RightArrowUI*>(Find_ChildUI(L"Shop_RightArrow"))->Set_MouseInput();

	return S_OK;
}

void CCanvas_PurchaseKinds::Tick(_double TimeDelta)
{
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	for (vector<pair<wstring, CCanvas_ShopListBar*>>::iterator iter = m_vecShopCanvass.begin(); iter != m_vecShopCanvass.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	if (m_bVisible == false) return;

	CCanvas::Tick(TimeDelta);

	m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	CurrentList();
	Key_Input();
}

void CCanvas_PurchaseKinds::Late_Tick(_double TimeDelta)
{
	if (m_bVisible == false) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_PurchaseKinds::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontBigSize = { 0.5f, 0.5f };
	_float2 vFontSmaillSize = { 0.45f, 0.45f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 오른쪽 상단 아이템 설명
	_float2 vPosition = Find_ChildUI(L"Shop_ItemInfo")->GetScreenSpaceLeftTop();
	
	pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].first.c_str(), vPosition + _float2(110.0f, 30.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].second.szItemEx[0], vPosition + _float2(35.0f, 100.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].second.szItemEx[1], vPosition + _float2(35.0f, 130.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].second.szItemEx[2], vPosition + _float2(35.0f, 160.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].second.szItemEx[3], vPosition + _float2(35.0f, 190.0f), 0.f, vFontBigSize, vColor);

	pGameInstance->Render_Font(L"Pretendard32", L"소지수", vPosition + _float2(460.0f, 55.0f ), 0.f, vFontSmaillSize, vColor);

	_uint iItemCount = m_vecItemInfo[m_iItemIndex].second.iCount;
	if (iItemCount >= 10)
		vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
	else
		vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	wsprintf(szText, TEXT("%u"), iItemCount);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(530.0f, 55.0f), 0.f, vFontSmaillSize, vColor);

	vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (m_vecItemInfo[m_iItemIndex].second.eType == CItem_Manager::MAINITEM::BATTLE)
		wsprintf(szText, TEXT("/  %d"), 10);
	else
		wsprintf(szText, TEXT("/  %d"), 1);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(560.0f, 55.0f), 0.f, vFontSmaillSize, vColor);

	// 오른쪽 하단 구매 내역
	vPosition = Find_ChildUI(L"Shop_Payment")->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"소지금", vPosition + _float2(20.0f, 11.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"구입 후 소지금", vPosition + _float2(20.0f, 41.0f), 0.f, vFontSmaillSize, vColor);

	wsprintf(szText, TEXT("%u"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 15.0f), 0.f, vFontSmaillSize, vColor);

	_int iResult = static_cast<_int>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin) - (m_vecItemInfo[m_iItemIndex].second.iPrice * m_iItemCount);
	if (0 >= iResult)
		vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
	else
		vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	wsprintf(szText, TEXT("%d"), iResult);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 48.0f), 0.f, vFontSmaillSize, vColor);

	vColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	_uint iItemValueResult = m_vecItemInfo[m_iItemIndex].second.iPrice * m_iItemCount;
	wsprintf(szText, TEXT("%d"), iItemValueResult);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 82.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"구입 금액", vPosition + _float2(20.0f, 80.0f), 0.f, vFontSmaillSize, vColor);

	return S_OK;
}

void CCanvas_PurchaseKinds::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_PurchaseKinds::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

	m_eMainItem = json["ITemType"];
}

void CCanvas_PurchaseKinds::Add_ShopListCanvas()
{
	m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	size_t iVecSize = m_vecItemInfo.size();

	if (m_eMainItem == CItem_Manager::MAINITEM::ALL)
	{
		// ALL 아이템 즉, BATTAL 과 WEAPON 의 아이템을 모두 만든다.
		for (size_t i = 0; i < iVecSize; i++)
		{
			if (m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::BATTLE || m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::WEAPON)
			{
				Clone_ShopListCanvas(i);
			}
		}
	}
	else if (m_eMainItem == CItem_Manager::MAINITEM::BATTLE)
	{
		for (size_t i = 0; i < iVecSize; i++)
		{
			if (m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::BATTLE)
			{
				Clone_ShopListCanvas(i);
			}
		}
	}
	else if (m_eMainItem == CItem_Manager::MAINITEM::WEAPON)
	{
		for (size_t i = 0; i < iVecSize; i++)
		{
			if (m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::WEAPON)
			{
				Clone_ShopListCanvas(i);
			}
		}
	}
}

void CCanvas_PurchaseKinds::Clone_ShopListCanvas(const size_t iIndex)
{
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ShopListBar.json");
	CCanvas_ShopListBar* pCanvas = dynamic_cast<CCanvas_ShopListBar*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_ShopListBar", &json));
	assert(pCanvas != nullptr && "Failed to Cloned : CCanvas_ShopListBar");

	size_t iVecSize = m_vecShopCanvass.size();
	pCanvas->Set_ShopListBar(CItem_Manager::GetInstance()->Get_ShopListPos(iVecSize), CItem_Manager::GetInstance()->Get_ShopIconPos(iVecSize), iIndex);
	m_vecShopCanvass.emplace_back(m_vecItemInfo[iIndex].first, pCanvas);
}

void CCanvas_PurchaseKinds::CurrentList()
{
	if (m_bLastCheck == true || m_bShortage == true) return;

	for (size_t i = 0; i < m_vecShopCanvass.size(); i++)
	{
		m_vecShopCanvass[i].second->SetVisible(m_bVisible);	// 굳이 for문 2번 돌리기 싫어서 Visble 도 여기서 돌려준다.

		if (true == m_vecShopCanvass[i].second->Get_OnButton())
		{
			m_vecShopCanvass[i].second->Set_OnButton();

			m_iCanvasIndex = i;
			m_iItemIndex = m_vecShopCanvass[i].second->Get_ItemInfoIndex();
			m_iItemCount = m_vecShopCanvass[m_iCanvasIndex].second->Get_NmberPurchases();

			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Shop_ItemIcon"))->Set_IconIndex(m_vecItemInfo[m_iItemIndex].second.vIconIndex);
			Find_ChildUI(L"Shop_CurrentCount")->Set_Position(m_vecShopCanvass[i].second->Get_ListPosititon());
			Find_ChildUI(L"Shop_CurrentWondowB")->Set_Position(m_vecShopCanvass[i].second->Get_ListPosititon());
			Find_ChildUI(L"Shop_LeftArrow")->Set_Position({ Find_ChildUI(L"Shop_LeftArrow")->Get_Position().x, m_vecShopCanvass[i].second->Get_ListPosititon().y });
			Find_ChildUI(L"Shop_RightArrow")->Set_Position({ Find_ChildUI(L"Shop_RightArrow")->Get_Position().x, m_vecShopCanvass[i].second->Get_ListPosititon().y });
			Find_ChildUI(L"Shop_CurrentCountButton")->Set_Position({ Find_ChildUI(L"Shop_CurrentCountButton")->Get_Position().x, m_vecShopCanvass[i].second->Get_ListPosititon().y });
		}
		else
		{
			m_vecShopCanvass[i].second->Set_OnAlpha();
		}
	}
}

void CCanvas_PurchaseKinds::Key_Input()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN) ||
		dynamic_cast<CButtonUI*>(Find_ChildUI(L"Shop_CurrentCountButton"))->Get_Input())
	{
		dynamic_cast<CButtonUI*>(Find_ChildUI(L"Shop_CurrentCountButton"))->Set_Input();
		m_iItemCount = m_vecShopCanvass[m_iCanvasIndex].second->Get_NmberPurchases();
		if (0 == m_iItemCount) return;

		// 플레이어의 소지금이 부족하면 아이템을 구매할 수 없다.
		_int iResult = static_cast<_int>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin) - (m_vecItemInfo[m_iItemIndex].second.iPrice * m_iItemCount);
		if (0 >= iResult)
		{
			m_bShortage = true;
			m_pCanvas_Shortage->SetVisible(true);
			return;
		}

		m_bLastCheck = true;
		m_pCanvas_ListCheck->SetVisible(true);
		m_pCanvas_ListCheck->Set_ItemInfo(m_iItemIndex, m_iItemCount);
	}

	if (m_bLastCheck == true || m_bShortage == true)
	{
		if (false == m_pCanvas_ListCheck->Get_Visible())
			m_bLastCheck = false;

		if (false == m_pCanvas_Shortage->Get_Visible())
			m_bShortage = false;

		return;
	}

	if (CGameInstance::GetInstance()->KeyDown(DIK_LEFT) ||
		dynamic_cast<CItem_LeftArrowUI*>(Find_ChildUI(L"Shop_LeftArrow"))->Get_KeyDown())
	{
		dynamic_cast<CItem_LeftArrowUI*>(Find_ChildUI(L"Shop_LeftArrow"))->Set_KeyDown();
		
		// 예외처리 : 왼쪽 버튼을 누를 때 구매하고자 하는 아이템 개수가 0개 이하라면 왼쪽 버튼을 누를 수 없다.
		m_iItemCount = m_vecShopCanvass[m_iCanvasIndex].second->Get_NmberPurchases();
		if (0 == m_iItemCount) return;

		--m_iItemCount;
		m_vecShopCanvass[m_iCanvasIndex].second->Set_NmberPurchases(m_iItemCount);
		dynamic_cast<CItem_LeftArrowUI*>(Find_ChildUI(L"Shop_LeftArrow"))->Set_Input();
	}

	if (CGameInstance::GetInstance()->KeyDown(DIK_RIGHT) ||
		dynamic_cast<CItem_RightArrowUI*>(Find_ChildUI(L"Shop_RightArrow"))->Get_KeyDown())
	{
		dynamic_cast<CItem_RightArrowUI*>(Find_ChildUI(L"Shop_RightArrow"))->Set_KeyDown();

		// 예외처리 : 배틀 아이템은 최대 10개, 무기 아이템은 최대 1개 이상 구매할 수 없다.
		if (m_vecItemInfo[m_iItemIndex].second.eType == CItem_Manager::MAINITEM::BATTLE)
		{
			if (10 <= m_iItemCount + m_vecItemInfo[m_iItemIndex].second.iCount)
			{
				m_iItemCount = 0; 
				m_vecShopCanvass[m_iCanvasIndex].second->Set_NmberPurchases(m_iItemCount);
				return;
			}
		}
		else 	if (m_vecItemInfo[m_iItemIndex].second.eType == CItem_Manager::MAINITEM::WEAPON)
		{
			if (1 <= m_iItemCount + m_vecItemInfo[m_iItemIndex].second.iCount)
			{
				m_iItemCount = 0;
				m_vecShopCanvass[m_iCanvasIndex].second->Set_NmberPurchases(m_iItemCount);
				return;
			}
		}

		m_iItemCount = m_vecShopCanvass[m_iCanvasIndex].second->Get_NmberPurchases();
		++m_iItemCount;
		m_vecShopCanvass[m_iCanvasIndex].second->Set_NmberPurchases(m_iItemCount);
		dynamic_cast<CItem_RightArrowUI*>(Find_ChildUI(L"Shop_RightArrow"))->Set_Input();
	}
}

CCanvas_PurchaseKinds * CCanvas_PurchaseKinds::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_PurchaseKinds*      pInstance = new CCanvas_PurchaseKinds(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_PurchaseKinds");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_PurchaseKinds::Clone(void * pArg)
{
	CCanvas_PurchaseKinds*      pInstance = new CCanvas_PurchaseKinds(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_PurchaseKinds");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_PurchaseKinds::Free()
{
	CCanvas::Free();

	m_vecShopCanvass.clear();
	m_vecItemInfo.clear();
}