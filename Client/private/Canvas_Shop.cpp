#include "stdafx.h"
#include "..\public\Canvas_Shop.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "Canvas_Main.h"
#include "Canvas_Purchase.h"
#include "Canvas_Sale.h"
#include "Main_PickUI.h"
#include "DefaultUI.h"
#include "FullUI.h"

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

	CUI_Manager::GetInstance()->Add_WindowCanvas(L"CCanvas_Shop", this);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	fill_n(m_arrCanvass, _int(SHOPCANVAS_END), nullptr);
	Add_MainCanvas();
	m_bVisible = true;

	m_szShopText = L"������ ��ǰ�� �������ּ���.";
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Purchase"))->Set_PickInitialize();	// ó���� �ش� ��ư�� ���� ���� �� �ֵ���

	return S_OK;
}

void CCanvas_Shop::Tick(_double TimeDelta)
{
	KeyInput();

	if (false == m_bShopUI) return;

	CCanvas::Tick(TimeDelta);

	Menu_Tick();
	
}

void CCanvas_Shop::Late_Tick(_double TimeDelta)
{
	if (false == m_bShopUI) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Shop::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.5f, 0.5f };
	_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// ���
	_float2 vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Purchase"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"����", vPosition + _float2(80.0f, 19.0f), 0.f, vFontSize, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Sale"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"�Ű�", vPosition + _float2(85.0f, 20.0f), 0.f, vFontSize, vColor);

	// �ϴ�
	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Shop_BasicInfo"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", m_szShopText, vPosition + _float2(170.0f, 38.0f), 0.f, vFontSize, vColor);

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
	CGameObject* pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_Purchase", &json);
	m_arrCanvass[PURCHASE] = dynamic_cast<CCanvas_Purchase*>(pCanvas);
	assert(pCanvas != nullptr && "Failed to Cloned : CCanvas_Purchase");

	/* For.Prototype_GameObject_Canvas_Sale*/
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Canvas_Sale"),
		CCanvas_Sale::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Sale.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_ShopUI", L"Canvas_Sale", &json);
	m_arrCanvass[SALE] = dynamic_cast<CCanvas_Sale*>(pCanvas);
	assert(pCanvas != nullptr && "Failed to Cloned : CCanvas_Sale");

	return S_OK;
}

void CCanvas_Shop::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_Z))
	{
		//dynamic_cast<CFullUI*>(Find_ChildUI(L"Shop_Entrance"))->Set_Alpha();

		m_bShopUI = !m_bShopUI;

		// m_bShopUI �� �ݴ�� �����Ѵ�.
		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
			iter->second->SetVisible(m_bShopUI);
		CUI_Manager::GetInstance()->Set_TempOff(m_bShopUI);

		m_arrCanvass[m_eMainCanvas]->SetVisible(m_bShopUI);

		dynamic_cast<CCanvas_Main*>(CUI_Manager::GetInstance()->Find_WindowCanvas(L"CCanvas_Main"))->Set_MainUIClose();
	}
}

void CCanvas_Shop::Menu_Tick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Purchase"))->Get_OnButton())
	{
		m_eMainCanvas = PURCHASE;
		m_szShopText = L"������ ��ǰ�� �������ּ���.";

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Purchase"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Sale"))->Set_OnAlpha();
		Find_ChildUI(L"Shop_MenuPick")->Set_Position(Find_ChildUI(L"ShopButton_Purchase")->Get_Position());

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Sale"))->Get_OnButton())
	{
		m_eMainCanvas = SALE;
		m_szShopText = L"�Ű��� ��ǰ�� �������ּ���";

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Sale"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"ShopButton_Purchase"))->Set_OnAlpha();
		Find_ChildUI(L"Shop_MenuPick")->Set_Position(Find_ChildUI(L"ShopButton_Sale")->Get_Position());

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
