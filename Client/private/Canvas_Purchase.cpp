#include "stdafx.h"
#include "..\public\Canvas_Purchase.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "Item_Manager.h"

#include "DefaultUI.h"
#include "Main_PickUI.h"
#include "ItemIconUI.h"

CCanvas_Purchase::CCanvas_Purchase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Purchase::CCanvas_Purchase(const CCanvas_Purchase& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Purchase::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Purchase::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	fill_n(m_arrCanvass, _int(MAINITEM_END), nullptr);
	m_bVisible = false;
	Add_MainCanvas();

	return S_OK;
}

void CCanvas_Purchase::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	//for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	//	iter->second->SetVisible(m_bVisible);

	//if (false == m_bVisible)
	//	m_arrCanvass[m_eMainItem]->SetVisible(m_bVisible);

	//Menu_Tick();
}

HRESULT CCanvas_Purchase::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Purchase::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	ImGui::DragFloat("X", &m_vPosssss.x);
	ImGui::DragFloat("Y", &m_vPosssss.y);
}

HRESULT CCanvas_Purchase::Add_MainCanvas()
{
	//CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	///* For.Prototype_GameObject_Canvas_MainItemKinds*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemKinds"),
	//	CCanvas_PurchaseKinds::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemKinds.json");
	//json["ITemType"] = CItem_Manager::MAINITEM::ALL;
	//CGameObject* pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemKinds", &json);
	//m_arrCanvass[ALL] = dynamic_cast<CCanvas_PurchaseKinds*>(pCanvas);

	//json["ITemType"] = CItem_Manager::MAINITEM::BATTLE;
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemKinds", &json);
	//m_arrCanvass[BATTLE] = dynamic_cast<CCanvas_PurchaseKinds*>(pCanvas);

	//json["ITemType"] = CItem_Manager::MAINITEM::WEAPON;
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemKinds", &json);
	//m_arrCanvass[WEAPON] = dynamic_cast<CCanvas_PurchaseKinds*>(pCanvas);

	//json["ITemType"] = CItem_Manager::MAINITEM::ETC;
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemKinds", &json);
	//m_arrCanvass[ETC] = dynamic_cast<CCanvas_PurchaseKinds*>(pCanvas);

	return S_OK;
}

void CCanvas_Purchase::Menu_Tick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->Get_OnButton())
	{
		m_eMainItem = ALL;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuBattle"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuWeapon"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuEtc"))->Set_OnAlpha();

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuBattle"))->Get_OnButton())
	{
		m_eMainItem = BATTLE;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuBattle"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuWeapon"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuEtc"))->Set_OnAlpha();

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuWeapon"))->Get_OnButton())
	{
		m_eMainItem = WEAPON;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuWeapon"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuBattle"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuEtc"))->Set_OnAlpha();

	}
}

void CCanvas_Purchase::Canvas_Visible()
{
	for (_int i = 0; i < MAINITEM_END; ++i)
	{
		if (i == m_eMainItem)
		{
			m_arrCanvass[i]->SetVisible(m_bVisible);	// 해당 캔버스의 UI를 그린다.
			continue;
		}

		m_arrCanvass[i]->SetVisible(!m_bVisible);	// 나머지 캔버스는 그리지 않는다.
	}
}

CCanvas_Purchase * CCanvas_Purchase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Purchase*		pInstance = new CCanvas_Purchase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Purchase");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Purchase::Clone(void * pArg)
{
	CCanvas_Purchase*		pInstance = new CCanvas_Purchase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Purchase");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Purchase::Free()
{
	CCanvas::Free();
}
