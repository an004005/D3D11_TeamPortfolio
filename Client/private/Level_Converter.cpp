#include "stdafx.h"
#include "..\public\Level_Converter.h"
#include <GameInstance.h>
#include "Imgui_LevelSwitcher.h"
#include "Imgui_ModelConverter.h"

CLevel_Converter::CLevel_Converter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Converter::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_ModelConverter::Create(m_pDevice, m_pContext));

	return S_OK;
}

CLevel_Converter* CLevel_Converter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Converter*	pInstance = new CLevel_Converter(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Maptool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Converter::Free()
{
	CLevel::Free();
}

HRESULT CLevel_Converter::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : Model Converter"));

	return CLevel::Render();
}
