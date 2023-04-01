#include "stdafx.h"
#include "..\public\Canvas_MainItemKinds.h"
#include "GameInstance.h"
#include "JsonStorage.h"

#include "Canvas_ItemWindow.h"

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

	for (vector<pair<wstring, class CCanvas_ItemWindow*>>::iterator iter = m_vecItemCanvass.begin(); iter != m_vecItemCanvass.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	m_bVisible = false;
	m_iPickIndex = 99;
	m_iPrePickIndex = 99;

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

			for (size_t i = 0; i < m_vecItemCanvass.size(); ++i)
			{
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
	ImGui::SameLine();
	if (ImGui::Button("All Jelly"))
	{
		static _int iNum;
		++iNum;
		CItem_Manager::GetInstance()->Set_ItemCount(L"전체: 회복(소) 젤리", iNum);
	}
	ImGui::SameLine();
	if (ImGui::Button("SAS"))
	{
		static _int iSASNum;
		++iSASNum;
		CItem_Manager::GetInstance()->Set_ItemCount(L"SAS 보급수", iSASNum);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete SAS"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"SAS 보급수", 0);
	}

	if (ImGui::Button("Weapon1"))
	{
		static _int ddd;
		++ddd;
		CItem_Manager::GetInstance()->Set_ItemCount(L"묘호 무라마사", ddd);
	}
	ImGui::SameLine();
	if (ImGui::Button("Weapon2"))
	{
		static _int ttt;
		++ttt;
		CItem_Manager::GetInstance()->Set_ItemCount(L"무기 테스트", ttt);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Weapon2"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"무기 테스트", 0);
	}

	if (ImGui::Button("Etc1"))
	{
		static _int TTT;
		++TTT;
		CItem_Manager::GetInstance()->Set_ItemCount(L"수수께끼의 텍스트 데이터", TTT);
	}
	ImGui::SameLine();
	if (ImGui::Button("Etc2"))
	{
		static _int ccc;
		++ccc;
		CItem_Manager::GetInstance()->Set_ItemCount(L"기타 테스트", ccc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Ect2"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"기타 테스트", 0);
	}
}

void CCanvas_MainItemKinds::LoadFromJson(const Json & json)
{
	m_eMainItem = json["ITemType"];
}

void CCanvas_MainItemKinds::Add_ItemCanvas(const size_t & iIndex)
{
	for (size_t i = 0; i < m_vecItemCanvass.size(); ++i)
	{
		if (m_vecItemCanvass[i].first == m_vecItemInfo[iIndex].first)
			return;
	}

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ItemWindow.json");
	CCanvas_ItemWindow* pCanvas = dynamic_cast<CCanvas_ItemWindow*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Lyaer_ItemWindowUI", L"Canvas_ItemWindow", &json));
	assert(pCanvas != nullptr && "Failed to Cloned ItemWindow");

	size_t iVecSize = m_vecItemCanvass.size();
	pCanvas->Set_ItemWindow(CItem_Manager::GetInstance()->Get_ItemIndexPos(iVecSize), CItem_Manager::GetInstance()->Get_IconIndexPos(iVecSize), iIndex);
	m_vecItemCanvass.emplace_back(m_vecItemInfo[iIndex].first, pCanvas);
}

void CCanvas_MainItemKinds::Item_Tick()
{
	_bool isDelete = false;

	// 계속 아이템의 개수를 확인해서 1개 이상 이라면 Item Canvas 를 띄우고 개수가 0개가 되었다면 지운다.
	m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();

	for (_int i = 0; i < m_vecItemInfo.size(); ++i)
	{
		if (0 < m_vecItemInfo[i].second.iCount)
		{
			if (CItem_Manager::MAINITEM::ALL == m_eMainItem || m_eMainItem == m_vecItemInfo[i].second.eType)
				Add_ItemCanvas(i);
		}

		if (0 == m_vecItemInfo[i].second.iCount)
		{
			if (CItem_Manager::MAINITEM::ALL == m_eMainItem || m_eMainItem == m_vecItemInfo[i].second.eType)
			{
				auto iter = find_if(m_vecItemCanvass.begin(), m_vecItemCanvass.end(), [&](pair<wstring, CCanvas_ItemWindow*> pair) {
					return pair.first == m_vecItemInfo[i].first;
				});

				if (iter != m_vecItemCanvass.end())
				{
					isDelete = true;
					break;
				
					/*(*iter).second->SetDelete();
					m_vecItemCanvass.erase(iter);*/
				}
			}
		}
	}

	if (isDelete == true)
	{
		for (auto itr : m_vecItemCanvass)
			itr.second->SetDelete();

		m_vecItemCanvass.clear();
	}
	
}

void CCanvas_MainItemKinds::ChildPickUI_Tick()
{
	// 계속 자식 UI의 Button 을 확인해서 선택 되었는지 확인한다.
	for (size_t i = 0; i < m_vecItemCanvass.size(); ++i)
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
	for (size_t i = 0; i < m_vecItemCanvass.size(); ++i)
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
	m_vecItemInfo.clear();
}
