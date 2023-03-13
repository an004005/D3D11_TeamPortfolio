#include "stdafx.h"
#include "..\public\Imgui_EffectBrowser.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "JsonStorage.h"
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
#include "EffectSystem.h"
#include "EffectGroup.h"
#include "TrailSystem.h"
#include "ParticleSystem.h"
#include "VFX_Manager.h"

CImgui_EffectBrowser::CImgui_EffectBrowser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_EffectBrowser::Initialize(void* pArg)
{
	// m_szTabname = "Effect's Home";
	m_szWindowName = "Effect Browser";

	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
	return CImguiObject::Initialize(pArg);
}

void CImgui_EffectBrowser::Imgui_RenderWindow()
{
	{
		char EffectSystemTag[MAX_PATH];
		strcpy(EffectSystemTag, m_EffectSystemTag.c_str());
		ImGui::InputText("EffectSystem Tag", EffectSystemTag, MAX_PATH);
		m_EffectSystemTag = EffectSystemTag;

		if (ImGui::Button("Add New EffectSystem"))
		{
			Json EffectJson = CJsonStorage::GetInstance()->LoadJson_ForWork(m_EffectSystemTag);
			if (EffectJson.empty())
				MSG_BOX("Failed to Add New EffectSystem");
			else
			{
				CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Work_EffectSystem", TEXT("ProtoVFX_EffectSystem"), &EffectJson);
			}
		}
	}

	ImGui::Separator();
	ImGui::Separator();
	{
		char EffectGroupTag[MAX_PATH];
		strcpy(EffectGroupTag, m_EffectGroupTag.c_str());
		ImGui::InputText("EffectGroup Tag", EffectGroupTag, MAX_PATH);
		m_EffectGroupTag = EffectGroupTag;
		if (ImGui::Button("Add New EffectGroup"))
		{
			Json EffectJson = CJsonStorage::GetInstance()->LoadJson_ForWork(m_EffectGroupTag);
			if (EffectJson.empty())
				MSG_BOX("Failed to Add New EffectGroup");
			else
			{
				CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Work_EffectGroup", TEXT("ProtoVFX_EffectGroup"), &EffectJson);
			}
		}
	}

	ImGui::Separator();
	ImGui::Separator();

	{
		char ParticleTag[MAX_PATH];
		strcpy(ParticleTag, m_ParticleSystemTag.c_str());
		ImGui::InputText("ParticleSystem Tag", ParticleTag, MAX_PATH);
		m_ParticleSystemTag = ParticleTag;

		if (ImGui::Button("Add New ParticleSystem"))
		{
			Json EffectJson = CJsonStorage::GetInstance()->LoadJson_ForWork(m_ParticleSystemTag);
			if (EffectJson.empty())
				MSG_BOX("Failed to Add New ParticleSystem");
			else
			{
				CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Work_ParticleSystem", TEXT("ProtoVFX_ParticleSystem"), &EffectJson);
			}
		}
	}

	ImGui::Separator();
	ImGui::Separator();

	{
		char ParticleGroupTag[MAX_PATH];
		strcpy(ParticleGroupTag, m_ParticleGroupTag.c_str());
		ImGui::InputText("ParticleGroup Tag", ParticleGroupTag, MAX_PATH);
		m_ParticleGroupTag = ParticleGroupTag;

		if (ImGui::Button("Add New ParticleGroup"))
		{
			Json EffectJson = CJsonStorage::GetInstance()->LoadJson_ForWork(m_ParticleGroupTag);
			if (EffectJson.empty())
				MSG_BOX("Failed to Add New ParticleGroup");
			else
			{
				CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Work_ParticleGroup", TEXT("ProtoVFX_ParticleGroup"), &EffectJson);
			}
		}
	}
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::NewLine();

	if (ImGui::CollapsingHeader("Effect Viewer"))
	{
		if (ImGui::Button("Refresh_Effect Folder"))
		{
			// LoadEffects("../Bin/Resources/Curve/EffectGroup/Fire_Attack/");
			// LoadEffects("../Bin/Resources/Curve/EffectGroup/Elec_Attack/");
			LoadEffects("../Bin/Resources/Curve/EffectGroup/Default_Attack/");
			// LoadEffects("../Bin/Resources/Curve/EffectGroup/NeedToWork/");
		}

		static char szSearchEffect[MAX_PATH] = "";
		ImGui::InputText("Effect Search", szSearchEffect, MAX_PATH);
		
		const string strSearch = szSearchEffect;
		const _bool bSearch = strSearch.empty() == false;

		if (ImGui::BeginListBox("Effects List"))
		{
			for (auto& Pair : m_mapEffectGroup)
			{
				if (bSearch)
				{
					if (Pair.first.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = m_CurEffectName == Pair.first;
				if (bSelected)
				{
					ImGui::SetItemDefaultFocus();

					// CVFX_Manager::GetInstance()->GetEffect()

					Json jsonEffect = CJsonStorage::GetInstance()->LoadJson_ForWork(Pair.second);
					// CVFX_Manager::GetEffect()
					dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectFolder", L"ProtoVFX_EffectGroup", &jsonEffect))->Start_EffectWork();

					// Pair.second->SetPlay();
					m_CurEffectName = "";
				}

				if (ImGui::Selectable(Pair.first.c_str(), bSelected))
					m_CurEffectName = Pair.first;
			}
			ImGui::EndListBox();
		}
	}

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::NewLine();

	if (ImGui::Button("Add Sample EffectSystem"))
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_NOW, L"Layer_Work_EffectSys", TEXT("ProtoVFX_EffectSystem")));
	}
	ImGui::SameLine();
	if ( ImGui::Button("Add Sample EffectGroup"))
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_NOW, L"Layer_Work_EffectGroup", TEXT("ProtoVFX_EffectGroup")));
	}
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::NewLine();
	if( ImGui::Button("Add Sample ParticleSystem"))
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_NOW, L"Layer_Work_ParticleSystem", TEXT("ProtoVFX_ParticleSystem")));
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Sample ParticleGroup"))
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_NOW, L"Layer_Work_ParticleGroup", TEXT("ProtoVFX_ParticleGroup")));
	}

}

void CImgui_EffectBrowser::LoadEffects(const char* pEffectDir)
{
	m_mapEffectGroup.clear();

	CGameUtils::ListFiles(pEffectDir, [this](const string& pEffectPath)
	{
		string FileName = CGameUtils::GetFileName(pEffectPath);

		m_mapEffectGroup.emplace(FileName, pEffectPath);
	});
}

CImgui_EffectBrowser* CImgui_EffectBrowser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_EffectBrowser(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
		return nullptr;

	return inst;
}

void CImgui_EffectBrowser::Free()
{
	CImguiObject::Free();

	Safe_Release(m_pGameInstance);
}


