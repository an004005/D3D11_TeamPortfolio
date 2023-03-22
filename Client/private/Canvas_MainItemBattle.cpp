#include "stdafx.h"
#include "..\public\Canvas_MainItemBattle.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "Canvas_ItemWindow.h"
#include "Main_PickUI.h"

CCanvas_MainItemBattle::CCanvas_MainItemBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainItemBattle::CCanvas_MainItemBattle(const CCanvas_MainItemBattle& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainItemBattle::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainItemBattle::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	for (vector<pair<wstring, class CCanvas_ItemWindow*>>::iterator iter = m_vecItemCanvass.begin(); iter != m_vecItemCanvass.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	/* For.Prototype_GameObject_Canvas_ItemWindow */
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Canvas_ItemWindow"), CCanvas_ItemWindow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	FAILED_CHECK(Initialize_ItemCanvas());

	return S_OK;
}

void CCanvas_MainItemBattle::Tick(_double TimeDelta)
{
	if (false == m_bVisible)
	{
		for (vector<pair<wstring, class CCanvas_ItemWindow*>>::iterator iter = m_vecItemCanvass.begin(); iter != m_vecItemCanvass.end(); ++iter)
			iter->second->SetVisible(false);

		m_bBeSeen = false;
		return;
	}
	else
	{
		if (false == m_bBeSeen)
		{
			m_bBeSeen = true;

			for (size_t i = 0; i < STORAGE; ++i)
			{
				if(L"-" != m_vecItemCanvass[i].first)
					m_vecItemCanvass[i].second->SetVisible(true);
			}
		}
	}

	CCanvas::Tick(TimeDelta);

	Item_Tick();
	ChildPickUI_Tick();
}

HRESULT CCanvas_MainItemBattle::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_MainItemBattle::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	if (ImGui::Button("Jelly"))
	{
		static _int iNum;
		++iNum;
		CItem_Manager::GetInstance()->Set_ItemCount(L"회복(소) 젤리", iNum);
	}

	if (ImGui::Button("SAS"))
	{
		static _int iSASNum;
		++iSASNum;
		CItem_Manager::GetInstance()->Set_ItemCount(L"SAS 보급수", iSASNum);
	}

	if (ImGui::Button("Delete SAS"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"SAS 보급수", 0);
	}

	if (ImGui::Button("Info"))
	{
		m_vecItemInfo[m_iItemIndex].second.bPick = true;
		CItem_Manager::GetInstance()->Set_ItemInfo(m_iItemIndex, m_vecItemInfo[m_iItemIndex].second);
	}
}

HRESULT CCanvas_MainItemBattle::Initialize_ItemCanvas()
{
	m_vecItemCanvass.resize(STORAGE);

	for (size_t i = 0; i < m_vecItemCanvass.size(); ++i)
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ItemWindow.json");
		CCanvas_ItemWindow* pCanvas = dynamic_cast<CCanvas_ItemWindow*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Lyaer_ItemWindowUI", L"Canvas_ItemWindow", &json));
		if (nullptr == pCanvas)	return E_FAIL;

		m_vecItemCanvass[i].first = L"-";
		m_vecItemCanvass[i].second = pCanvas;
	}

	return S_OK;
}

void CCanvas_MainItemBattle::Add_ItemCanvas(const size_t & iIndex)
{
	// 현재 비어있는 캔버스를 확인하고 값을 채워준다.
	for (size_t i = 0; i < STORAGE; ++i)
	{
		if (L"-" == m_vecItemCanvass[i].first)
		{
			m_vecItemCanvass[i].first = m_vecItemInfo[m_iItemIndex].first;
			m_vecItemCanvass[i].second->Set_ItemWindow(CItem_Manager::GetInstance()->Get_ItemIndexPos(i), CItem_Manager::GetInstance()->Get_IconIndexPos(i), m_iItemIndex);
			return;
		}

		if (i == STORAGE)	Assert(!"Full Item Window");
	}
}

void CCanvas_MainItemBattle::Item_Tick()
{
	// 계속 아이템의 개수를 확인해서 1개 이상 이라면 Item Canvas 를 띄우고 개수가 0개가 되었다면 지운다.
	m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();

	for (_int i = 0; i < m_vecItemInfo.size(); ++i)
	{
		if (CItem_Manager::BATTLE == m_vecItemInfo[i].second.eType && false == m_vecItemInfo[i].second.bIsWindow && 0 < m_vecItemInfo[i].second.iCount)
		{
			m_iItemIndex = i;
			m_vecItemInfo[m_iItemIndex].second.bIsWindow = true;
			CItem_Manager::GetInstance()->Set_ItemInfo(m_iItemIndex, m_vecItemInfo[m_iItemIndex].second);
			Add_ItemCanvas(m_iItemIndex);
			break;
		}

		if (CItem_Manager::BATTLE == m_vecItemInfo[i].second.eType && true == m_vecItemInfo[i].second.bIsWindow && 0 == m_vecItemInfo[i].second.iCount)
		{
			m_vecItemInfo[i].second.bIsWindow = false;
			CItem_Manager::GetInstance()->Set_ItemInfo(i, m_vecItemInfo[i].second);

			auto iter = find_if(m_vecItemCanvass.begin(), m_vecItemCanvass.end(), [&](pair<wstring, CCanvas_ItemWindow*> pair) {
				return pair.first == m_vecItemInfo[i].first;
			});

			iter->first = L"-";
			iter->second->SetVisible(false);
			//m_vecItemCanvass.erase(iter);
		}
	}
}

void CCanvas_MainItemBattle::ChildPickUI_Tick()
{
	// 계속 자식 UI의 Button 을 확인해서 선택 되었는지 확인한다.

	for (size_t i = 0; i < STORAGE; ++i)
	{
		if (true == m_vecItemCanvass[i].second->Get_OnButton())
		{
			m_iPickIndex = i;
			m_vecItemCanvass[i].second->Set_OnButton();
			break;
		}
	}

	if (m_iPrePickIndex == m_iPickIndex)	// 이전이랑 값이 다르면
		return;

	// Pick 된 Item 을 제외한 캔버스의 색을 변경한다.
	for (size_t i = 0; i < STORAGE; ++i)
	{
		if (i == m_iPickIndex)
			continue;
		else
			m_vecItemCanvass[i].second->Set_OnAlpha();
	}

	// Pick 된 Item 을 변경한다.
	for (size_t i = 0; i < m_vecItemInfo.size(); ++i)
	{
		if (m_vecItemCanvass[m_iPickIndex].first == m_vecItemInfo[i].first)
		{
			m_vecItemInfo[i].second.bPick = true;
			CItem_Manager::GetInstance()->Set_ItemInfo(i, m_vecItemInfo[i].second);
		}
		else
		{
			m_vecItemInfo[i].second.bPick = false;
			CItem_Manager::GetInstance()->Set_ItemInfo(i, m_vecItemInfo[i].second);
		}
	}

	m_iPrePickIndex = m_iPickIndex;
}

CCanvas_MainItemBattle * CCanvas_MainItemBattle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MainItemBattle*		pInstance = new CCanvas_MainItemBattle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainItemBattle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MainItemBattle::Clone(void * pArg)
{
	CCanvas_MainItemBattle*		pInstance = new CCanvas_MainItemBattle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainItemBattle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainItemBattle::Free()
{
	CCanvas::Free();

	m_vecItemCanvass.clear();
}
