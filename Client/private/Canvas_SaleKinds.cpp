#include "stdafx.h"
#include "..\public\Canvas_SaleKinds.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "Item_Manager.h"
#include "PlayerInfoManager.h"
#include "UI_Manager.h"

#include "Canvas_SaleListCheck.h"
#include "Canvas_ShopListBar.h"
#include "Main_PickUI.h"
#include "DefaultUI.h"
#include "ItemIconUI.h"
#include "Item_LeftArrowUI.h"
#include "Item_RightArrowUI.h"
#include "ButtonUI.h"

CCanvas_SaleKinds::CCanvas_SaleKinds(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SaleKinds::CCanvas_SaleKinds(const CCanvas_SaleKinds& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SaleKinds::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SaleKinds::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	Add_ShopListCanvas();
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SaleListCheck.json");
	m_pCanvas_SaleListCheck = dynamic_cast<CCanvas_SaleListCheck*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_SaleListCheck", &json));
	assert(m_pCanvas_SaleListCheck != nullptr && "Failed to Cloned : CCanvas_SaleListCheck");

	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_CurrentWondowB"))->Set_PickInitialize();
	dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Shop_ItemIcon"))->Set_IconIndex(m_vecItemInfo[m_iItemIndex].second.vIconIndex);

	if (!m_vecShopCanvass.empty())	// 벡터가 비어있지 않은 경우에만 Current 를 배치한다.
	{
		Find_ChildUI(L"Shop_CurrentCount")->Set_Position(m_vecShopCanvass[0].second->Get_ListPosititon());
		Find_ChildUI(L"Shop_CurrentWondowB")->Set_Position(m_vecShopCanvass[0].second->Get_ListPosititon());
		Find_ChildUI(L"Shop_LeftArrow")->Set_Position({ Find_ChildUI(L"Shop_LeftArrow")->Get_Position().x, m_vecShopCanvass[0].second->Get_ListPosititon().y });
		Find_ChildUI(L"Shop_RightArrow")->Set_Position({ Find_ChildUI(L"Shop_RightArrow")->Get_Position().x, m_vecShopCanvass[0].second->Get_ListPosititon().y });
		Find_ChildUI(L"Shop_CurrentCountButton")->Set_Position({ Find_ChildUI(L"Shop_CurrentCountButton")->Get_Position().x, m_vecShopCanvass[0].second->Get_ListPosititon().y });

		dynamic_cast<CItem_LeftArrowUI*>(Find_ChildUI(L"Shop_LeftArrow"))->Set_MouseInput();
		dynamic_cast<CItem_RightArrowUI*>(Find_ChildUI(L"Shop_RightArrow"))->Set_MouseInput();
	}
	else
	{
		// 비어있는 경우 보여주지 않는다.
		CurrentVisible(false);
	}

	return S_OK;
}

void CCanvas_SaleKinds::Tick(_double TimeDelta)
{
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		//if (m_bVisible == true)
		//{
		//	if (iter->first == L"Shop_CurrentCount" ||
		//		iter->first == L"Shop_CurrentWondowB" ||
		//		iter->first == L"Shop_LeftArrow" ||
		//		iter->first == L"Shop_RightArrow")
		//		continue;
		//}

		iter->second->SetVisible(m_bVisible);
	}

	for (vector<pair<wstring, CCanvas_ShopListBar*>>::iterator iter = m_vecShopCanvass.begin(); iter != m_vecShopCanvass.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	if (m_bVisible == false) 
	{
		m_bCanvasReset = false;
		return;
	}
	else
	{
		if (m_bCanvasReset == false)
		{
			m_bCanvasReset = true;
			Add_ShopListCanvas();
		}
	}

	CCanvas::Tick(TimeDelta);

	m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	CurrentList();
	Key_Input();
}

void CCanvas_SaleKinds::Late_Tick(_double TimeDelta)
{
	if (m_bVisible == false) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_SaleKinds::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontBigSize = { 0.5f, 0.5f };
	_float2 vFontSmaillSize = { 0.45f, 0.45f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_float2 vPosition = { 0.0f, 0.0f };

	// 오른쪽 상단 아이템 설명
	vPosition = Find_ChildUI(L"Shop_ItemInfo")->GetScreenSpaceLeftTop();
	if (!m_vecShopCanvass.empty())
	{
		pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].first.c_str(), vPosition + _float2(60.0f, 10.0f), 0.f, vFontBigSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].second.szItemEx[0], vPosition + _float2(-26.0f, 100.0f), 0.f, vFontBigSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].second.szItemEx[1], vPosition + _float2(-26.0f, 130.0f), 0.f, vFontBigSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].second.szItemEx[2], vPosition + _float2(-26.0f, 160.0f), 0.f, vFontBigSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", m_vecItemInfo[m_iItemIndex].second.szItemEx[3], vPosition + _float2(-26.0f, 190.0f), 0.f, vFontBigSize, vColor);

		pGameInstance->Render_Font(L"Pretendard32", L"소지수", vPosition + _float2(530.0f, 50.0f), 0.f, vFontSmaillSize, vColor);

		_uint iItemCount = m_vecItemInfo[m_iItemIndex].second.iCount;
		if (iItemCount >= 10)
			vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
		else
			vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		wsprintf(szText, TEXT("%u"), iItemCount);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(606.0f, 50.0f), 0.f, vFontSmaillSize, vColor);

		vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (m_vecItemInfo[m_iItemIndex].second.eType == CItem_Manager::MAINITEM::BATTLE)
			wsprintf(szText, TEXT("/  %d"), 10);
		else
			wsprintf(szText, TEXT("/  %d"), 1);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(630.0f, 50.0f), 0.f, vFontSmaillSize, vColor);
	}
	else
	{
		pGameInstance->Render_Font(L"Pretendard32", L"매각 가능한 아이템이 없습니다.", vPosition + _float2(110.0f, 30.0f), 0.f, vFontBigSize, vColor);
	}

	// 오른쪽 하단 매각 내역
	vPosition = Find_ChildUI(L"Shop_Payment")->GetScreenSpaceLeftTop();

	pGameInstance->Render_Font(L"Pretendard32", L"소지금", vPosition + _float2(-30.0f, 5.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%u"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(390.0f, 5.0f), 0.f, vFontSmaillSize, vColor);

	pGameInstance->Render_Font(L"Pretendard32", L"매각 후 소지금", vPosition + _float2(-30.0f, 41.0f), 0.f, vFontSmaillSize, vColor);
	_int iResult = static_cast<_int>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin) + (m_vecItemInfo[m_iItemIndex].second.iSellingPrice * m_iItemCount);
	if (0 >= iResult)
		vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
	else
		vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	wsprintf(szText, TEXT("%d"), iResult);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(390.0f, 41.0f), 0.f, vFontSmaillSize, vColor);

	vColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	_uint iItemValueResult = m_vecItemInfo[m_iItemIndex].second.iSellingPrice * m_iItemCount;
	pGameInstance->Render_Font(L"Pretendard32", L"매각 금액", vPosition + _float2(-30.0f, 85.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), iItemValueResult);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(390.0f, 85.0f), 0.f, vFontSmaillSize, vColor);

	return S_OK;
}

void CCanvas_SaleKinds::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_SaleKinds::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

	m_eMainItem = json["ITemType"];
}

void CCanvas_SaleKinds::Add_ShopListCanvas()
{
	m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	size_t iVecSize = m_vecItemInfo.size();

	if (m_eMainItem == CItem_Manager::MAINITEM::ALL)
	{
		// ALL 아이템 즉, BATTAL 과 WEAPON 의 아이템이 "1개" 이상으로 있는 것을 추가한다.
		for (size_t i = 0; i < iVecSize; i++)
		{
			if (m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::BATTLE || m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::WEAPON)
			{
				if(1 <= m_vecItemInfo[i].second.iCount)
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
				if (1 <= m_vecItemInfo[i].second.iCount)
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
				if (1 <= m_vecItemInfo[i].second.iCount)
					Clone_ShopListCanvas(i);
			}
		}
	}
}

void CCanvas_SaleKinds::Clone_ShopListCanvas(const size_t iIndex)
{
	// 단' 이미 추가되어 있는 List 라면 추가하지 않는다.
	for (size_t i = 0; i < m_vecShopCanvass.size(); i++)
	{
		if (m_vecShopCanvass[i].first == m_vecItemInfo[iIndex].first)
			return;
	}

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ShopListBar.json");
	CCanvas_ShopListBar* pCanvas = dynamic_cast<CCanvas_ShopListBar*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_ShopListBar", &json));
	assert(pCanvas != nullptr && "Failed to Cloned : CCanvas_ShopListBar");

	size_t iVecSize = m_vecShopCanvass.size();
	pCanvas->Set_Sale();
	pCanvas->Set_ShopListBar(CItem_Manager::GetInstance()->Get_ShopListPos(iVecSize), CItem_Manager::GetInstance()->Get_ShopIconPos(iVecSize), iIndex);
	m_vecShopCanvass.emplace_back(m_vecItemInfo[iIndex].first, pCanvas);
}

void CCanvas_SaleKinds::CurrentList()
{
	if (m_bLastCheck == true)
	{
		if (4 < m_vecShopCanvass.size())
		{
			m_vecShopCanvass[3].second->Set_OnButton();
			m_vecShopCanvass[4].second->Set_OnButton();
		}
		return;
	}

	if (m_vecShopCanvass.empty())
	{
		CurrentVisible(false);
		return;
	}

	for (size_t i = 0; i < m_vecShopCanvass.size(); i++)
	{
		// 1 : 현재 소지중이 아이템을 출력한다.
		m_vecShopCanvass[i].second->SetVisible(m_bVisible);	

		// 2 : 현재 소지중인 아이템 중 버튼을 눌렀는지 확인한다.
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

			CurrentVisible(true);
		}
		else
		{
			// 버튼을 누르고 있지 않은 인덱스는 알파값을 조절하지 않도록 한다.
			m_vecShopCanvass[i].second->Set_OnAlpha();
		}
	
		// 3 : 현재 소지중인 아이템 중 모두 판매를 해서 개수가 0개가 되어버리면 목록에서 지워준다.
		if (0 == m_vecShopCanvass[i].second->Get_ItemCount())
		{
			for (size_t j = 0; j < m_vecShopCanvass.size(); j++)
				m_vecShopCanvass[j].second->SetDelete();	// 정렬을 위해서 벡터에 있던 객체들을 모두 삭제하고
			
			m_vecShopCanvass.clear();								// 벡터에 있는 데이터도 비우고
			Add_ShopListCanvas();									// 다시 추가한다.

			CurrentVisible(false);
		}
	}
}

void CCanvas_SaleKinds::Key_Input()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN) ||
		dynamic_cast<CButtonUI*>(Find_ChildUI(L"Shop_CurrentCountButton"))->Get_Input())
	{
		dynamic_cast<CButtonUI*>(Find_ChildUI(L"Shop_CurrentCountButton"))->Set_Input();
		m_iItemCount = m_vecShopCanvass[m_iCanvasIndex].second->Get_NmberPurchases();
		if (0 == m_iItemCount) return;

		m_bLastCheck = true;
		m_pCanvas_SaleListCheck->SetVisible(true);
		m_pCanvas_SaleListCheck->Set_ItemInfo(m_iItemIndex, m_iItemCount);
	}

	if (m_bLastCheck == true)
	{
		if (false == m_pCanvas_SaleListCheck->Get_Visible())
			m_bLastCheck = false;

		return;
	}

	if (m_vecShopCanvass.empty()) return;

	if (CGameInstance::GetInstance()->KeyDown(DIK_LEFT) ||
		dynamic_cast<CItem_LeftArrowUI*>(Find_ChildUI(L"Shop_LeftArrow"))->Get_KeyDown())
	{
		dynamic_cast<CItem_LeftArrowUI*>(Find_ChildUI(L"Shop_LeftArrow"))->Set_KeyDown();

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

		// 예외처리 : 현재 가지고 있는 아이템 개수 보다 많은 개수를 판매할 수 없다.
		if (m_iItemCount >= m_vecItemInfo[m_iItemIndex].second.iCount)
		{
			m_iItemCount = 0;
			m_vecShopCanvass[m_iCanvasIndex].second->Set_NmberPurchases(m_iItemCount);
			return;
		}

		m_iItemCount = m_vecShopCanvass[m_iCanvasIndex].second->Get_NmberPurchases();
		++m_iItemCount;
		m_vecShopCanvass[m_iCanvasIndex].second->Set_NmberPurchases(m_iItemCount);
		dynamic_cast<CItem_RightArrowUI*>(Find_ChildUI(L"Shop_RightArrow"))->Set_Input();
	}
}

void CCanvas_SaleKinds::CurrentVisible(const _bool bVisible)
{
	Find_ChildUI(L"Shop_CurrentCountButton")->SetVisible(bVisible);
	Find_ChildUI(L"Shop_ItemIcon")->SetVisible(bVisible);
	Find_ChildUI(L"Shop_CurrentCount")->SetVisible(bVisible);
	Find_ChildUI(L"Shop_CurrentWondowB")->SetVisible(bVisible);
	Find_ChildUI(L"Shop_LeftArrow")->SetVisible(bVisible);
	Find_ChildUI(L"Shop_RightArrow")->SetVisible(bVisible);
}

CCanvas_SaleKinds * CCanvas_SaleKinds::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SaleKinds*      pInstance = new CCanvas_SaleKinds(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SaleKinds");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SaleKinds::Clone(void * pArg)
{
	CCanvas_SaleKinds*      pInstance = new CCanvas_SaleKinds(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SaleKinds");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SaleKinds::Free()
{
	CCanvas::Free();

	m_vecShopCanvass.clear();
	m_vecItemInfo.clear();
}