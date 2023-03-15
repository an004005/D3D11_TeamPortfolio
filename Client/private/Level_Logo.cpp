#include "stdafx.h"
#include "Level_Logo.h"
#include "GameInstance.h"
#include "Imgui_PropertyEditor.h"
#include "Level_Loading_Simple.h"
#include "Level_GamePlay.h"
#include "Imgui_LevelSwitcher.h"
#include "JsonStorage.h"
#include "Level_Maptool.h"
#include "VFX_Manager.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	Ready_com();
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CVFX_Manager::GetInstance()->Initialize((LEVEL)LEVEL_NOW);

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CLevel_Logo::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	{
		CGameInstance::GetInstance()->Open_Loading(
			LEVEL_GAMEPLAY,
			CLevel_Loading_Simple::Create<CLevel_GamePlay>(m_pDevice, m_pContext));
		return;
	}
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : LOGO"));


	return S_OK;
}

void CLevel_Logo::Ready_com()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
