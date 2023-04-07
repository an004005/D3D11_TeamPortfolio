#include "stdafx.h"
#include "..\public\Canvas_Main.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

#include "Canvas_Shop.h"
#include "Canvas_Party.h"
#include "Canvas_MainItem.h"
#include "Canvas_Equipment.h"
#include "Canvas_BrainMap.h"
#include "DefaultUI.h"
#include "Main_PickUI.h"
#include "ShaderUI.h"

CCanvas_Main::CCanvas_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Main::CCanvas_Main(const CCanvas_Main& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Main::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Main::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_WindowCanvas(L"CCanvas_Main", this);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	fill_n(m_arrCanvass, _int(MAINCANVAS_END), nullptr);
	Add_MainCanvas();
	m_bVisible = true;
	
	m_szManuText = L"파티 멤버를 변경합니다.";
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_PickInitialize();	// 처음에 해당 버튼에 불이 들어올 수 있도록

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/ShaderUI.json");
	m_pShaderUI = dynamic_cast<CShaderUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Shader_UI", &json));
	assert(m_pShaderUI != nullptr && "Failed to Clone : CFullUI");

	return S_OK;
}

void CCanvas_Main::Tick(_double TimeDelta)
{
	KeyInput();
	m_pShaderUI->Tick(TimeDelta);

	if (false == m_bMainUI)
		return;

	CCanvas::Tick(TimeDelta);

	Menu_Tick();

}

void CCanvas_Main::Late_Tick(_double TimeDelta)
{
	m_pShaderUI->Late_Tick(TimeDelta);
	if (false == m_bMainUI) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Main::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.5f, 0.5f };
	_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 상단
	_float2 vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"파티", vPosition + _float2(80.0f, 19.0f), 0.f, vFontSize, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"아이템", vPosition + _float2(75.0f, 20.0f), 0.f, vFontSize, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"장비", vPosition + _float2(80.0f, 20.0f), 0.f, vFontSize, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"브레인 맵", vPosition + _float2(62.0f, 20.0f), 0.f, vFontSize, vColor);

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Main_Menu_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(140.0f, 41.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(315.0f, 41.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(485.0f, 38.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(660.0f, 35.0f), 0.f, vFontSize, vColor);

	// 하단
	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Main_BasicInfo"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", m_szManuText, vPosition + _float2(170.0f, 38.0f), 0.f, vFontSize, vColor);

	_tchar szText[MAX_PATH] = TEXT("");
	PLAYER_STAT tPlayerStat = CPlayerInfoManager::GetInstance()->Get_PlayerStat();
	wsprintf(szText, TEXT("%d"), tPlayerStat.iBP);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(1270.0f, 34.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"BP", vPosition + _float2(1215.0f, 34.0f), 0.f, vFontSize, vColor);

	wsprintf(szText, TEXT("%d"), tPlayerStat.iCoin);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(1420.0f, 36.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"K", vPosition + _float2(1350.0f, 36.0f), 0.f, vFontSize, vColor);

	wsprintf(szText, TEXT("%f"), g_fTimeDelta_Add);
	_int iHours = static_cast<int>(g_fTimeDelta_Add / 3600); // 시
	_int iMinutes = static_cast<int>((g_fTimeDelta_Add - iHours * 3600) / 60); // 분
	_int iSeconds = static_cast<int>(g_fTimeDelta_Add - iHours * 3600 - iMinutes * 60); // 초

	if(10 > iHours)
		wsprintf(szText, TEXT("0%d :"), iHours);
	else
		wsprintf(szText, TEXT("%d :"), iHours);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(1525.0f, 39.0f), 0.f, vFontSize, vColor);
	if (10 > iMinutes)
		wsprintf(szText, TEXT("0%d :"), iMinutes);
	else
		wsprintf(szText, TEXT("%d :"), iMinutes);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(1570.0f, 39.5f), 0.f, vFontSize, vColor);
	if (10 > iSeconds)
		wsprintf(szText, TEXT("0%d"), iSeconds);
	else
		wsprintf(szText, TEXT("%d"), iSeconds);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(1610.0f, 40.5f), 0.f, vFontSize, vColor);

	return S_OK;
}

void CCanvas_Main::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

HRESULT CCanvas_Main::Add_MainCanvas()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	/* For.Prototype_GameObject_Canvas_Party */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Party"),
		CCanvas_Party::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Party.json");
	CGameObject* pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_MainUI", L"Canvas_Party", &json);
	m_arrCanvass[PARTY] = dynamic_cast<CCanvas_Party*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_MainItem*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItem"),
		CCanvas_MainItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItem.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_MainUI", L"Canvas_MainItem", &json);
	m_arrCanvass[ITEM] = dynamic_cast<CCanvas_MainItem*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_Equipment */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Equipment"),
		CCanvas_Equipment::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Equipment.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_MainUI", L"Canvas_Equipment", &json);
	m_arrCanvass[EQUIPMENT] = dynamic_cast<CCanvas_Equipment*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_BrainMap */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_BrainMap"),
		CCanvas_BrainMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_BrainMap.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Layer_MainUI", L"Canvas_BrainMap", &json);
	m_arrCanvass[BRAINMAP] = dynamic_cast<CCanvas_BrainMap*>(pCanvas);

	return S_OK;
}

void CCanvas_Main::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_ESCAPE))
	{
		m_bAlpha = true;
		m_pShaderUI->SetVisible(true);
	}

	if (false == m_bAlpha)
		return;

	m_pShaderUI->Set_Size(_float2(g_iWinSizeX, g_iWinSizeY));
	_float fAlpha = m_pShaderUI->Get_Float4s_W();
	if (m_bReverse == false && fAlpha >= 0.5f)
	{
		m_bReverse = true;
		m_bOpen = true;
	}
	else if (m_bReverse == true && fAlpha <= 0.0f)
	{
		m_bReverse = false;
		m_bAlpha = false;
		m_pShaderUI->SetVisible(false);
	}

	_float fSpeed = 0.7f;
	if (m_bReverse == false)
		fAlpha += _float(TIME_DELTA) * fSpeed;
	else
		fAlpha -= _float(TIME_DELTA) * fSpeed;
	m_pShaderUI->Set_Float4s_W(fAlpha);

	if (true == m_bOpen)
	{
		m_bOpen = false;
		m_bMainUI = !m_bMainUI;

		// m_bMainUI 와 반대로 동작한다.
		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		{
			if (L"Main_Entrance" == iter->first) continue;
			iter->second->SetVisible(m_bMainUI);
		}
		CUI_Manager::GetInstance()->Set_TempOff(m_bMainUI);

		// MainUI 들을 끄고 켜기를 한다.
		m_arrCanvass[m_eMainCanvas]->SetVisible(m_bMainUI);

		dynamic_cast<CCanvas_Shop*>(CUI_Manager::GetInstance()->Find_WindowCanvas(L"CCanvas_Shop"))->Set_ShopUIClose();
	}
}

void CCanvas_Main::Menu_Tick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Get_OnButton())
	{
		m_eMainCanvas = PARTY;
		m_szManuText = L"파티 멤버를 변경합니다.";

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Set_OnAlpha();
		Find_ChildUI(L"Main_MenuPick")->Set_Position(Find_ChildUI(L"MainButton_Party")->Get_Position());

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Get_OnButton())
	{
		m_eMainCanvas = ITEM;
		m_szManuText = L"소지 아이템을 확인합니다.";

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Set_OnAlpha();
		Find_ChildUI(L"Main_MenuPick")->Set_Position(Find_ChildUI(L"MainButton_Itme")->Get_Position());

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Get_OnButton())
	{
		m_eMainCanvas = EQUIPMENT;
		m_szManuText = L"장비를 변경합니다.";

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Set_OnAlpha();
		Find_ChildUI(L"Main_MenuPick")->Set_Position(Find_ChildUI(L"MainButton_Equipment")->Get_Position());

	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Get_OnButton())
	{
		m_eMainCanvas = BRAINMAP;
		m_szManuText = L"BP를 소비해서 스킬을 습득합니다.";

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Set_OnAlpha();

		Find_ChildUI(L"Main_MenuPick")->Set_Position(Find_ChildUI(L"MainButton_BrainMap")->Get_Position());
	}
}

void CCanvas_Main::Canvas_Visible()
{
	for (_int i = 0; i < MAINCANVAS_END; ++i)
	{
		if (i == m_eMainCanvas)
		{
			m_arrCanvass[i]->SetVisible(m_bMainUI);	// 해당 캔버스의 UI를 그린다.
			continue;
		}

		m_arrCanvass[i]->SetVisible(!m_bMainUI);	// 나머지 캔버스는 그리지 않는다.
	}
}

CCanvas_Main * CCanvas_Main::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Main*		pInstance = new CCanvas_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Main");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Main::Clone(void * pArg)
{
	CCanvas_Main*		pInstance = new CCanvas_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Main");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Main::Free()
{
	CCanvas::Free();

	Safe_Release(m_pShaderUI);
}
