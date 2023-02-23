#include "stdafx.h"
#include "Imgui_LevelSwitcher.h"
#include "GameInstance.h"
#include "Level_Loading_Simple.h"
#include "Level_GamePlay.h"
#include "Level_Maptool.h"
#include "Level_Converter.h"
#include "Level_AnimModify.h"
#include "Level_Effect.h"
#include "Level_UI.h"
#include "Level_PlayerTest.h"

CImgui_LevelSwitcher::CImgui_LevelSwitcher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_LevelSwitcher::Initialize(void* pArg)
{
	return CImguiObject::Initialize(pArg);
}

void CImgui_LevelSwitcher::Imgui_RenderMenu()
{
	if (ImGui::BeginMenu("Levels"))
	{
		if (ImGui::MenuItem("Level_GamePlaye"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_GAMEPLAY, 
				CLevel_Loading_Simple::Create<CLevel_GamePlay>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_MapTool"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_MAPTOOL, 
				CLevel_Loading_Simple::Create<CLevel_Maptool>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_AnimModifier"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_ANIM, 
				CLevel_Loading_Simple::Create<CLevel_AnimModify>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_Converter"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_CONVERTER, 
				CLevel_Loading_Simple::Create<CLevel_Converter>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_PlayerTest"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_PLAYERTEST,
				CLevel_Loading_Simple::Create<CLevel_PlayerTest>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_Effect"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_EFFECT, 
				CLevel_Loading_Simple::Create<CLevel_Effect>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_UI"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_UI,
				CLevel_Loading_Simple::Create<CLevel_UI>(m_pDevice, m_pContext));
		}
		ImGui::EndMenu();
	}
}

CImgui_LevelSwitcher* CImgui_LevelSwitcher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_LevelSwitcher(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
		return nullptr;

	return inst;
}

void CImgui_LevelSwitcher::Free()
{
	CImguiObject::Free();
}
