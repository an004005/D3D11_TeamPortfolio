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
		CItem_Manager::GetInstance()->Set_ItemCount(L"ȸ��(��) ����", iNum);
	}

	if (ImGui::Button("SAS"))
	{
		static _int iSASNum;
		++iSASNum;
		CItem_Manager::GetInstance()->Set_ItemCount(L"SAS ���޼�", iSASNum);
	}

	if (ImGui::Button("Delete SAS"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"SAS ���޼�", 0);
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
	// ���� ����ִ� ĵ������ Ȯ���ϰ� ���� ä���ش�.
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
	// ��� �������� ������ Ȯ���ؼ� 1�� �̻� �̶�� Item Canvas �� ���� ������ 0���� �Ǿ��ٸ� �����.
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
	// ��� �ڽ� UI�� Button �� Ȯ���ؼ� ���� �Ǿ����� Ȯ���Ѵ�.

	for (size_t i = 0; i < STORAGE; ++i)
	{
		if (true == m_vecItemCanvass[i].second->Get_OnButton())
		{
			m_iPickIndex = i;
			m_vecItemCanvass[i].second->Set_OnButton();
			break;
		}
	}

	if (m_iPrePickIndex == m_iPickIndex)	// �����̶� ���� �ٸ���
		return;

	// Pick �� Item �� ������ ĵ������ ���� �����Ѵ�.
	for (size_t i = 0; i < STORAGE; ++i)
	{
		if (i == m_iPickIndex)
			continue;
		else
			m_vecItemCanvass[i].second->Set_OnAlpha();
	}

	// Pick �� Item �� �����Ѵ�.
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
