#include "stdafx.h"
#include "..\public\Canvas_MainItemKinds.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "Canvas_ItemWindow.h"
#include "Main_PickUI.h"

CCanvas_MainItemKinds::CCanvas_MainItemKinds(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainItemKinds::CCanvas_MainItemKinds(const CCanvas_MainItemKinds& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainItemKinds::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainItemKinds::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	FAILED_CHECK(Initialize_ItemCanvas());

	m_bVisible = false;
	m_iPickIndex = 99;
	m_iPrePickIndex = 99;

	for (vector<pair<wstring, class CCanvas_ItemWindow*>>::iterator iter = m_vecItemCanvass.begin(); iter != m_vecItemCanvass.end(); ++iter)
		iter->second->SetVisible(m_bVisible);


	return S_OK;
}

void CCanvas_MainItemKinds::Tick(_double TimeDelta)
{
	if (false == m_bVisible)
	{
		m_bBeSeen = false;

		if (false == m_bPickInitialize)
		{
			m_bPickInitialize = true;

			for (vector<pair<wstring, class CCanvas_ItemWindow*>>::iterator iter = m_vecItemCanvass.begin(); iter != m_vecItemCanvass.end(); ++iter)
				iter->second->SetVisible(false);

			m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
			for (size_t i = 0; i < m_vecItemInfo.size(); ++i)
			{
				m_vecItemInfo[i].second.bPick = false;
				CItem_Manager::GetInstance()->Set_ItemInfo(i, m_vecItemInfo[i].second);
			}
		}

		return;
	}
	else
	{
		// 다른 메뉴를 갔다 왔을 때 이전 데이터 기록을 한 번 불러오기 위한 작업
		if (false == m_bBeSeen)
		{
			m_bPickInitialize = false;
			m_bBeSeen = true;

			m_iPickIndex = 99;
			m_iPrePickIndex = 99;

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

HRESULT CCanvas_MainItemKinds::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_MainItemKinds::Imgui_RenderProperty()
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

	if (ImGui::Button("DDD"))
	{
		static _int ddd;
		++ddd;
		CItem_Manager::GetInstance()->Set_ItemCount(L"묘호 무라마사", ddd);
	}

	if (ImGui::Button("TTT"))
	{
		static _int TTT;
		++TTT;
		CItem_Manager::GetInstance()->Set_ItemCount(L"수수께끼의 텍스트 데이터", TTT);
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

void CCanvas_MainItemKinds::LoadFromJson(const Json & json)
{
	m_eMainItem = json["ITemType"];
}

HRESULT CCanvas_MainItemKinds::Initialize_ItemCanvas()
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

void CCanvas_MainItemKinds::Add_ItemCanvas(const size_t & iIndex)
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

void CCanvas_MainItemKinds::Item_Tick()
{
	// 계속 아이템의 개수를 확인해서 1개 이상 이라면 Item Canvas 를 띄우고 개수가 0개가 되었다면 지운다.
	m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();

	for (_int i = 0; i < m_vecItemInfo.size(); ++i)
	{
		if (m_eMainItem == m_vecItemInfo[i].second.eType && false == m_vecItemInfo[i].second.bIsWindow && 0 < m_vecItemInfo[i].second.iCount)
		{
			m_iItemIndex = i;
			m_vecItemInfo[m_iItemIndex].second.bIsWindow = true;
			CItem_Manager::GetInstance()->Set_ItemInfo(m_iItemIndex, m_vecItemInfo[m_iItemIndex].second);
			Add_ItemCanvas(m_iItemIndex);
			break;
		}

		if (m_eMainItem == m_vecItemInfo[i].second.eType && true == m_vecItemInfo[i].second.bIsWindow && 0 == m_vecItemInfo[i].second.iCount)
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

void CCanvas_MainItemKinds::ChildPickUI_Tick()
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

CCanvas_MainItemKinds * CCanvas_MainItemKinds::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MainItemKinds*		pInstance = new CCanvas_MainItemKinds(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainItemKinds");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MainItemKinds::Clone(void * pArg)
{
	CCanvas_MainItemKinds*		pInstance = new CCanvas_MainItemKinds(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainItemKinds");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainItemKinds::Free()
{
	CCanvas::Free();

	m_vecItemCanvass.clear();
}
