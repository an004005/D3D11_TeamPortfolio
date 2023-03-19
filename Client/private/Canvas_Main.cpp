#include "stdafx.h"
#include "..\public\Canvas_Main.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "Canvas_Party.h"
#include "Canvas_MainItem.h"
#include "Canvas_Equipment.h"
#include "Canvas_BrainMap.h"

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

	return S_OK;
}

void CCanvas_Main::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	KeyInput();

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
