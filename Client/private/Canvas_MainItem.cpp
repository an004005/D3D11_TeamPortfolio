#include "stdafx.h"
#include "..\public\Canvas_MainItem.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "Item_Manager.h"

#include "Canvas_MainItemAll.h"
#include "Canvas_MainItemKinds.h"
#include "DefaultUI.h"
#include "Main_PickUI.h"
#include "ItemIconUI.h"

CCanvas_MainItem::CCanvas_MainItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainItem::CCanvas_MainItem(const CCanvas_MainItem& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainItem::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainItem::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	fill_n(m_arrCanvass, _int(MAINITEM_END), nullptr);
	m_bVisible = false;
	Add_MainCanvas();

	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->Set_InitializeAlpha();
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->Set_ColorType(1);
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuBattle"))->Set_ColorType(1);
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuWeapon"))->Set_ColorType(1);
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuEtc"))->Set_ColorType(1);

	return S_OK;
}

void CCanvas_MainItem::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	if (false == m_bVisible)
		m_arrCanvass[m_eMainItem]->SetVisible(m_bVisible);

	Menu_Tick();
}

HRESULT CCanvas_MainItem::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.45f, 0.45f };
	_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// ------- 메뉴
	_float2 vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->GetScreenSpaceLeftTop();
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->Get_OnAlpha())
		vColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	else
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	pGameInstance->Render_Font(L"Pretendard32", L"모두", vPosition + _float2(78.0f, 7.0f), 0.f, vFontSize, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuBattle"))->GetScreenSpaceLeftTop();
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuBattle"))->Get_OnAlpha())
		vColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	else
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	pGameInstance->Render_Font(L"Pretendard32", L"배틀 아이템", vPosition + _float2(55.0f, 8.0f), 0.f, vFontSize, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuWeapon"))->GetScreenSpaceLeftTop();
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuWeapon"))->Get_OnAlpha())
		vColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	else
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	pGameInstance->Render_Font(L"Pretendard32", L"무기", vPosition + _float2(79.0f, 8.0f), 0.f, vFontSize, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuEtc"))->GetScreenSpaceLeftTop();
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuEtc"))->Get_OnAlpha())
		vColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	else
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	pGameInstance->Render_Font(L"Pretendard32", L"기타", vPosition + _float2(79.0f, 8.0f), 0.f, vFontSize, vColor);
	vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	// ------- 아이템 상세 설명

	// 아이템 이름
	vector<pair<wstring, CItem_Manager::ITEMINFO>> ItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();

	for (size_t i = 0; i < ItemInfo.size(); ++i)
	{
		if (false == ItemInfo[i].second.bPick)
			continue;

		// 아이콘 하나만 좀 변경하겠슴다...
		dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_Icon"))->Set_IconIndex(ItemInfo[i].second.vIconIndex);

		vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Item_ItemInfo"))->GetScreenSpaceLeftTop();
		pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].first.c_str(), vPosition + _float2(215.0f, 35.0f), 0.f, { 0.6f, 0.6f }, vColor);

		// 아이템 설명
		pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[0], vPosition + _float2(743.0f, 39.0f), 0.f, vFontSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[1], vPosition + _float2(743.0f, 69.0f), 0.f, vFontSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[2], vPosition + _float2(743.0f, 99.0f), 0.f, vFontSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[3], vPosition + _float2(743.0f, 129.0f), 0.f, vFontSize, vColor);

		// 소지 수
		_tchar szText[MAX_PATH] = TEXT("");
		pGameInstance->Render_Font(L"Pretendard32", L"소지 수", vPosition + _float2(560.0f, 77.0f), 0.f, vFontSize, vColor);
		if (ItemInfo[i].second.iCount == ItemInfo[i].second.iMaxCount)
			vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
		else
			vColor = { 0.752f, 0.752f, 0.596f, 1.0f }; 
		wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iCount);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(632.0f, 77.0f), 0.f, vFontSize, vColor);
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
		wsprintf(szText, TEXT("/ %d"), ItemInfo[i].second.iMaxCount);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(662.0f, 77.0f), 0.f, vFontSize, vColor);
	}

	return S_OK;
}

void CCanvas_MainItem::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	ImGui::DragFloat("X", &m_vPosssss.x);
	ImGui::DragFloat("Y", &m_vPosssss.y);
}

void CCanvas_MainItem::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_MainItem::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

HRESULT CCanvas_MainItem::Add_MainCanvas()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	/* For.Prototype_GameObject_Canvas_Party */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemAll"),
		CCanvas_MainItemAll::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemAll.json");
	CGameObject* pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemAll", &json);
	m_arrCanvass[ALL] = dynamic_cast<CCanvas_MainItemAll*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_MainItemKinds*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemKinds"),
		CCanvas_MainItemKinds::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json["ITemType"] = CItem_Manager::MAINITEM::BATTLE;
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemKinds", &json);
	m_arrCanvass[BATTLE] = dynamic_cast<CCanvas_MainItemKinds*>(pCanvas);

	json["ITemType"] = CItem_Manager::MAINITEM::WEAPON;
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemKinds", &json);
	m_arrCanvass[WEAPON] = dynamic_cast<CCanvas_MainItemKinds*>(pCanvas);

	json["ITemType"] = CItem_Manager::MAINITEM::ETC;
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemKinds", &json);
	m_arrCanvass[ETC] = dynamic_cast<CCanvas_MainItemKinds*>(pCanvas);




	///* For.Prototype_GameObject_Canvas_Party */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemAll"),
	//	CCanvas_MainItemAll::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemAll.json");
	//CGameObject* pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemAll", &json);
	//m_arrCanvass[ALL] = dynamic_cast<CCanvas_MainItemAll*>(pCanvas);

	///* For.Prototype_GameObject_Canvas_MainItemKinds*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemBattle"),
	//	CCanvas_MainItemBattle::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemBattle.json");
	//json["ITemType"] = CItem_Manager::MAINITEM::ALL;
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemBattle", &json);
	//m_arrCanvass[BATTLE] = dynamic_cast<CCanvas_MainItemBattle*>(pCanvas);

	///* For.Prototype_GameObject_Canvas_Equipment */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemWeapon"),
	//	CCanvas_MainItemWeapon::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemWeapon.json");
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemWeapon", &json);
	//m_arrCanvass[WEAPON] = dynamic_cast<CCanvas_MainItemWeapon*>(pCanvas);

	///* For.Prototype_GameObject_Canvas_BrainMap */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemEtc"),
	//	CCanvas_MainItemEtc::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemEtc.json");
	//pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemEtc", &json);
	//m_arrCanvass[ETC] = dynamic_cast<CCanvas_MainItemEtc*>(pCanvas);

	return S_OK;
}

void CCanvas_MainItem::Menu_Tick()
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

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuEtc"))->Get_OnButton())
	{
		m_eMainItem = ETC;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuEtc"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuAll"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuBattle"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Item_MenuWeapon"))->Set_OnAlpha();

	}
}

void CCanvas_MainItem::Canvas_Visible()
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

void CCanvas_MainItem::PickInfo_Tick()
{
}

CCanvas_MainItem * CCanvas_MainItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MainItem*		pInstance = new CCanvas_MainItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainItem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MainItem::Clone(void * pArg)
{
	CCanvas_MainItem*		pInstance = new CCanvas_MainItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainItem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainItem::Free()
{
	CCanvas::Free();
}
