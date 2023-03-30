#include "stdafx.h"
#include "..\public\Canvas_Shop.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "Canvas_Purchase.h"
#include "Main_PickUI.h"
#include "DefaultUI.h"

CCanvas_Shop::CCanvas_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Shop::CCanvas_Shop(const CCanvas_Shop& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Shop::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Shop::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(true);

	fill_n(m_arrCanvass, _int(SHOPCANVAS_END), nullptr);
	Add_MainCanvas();
	m_bVisible = true;
	//dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_InitializeAlpha();	// ó���� �ش� ��ư�� ���� ���� �� �ֵ���

	return S_OK;
}

void CCanvas_Shop::Tick(_double TimeDelta)
{
	//KeyInput();

	//if (false == m_bShopUI) return;

	CCanvas::Tick(TimeDelta);

	//Menu_Tick();

}

void CCanvas_Shop::Late_Tick(_double TimeDelta)
{
	//if (false == m_bShopUI) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Shop::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Shop::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

HRESULT CCanvas_Shop::Add_MainCanvas()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	/* For.Prototype_GameObject_Canvas_Purchase*/
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Canvas_Purchase"),
		CCanvas_Purchase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Purchase.json");
	CGameObject* pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_ShopPurchase", L"Canvas_Purchase", &json);
	m_arrCanvass[PURCHASE] = dynamic_cast<CCanvas_Purchase*>(pCanvas);

	///* For.Prototype_GameObject_Canvas_MainItem*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItem"),
	//	CCanvas_ShopItem::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItem.json");
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_MainItemUI", L"Canvas_MainItem", &json);
	//m_arrCanvass[ITEM] = dynamic_cast<CCanvas_ShopItem*>(pCanvas);

	///* For.Prototype_GameObject_Canvas_Equipment */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Equipment"),
	//	CCanvas_Equipment::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Equipment.json");
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_MainItemUI", L"Canvas_Equipment", &json);
	//m_arrCanvass[EQUIPMENT] = dynamic_cast<CCanvas_Equipment*>(pCanvas);

	///* For.Prototype_GameObject_Canvas_BrainMap */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_BrainMap"),
	//	CCanvas_BrainMap::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_BrainMap.json");
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_MainItemUI", L"Canvas_BrainMap", &json);
	//m_arrCanvass[BRAINMAP] = dynamic_cast<CCanvas_BrainMap*>(pCanvas);

	return S_OK;
}

void CCanvas_Shop::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_ESCAPE))
	{
		m_bShopUI = !m_bShopUI;

		// m_bShopUI �� �ݴ�� �����Ѵ�.
		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
			iter->second->SetVisible(m_bShopUI);
		CUI_Manager::GetInstance()->Set_TempOff(m_bShopUI);

		// MainUI ���� ���� �ѱ⸦ �Ѵ�.
		m_arrCanvass[m_eMainCanvas]->SetVisible(m_bShopUI);
	}
}

void CCanvas_Shop::Menu_Tick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Get_OnButton())
	{
		m_eMainCanvas = PURCHASE;
		m_szShopText = L"������ ��ǰ�� �������ּ���.";

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Set_OnAlpha();
		Find_ChildUI(L"Main_MenuPick")->Set_Position(Find_ChildUI(L"MainButton_Party")->Get_Position());

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Get_OnButton())
	{
		m_eMainCanvas = SALE;
		m_szShopText = L"�Ű��� ��ǰ�� �������ּ���";

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Set_OnAlpha();
		Find_ChildUI(L"Main_MenuPick")->Set_Position(Find_ChildUI(L"MainButton_Itme")->Get_Position());

	}
}

void CCanvas_Shop::Canvas_Visible()
{
	for (_int i = 0; i < SHOPCANVAS_END; ++i)
	{
		if (i == m_eMainCanvas)
		{
			m_arrCanvass[i]->SetVisible(m_bShopUI);	// �ش� ĵ������ UI�� �׸���.
			continue;
		}

		m_arrCanvass[i]->SetVisible(!m_bShopUI);	// ������ ĵ������ �׸��� �ʴ´�.
	}
}

CCanvas_Shop * CCanvas_Shop::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Shop*		pInstance = new CCanvas_Shop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Shop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Shop::Clone(void * pArg)
{
	CCanvas_Shop*		pInstance = new CCanvas_Shop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Shop");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Shop::Free()
{
	CCanvas::Free();
}
