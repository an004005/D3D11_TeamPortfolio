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

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	/* For.Prototype_GameObject_Canvas_ItemWindow */
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Canvas_ItemWindow"), CCanvas_ItemWindow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_vecItemCanvass.resize(18);

	for (size_t i = 0; i < m_vecItemCanvass.size(); ++i)
		m_vecItemCanvass[i].first = L"-";





	return S_OK;
}

void CCanvas_MainItemBattle::Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	Item_Tick();
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

void CCanvas_MainItemBattle::Add_ItemCanvas(const size_t & iIndex)
{
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ItemWindow.json");
	CCanvas_ItemWindow* pCanvas = dynamic_cast<CCanvas_ItemWindow*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Lyaer_UI", L"Canvas_ItemWindow", &json));
	
	if (nullptr == pCanvas)	return;
		
	for (size_t i = 0; i < 18; ++i)
	{
		if (L"-" == m_vecItemCanvass[i].first)
		{
			pCanvas->Set_ItemWindow(CItem_Manager::GetInstance()->Get_ItemIndexPos(i), CItem_Manager::GetInstance()->Get_IconIndexPos(i), m_iItemIndex);
			m_vecItemCanvass[i].first = m_vecItemInfo[m_iItemIndex].first;
			m_vecItemCanvass[i].second = pCanvas;
			return;
		}

		if (i == m_vecItemCanvass.size())	Assert(!"Full Item Window");
	}
}

void CCanvas_MainItemBattle::Item_Tick()
{
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

			auto iter = find_if(m_vecItemCanvass.begin(), m_vecItemCanvass.end(), [&](pair<wstring, CCanvas*> pair) {
				return pair.first == m_vecItemInfo[i].first;
			});

			iter->second->SetVisible(false);
			m_vecItemCanvass.erase(iter);
		}
	}
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
