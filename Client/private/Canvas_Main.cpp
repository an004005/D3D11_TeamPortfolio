#include "stdafx.h"
#include "..\public\Canvas_Main.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "Canvas_Party.h"
#include "Canvas_MainItem.h"
#include "Canvas_Equipment.h"
#include "Canvas_BrainMap.h"
#include "DefaultUI.h"
#include "Main_PickUI.h"

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

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	fill_n(m_arrCanvass, _int(MAINCANVAS_END), nullptr);
	Add_MainCanvas();
	m_bVisible = true;

	return S_OK;
}

void CCanvas_Main::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	KeyInput();
	Menu_Tick();

	if (false == m_bMainUI) return;

	m_arrCanvass[m_eMainCanvas]->Tick(TimeDelta);
}

void CCanvas_Main::Late_Tick(_double TimeDelta)
{
	if (false == m_bMainUI) return;

	CCanvas::Late_Tick(TimeDelta);

	m_arrCanvass[m_eMainCanvas]->Late_Tick(TimeDelta);

}

HRESULT CCanvas_Main::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float fFontSize = 0.5f;
	_float4 vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float2 vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"파티", vPosition + _float2(80.0f, 21.0f), 0.f, { fFontSize, fFontSize }, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"아이템", vPosition + _float2(75.0f, 22.0f), 0.f, { fFontSize, fFontSize }, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"장비", vPosition + _float2(80.0f, 22.0f), 0.f, { fFontSize, fFontSize }, vColor);

	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"브레인 맵", vPosition + _float2(62.0f, 24.0f), 0.f, { fFontSize, fFontSize }, vColor);

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Main_Menu_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(140.0f, 41.0f), 0.f, { fFontSize, fFontSize }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(315.0f, 41.0f), 0.f, { fFontSize, fFontSize }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(485.0f, 38.0f), 0.f, { fFontSize, fFontSize }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(660.0f, 35.0f), 0.f, { fFontSize, fFontSize }, vColor);

	return S_OK;
}

void CCanvas_Main::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_Main::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Main::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

HRESULT CCanvas_Main::Add_MainCanvas()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	/* For.Prototype_GameObject_Canvas_Party */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Party"),
		CCanvas_Party::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Party.json");
	CGameObject* pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_UI", L"Canvas_Party", &json);
	m_arrCanvass[PARTY] = dynamic_cast<CCanvas_Party*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_MainItem*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_MainItem"),
		CCanvas_MainItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainItem.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_UI", L"Canvas_MainItem", &json);
	m_arrCanvass[ITEM] = dynamic_cast<CCanvas_MainItem*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_Equipment */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Equipment"),
		CCanvas_Equipment::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Equipment.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_UI", L"Canvas_Equipment", &json);
	m_arrCanvass[EQUIPMENT] = dynamic_cast<CCanvas_Equipment*>(pCanvas);

	/* For.Prototype_GameObject_Canvas_BrainMap */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_BrainMap"),
		CCanvas_BrainMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_BrainMap.json");
	pCanvas = pGameInstance->Clone_GameObject_Get(L"Lyaer_UI", L"Canvas_BrainMap", &json);
	m_arrCanvass[BRAINMAP] = dynamic_cast<CCanvas_BrainMap*>(pCanvas);

	return S_OK;
}

void CCanvas_Main::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_ESCAPE))
	{
		m_bMainUI = !m_bMainUI;

		// m_bMainUI 와 반대로 동작한다.
		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
			iter->second->SetVisible(m_bMainUI);
		CUI_Manager::GetInstance()->Set_TempOff(m_bMainUI);

		// MainUI 들을 끄고 켜기를 한다.
		m_arrCanvass[m_eMainCanvas]->SetVisible(m_bMainUI);
	}
}

void CCanvas_Main::Menu_Tick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Get_OnButton())
	{
		m_eMainCanvas = PARTY;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Party"))->Set_OnButton();
	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Get_OnButton())
	{
		m_eMainCanvas = ITEM;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Itme"))->Set_OnButton();
	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Get_OnButton())
	{
		m_eMainCanvas = EQUIPMENT;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_Equipment"))->Set_OnButton();
	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Get_OnButton())
	{
		m_eMainCanvas = BRAINMAP;

		Canvas_Visible();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"MainButton_BrainMap"))->Set_OnButton();
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
}
