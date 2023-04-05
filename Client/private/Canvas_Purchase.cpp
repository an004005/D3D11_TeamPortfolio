#include "stdafx.h"
#include "..\public\Canvas_Purchase.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "Item_Manager.h"

#include "Canvas_PurchaseKinds.h"
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
	Add_ShopCanvas();

	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuAll"))->Set_PickInitialize();
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuAll"))->Set_ColorType(1);
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuBattle"))->Set_ColorType(1);
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuEtc"))->Set_ColorType(1);

	return S_OK;
}

void CCanvas_Purchase::Tick(_double TimeDelta)
{
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	if (false == m_bVisible)
	{
		m_arrCanvass[m_eShopMunu]->SetVisible(m_bVisible);
		return;
	}

	CCanvas::Tick(TimeDelta);
	MenuPcik_Tick();
}

HRESULT CCanvas_Purchase::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.45f, 0.45f };
	_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 메뉴
	_float2 vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuAll"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"모두", vPosition + _float2(77.0f, 8.0f), 0.f, vFontSize, vColor);
	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuBattle"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"배틀 아이템", vPosition + _float2(52.0f, 8.0f), 0.f, vFontSize, vColor);
	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuEtc"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"무기", vPosition + _float2(77.0f, 8.0f), 0.f, vFontSize, vColor);
	

	return S_OK;
}

void CCanvas_Purchase::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

HRESULT CCanvas_Purchase::Add_ShopCanvas()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	
	/* For.Prototype_GameObject_Canvas_PurchaseKinds*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_PurchaseKinds"), CCanvas_PurchaseKinds::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_PurchaseKinds.json");
	json["ITemType"] = CItem_Manager::MAINITEM::ALL;
	CGameObject* pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_PurchaseKinds", &json);
	assert(pCanvas != nullptr && "Failed to Cloned : Canvas_PurchaseKinds0");
	m_arrCanvass[ALL] = dynamic_cast<CCanvas_PurchaseKinds*>(pCanvas);

	json["ITemType"] = CItem_Manager::MAINITEM::BATTLE;
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_PurchaseKinds", &json);
	assert(pCanvas != nullptr && "Failed to Cloned : Canvas_PurchaseKinds1");
	m_arrCanvass[BATTLE] = dynamic_cast<CCanvas_PurchaseKinds*>(pCanvas);

	json["ITemType"] = CItem_Manager::MAINITEM::WEAPON;
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_PurchaseKinds", &json);
	assert(pCanvas != nullptr && "Failed to Cloned : Canvas_PurchaseKinds2");
	m_arrCanvass[WEAPON] = dynamic_cast<CCanvas_PurchaseKinds*>(pCanvas);

	return S_OK;
}

void CCanvas_Purchase::MenuPcik_Tick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuAll"))->Get_OnButton())
	{
		m_eShopMunu = ALL;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuAll"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuBattle"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuEtc"))->Set_OnAlpha();

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuBattle"))->Get_OnButton())
	{
		m_eShopMunu = BATTLE;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuBattle"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuAll"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuEtc"))->Set_OnAlpha();

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuEtc"))->Get_OnButton())
	{
		m_eShopMunu = WEAPON;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuEtc"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuAll"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Shop_MenuBattle"))->Set_OnAlpha();

	}
}

void CCanvas_Purchase::Canvas_Visible()
{
	for (_int i = 0; i < MAINITEM_END; ++i)
	{
		if (i == m_eShopMunu)
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
