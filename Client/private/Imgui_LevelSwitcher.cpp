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
#include "Level_Tutorial.h"
#include "Level_EnemiesTest.h"
#include "Level_Batch.h"
#include "Level_KineticEditor.h"
#include "Level_ConstructionSite3F.h"
#include "Level_Subway.h"
#include "Level_NaomiRoom.h"
#include "Level_Hospital_1F.h"
#include "Level_FinalStage.h"
#include "Level_ConstructionSite2F.h"
#include "Level_DownTown.h"

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
		if (ImGui::MenuItem("Level_EnemiesTest")) // 23.02.21 PJW add
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_ENEMIESTEST,
				CLevel_Loading_Simple::Create<CLevel_EnemiesTest>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_Batch")) // 23.02.21 PJW add
		{
		/*	CGameInstance::GetInstance()->Open_Loading(
				LEVEL_BATCH	,
				CLevel_Loading_Simple::Create<CLevel_Batch>(m_pDevice, m_pContext));*/
		}
		if (ImGui::MenuItem("Level_KineticEditor"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_KINETIC_EDITOR	,
				CLevel_Loading_Simple::Create<CLevel_KineticEditor>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_Tutorial"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_TUTORIAL,
				CLevel_Loading_Simple::Create<CLevel_Tutorial>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_ConstructionSite2F"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_CONSTRUCTIONSITE_2F,
				CLevel_Loading_Simple::Create<CLevel_ConstructionSite2F>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_ConstructionSite3F"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_CONSTRUCTIONSITE_3F,
				CLevel_Loading_Simple::Create<CLevel_ConstructionSite3F>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_Subway"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_SUBWAY,
				CLevel_Loading_Simple::Create<CLevel_Subway>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_NaomiRoom"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_NAOMIROOM,
				CLevel_Loading_Simple::Create<CLevel_NaomiRoom>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_Hospital1F"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_HOSPITAL_1F,
				CLevel_Loading_Simple::Create<CLevel_Hospital_1F>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_FinalStage"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_FINAL_STAGE,
				CLevel_Loading_Simple::Create<CLevel_FinalStage>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_DownTown1"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_DOWNTOWN_1,
				CLevel_Loading_Simple::Create<CLevel_DownTown>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_DownTown2"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_DOWNTOWN_2,
				CLevel_Loading_Simple::Create<CLevel_DownTown_Second>(m_pDevice, m_pContext));
		}
		if (ImGui::MenuItem("Level_DownTown3"))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_DOWNTOWN_3,
				CLevel_Loading_Simple::Create<CLevel_DownTown_Third>(m_pDevice, m_pContext));
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
