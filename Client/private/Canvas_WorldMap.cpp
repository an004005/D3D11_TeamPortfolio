#include "stdafx.h"
#include "..\public\Canvas_WorldMap.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"
#include "GameManager.h"
#include "Level_Loading_Simple.h"
#include "Level_Tutorial.h"
#include "Level_ConstructionSite3F.h"
#include "Level_ConstructionSite2F.h"
#include "Level_Subway.h"
#include "Level_NaomiRoom.h"
#include "Level_Hospital_1F.h"
#include "Level_FinalStage.h"

#include "Canvas_MapBar.h"
#include "ShaderUI.h"

CCanvas_WorldMap::CCanvas_WorldMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_WorldMap::CCanvas_WorldMap(const CCanvas_WorldMap& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_WorldMap::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_WorldMap::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/ShaderUI.json");
	m_pShaderUI = dynamic_cast<CShaderUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Shader_UI", &json));
	assert(m_pShaderUI != nullptr && "Failed to Clone : CFullUI");

	arrMapChake.fill(false);
	m_bVisible = true;
	CloneBar();

	return S_OK;
}

void CCanvas_WorldMap::Tick(_double TimeDelta)
{
	KeyInput();

	if (false == m_bMapUI) return;

	CCanvas::Tick(TimeDelta);

	CloneBar();
	MapBar_Tick();
}

void CCanvas_WorldMap::Late_Tick(_double TimeDelta)
{
	if (false == m_bMapUI) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_WorldMap::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;
	
	_float2 vSize = { 0.5f, 0.5f };
	_tchar szText[MAX_PATH] = TEXT("");
	
	_float2 vPosition = Find_ChildUI(L"Menu")->GetScreenSpaceLeftTop();
	wsprintf(szText, TEXT("%s에서 탈출한다."), m_wsName.c_str());
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(112.0f, -46.0f), 0.f, vSize, {1.0f, 0.99f, 0.87f, 1.0f});
	vPosition = Find_ChildUI(L"BasicInfo")->GetScreenSpaceLeftTop();
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"목적지를 선택해주세요.", vPosition + _float2(183.0f, 39.0f), 0.f, vSize, { 1.0f, 0.99f, 0.87f, 1.0f });

	return S_OK;
}

void CCanvas_WorldMap::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _int iTemp = 0;
	ImGui::InputInt("Input", &iTemp);
	if(ImGui::Button("Set Map bool"))
	{
		CGameManager::GetInstance()->Set_Map(iTemp);
	}
}

void CCanvas_WorldMap::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_WorldMap::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_WorldMap::CloneBar()
{
	array<_bool, 6> arrMap = CGameManager::GetInstance()->Get_Map();
	
	for (size_t i = 0; i < arrMap.size(); i++)
	{
		if (false == arrMap[i] || true == arrMapChake[i]) continue;
		arrMapChake[i] = true;

		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MapBar.json");
		CCanvas_MapBar* pCanvasMapBar = dynamic_cast<CCanvas_MapBar*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_MapBar", &json));
		assert(pCanvasMapBar != nullptr && "Failed to Cloned : CCanvas_MapBar");

		if (0 == i)
		{
			pCanvasMapBar->Set_Name(L"대로");
			pCanvasMapBar->Set_PosY(250.0f);
		}
		else if (1 == i)
		{
			pCanvasMapBar->Set_Name(L"스오-류신구");
			pCanvasMapBar->Set_PosY(190.0f);
		}
		else if (2 == i)
		{
			pCanvasMapBar->Set_Name(L"키쿠치바 3층");
			pCanvasMapBar->Set_PosY(130.0f);
		}
		else if (3 == i)
		{
			pCanvasMapBar->Set_Name(L"키쿠치바 2층");
			pCanvasMapBar->Set_PosY(70.0f);
		}
		else if (4 == i)
		{
			pCanvasMapBar->Set_Name(L"폐지하선로 스오 9호선");
			pCanvasMapBar->Set_PosY(10.0f);
		}
		else if (5 == i)
		{
			pCanvasMapBar->Set_Name(L"구 토벌군병원");
			pCanvasMapBar->Set_PosY(-50.0f);
		}
		else
			pCanvasMapBar->SetDelete();

		pCanvasMapBar->SetVisible(m_bMapUI);
		vecMapBar.push_back(pCanvasMapBar);
	}
}

void CCanvas_WorldMap::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_TAB))
	{
		m_bAlpha = true;
		m_pShaderUI->SetVisible(true);
	}

	if (false == m_bAlpha) return;

	m_pShaderUI->Set_Size({ _float(g_iWinSizeX), _float(g_iWinSizeY) });
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
		m_bMapUI = !m_bMapUI;

		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
			iter->second->SetVisible(m_bMapUI);

		CUI_Manager::GetInstance()->Set_TempOff(m_bMapUI);
	}
}

void CCanvas_WorldMap::MapBar_Tick()
{
	if (vecMapBar.empty()) return;

	for (size_t i = 0; i < vecMapBar.size(); i++)
	{
		vecMapBar[i]->SetVisible(m_bMapUI);

		if (true == dynamic_cast<CCanvas_MapBar*>(vecMapBar[i])->Get_OnButton())
		{
			dynamic_cast<CCanvas_MapBar*>(vecMapBar[i])->Set_OnButton();
			m_wsName = dynamic_cast<CCanvas_MapBar*>(vecMapBar[i])->Get_Name();

			// 여기서 맵 이동하기
			if (0 == i)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_TUTORIAL,
					CLevel_Loading_Simple::Create<CLevel_Tutorial>(m_pDevice, m_pContext));
			}
			else if (1 == i)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_CONSTRUCTIONSITE_3F,
					CLevel_Loading_Simple::Create<CLevel_ConstructionSite3F>(m_pDevice, m_pContext));
			}
			else if (2 == i)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_CONSTRUCTIONSITE_2F,
					CLevel_Loading_Simple::Create<CLevel_ConstructionSite2F>(m_pDevice, m_pContext));

			}
			else if (3 == i)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_SUBWAY,
					CLevel_Loading_Simple::Create<CLevel_Subway>(m_pDevice, m_pContext));
			}
			else if (4 == i)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_NAOMIROOM,
					CLevel_Loading_Simple::Create<CLevel_NaomiRoom>(m_pDevice, m_pContext));
			}
			else if (5 == i)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_HOSPITAL_1F,
					CLevel_Loading_Simple::Create<CLevel_Hospital_1F>(m_pDevice, m_pContext));
			}
		}
		else
		{
			dynamic_cast<CCanvas_MapBar*>(vecMapBar[i])->Set_OnAlpha();
		}
	}
}

CCanvas_WorldMap * CCanvas_WorldMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_WorldMap*		pInstance = new CCanvas_WorldMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_WorldMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_WorldMap::Clone(void * pArg)
{
	CCanvas_WorldMap*		pInstance = new CCanvas_WorldMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_WorldMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_WorldMap::Free()
{
	CCanvas::Free();
}
