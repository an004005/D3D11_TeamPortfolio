#include "stdafx.h"
#include "..\public\Canvas_MainItem.h"
#include "GameInstance.h"
#include "JsonStorage.h"

#include "Canvas_MainItemAll.h"
#include "Canvas_MainItemBattle.h"
#include "Canvas_MainItemWeapon.h"
#include "Canvas_MainItemEtc.h"
#include "Main_PickUI.h"

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

	/* For.Prototype_GameObject_Canvas_MainItem*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemBattle"),
		CCanvas_MainItemBattle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemBattle.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemBattle", &json);
	m_arrCanvass[BATTLE] = dynamic_cast<CCanvas_MainItemBattle*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_Equipment */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemWeapon"),
		CCanvas_MainItemWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemWeapon.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemWeapon", &json);
	m_arrCanvass[WEAPON] = dynamic_cast<CCanvas_MainItemWeapon*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_BrainMap */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItemEtc"),
		CCanvas_MainItemEtc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItemEtc.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_MainItemUI", L"Canvas_MainItemEtc", &json);
	m_arrCanvass[ETC] = dynamic_cast<CCanvas_MainItemEtc*>(pCanvas);

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
