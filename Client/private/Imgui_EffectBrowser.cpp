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
				dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Work_EffectGroup", TEXT("ProtoVFX_EffectGroup"), &EffectJson))->Start_EffectWork();
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
				CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Work_ParticleSystem", L"ProtoVFX_ParticleSystem", &EffectJson);
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
				dynamic_cast<CParticleGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Work_ParticleGroup", L"ProtoVFX_ParticleGroup", &EffectJson))->Start_ParticleWork();
			}
		}
	}
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::NewLine();

	if (ImGui::CollapsingHeader("EFFECT_Viewer"))
	{
		const char* EffectTypes[EFFECTFOLDER::EFFECT_END] = { "Blood_Effect", "Defualt_Effect", "Elec_Effect",
															"Fire_Effect", "ENV_Effect", "Monster_Effect", "Sas_Effect" };

		ImGui::Combo("##Effect_Type", &m_iSelectFolder, EffectTypes, IM_ARRAYSIZE(EffectTypes));
		ImGui::SameLine();
		if(ImGui::Button("Update_Effect_List"))
		{
			switch (m_iSelectFolder)
			{
				case BLOOD_EFFECT:
					LoadEffects("../Bin/Resources/Curve/EffectGroup/Blood_Effect/");
					break;

				case DEFAULT_EFFECT:
					LoadEffects("../Bin/Resources/Curve/EffectGroup/Default_Attack/");
					break;

				case ELEC_EFFECT:
					LoadEffects("../Bin/Resources/Curve/EffectGroup/Elec_Attack/");
					break;

				case FIRE_EFFECT:
					LoadEffects("../Bin/Resources/Curve/EffectGroup/Fire_Attack/");
					break;

				case ENV_EFFECT:
					LoadEffects("../Bin/Resources/Curve/EffectGroup/Envirenment_Effect/");
					break;

				case MONSTER_EFFECT:
					LoadEffects("../Bin/Resources/Curve/EffectGroup/Monster_Effect/");
					break;

				case SAS_EFFECT:
					LoadEffects("../Bin/Resources/Curve/EffectGroup/Sas_Effect/");
					break;

			default:
					NODEFAULT;
			}
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

	if (ImGui::CollapsingHeader("PARTICLE_Viewer"))
	{
		const char* ParticleTypes[EFFECTFOLDER::EFFECT_END] = { "Blood_Particle", "Defualt_Particle", "Elec_Particle",
															"Fire_Particle", "ENV_Particle", "Monster_Particle", "Sas_Particle" };

		ImGui::Combo("##Particle_Type", &m_iSelectFolder, ParticleTypes, IM_ARRAYSIZE(ParticleTypes));
		ImGui::SameLine();
		if (ImGui::Button("Update_Particle_List"))
		{
			switch (m_iSelectFolder)
			{
			case BLOOD_EFFECT:
				LoadParticles("../Bin/Resources/Curve/ParticleGroup/Blood_Particle/");
				break;

			case DEFAULT_EFFECT:
				LoadParticles("../Bin/Resources/Curve/ParticleGroup/Default_Particle/");
				break;

			case ELEC_EFFECT:
				LoadParticles("../Bin/Resources/Curve/ParticleGroup/Elec_Particle/");
				break;

			case FIRE_EFFECT:
				LoadParticles("../Bin/Resources/Curve/ParticleGroup/Fire_Particle/");
				break;

			case ENV_EFFECT:
				LoadParticles("../Bin/Resources/Curve/ParticleGroup/Envirenment_Particle/");
				break;

			case MONSTER_EFFECT:
				LoadParticles("../Bin/Resources/Curve/ParticleGroup/Monster_Particle/");
				break;

			case SAS_EFFECT:
				LoadParticles("../Bin/Resources/Curve/ParticleGroup/Sas_Particle/");
				break;

			default:
				NODEFAULT;
			}
		}

		static char szSearchParticle[MAX_PATH] = "";
		ImGui::InputText("Particle Search", szSearchParticle, MAX_PATH);

		const string strSearch = szSearchParticle;
		const _bool bSearch = strSearch.empty() == false;

		if (ImGui::BeginListBox("Particles List"))
		{
			for (auto& Pair : m_mapParticleGroup)
			{
				if (bSearch)
				{
					if (Pair.first.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = m_CurParticleName == Pair.first;

				if (bSelected)
				{
					ImGui::SetItemDefaultFocus();

					// {
						Json EffectJson = CJsonStorage::GetInstance()->LoadJson_ForWork(Pair.second);
						if (EffectJson.empty())
							MSG_BOX("Failed to Add New ParticleGroup");
						else
						{
							dynamic_cast<CParticleGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Work_ParticleGroup", L"ProtoVFX_ParticleGroup", &EffectJson))->Start_ParticleWork_Play();
						}
					// }



					// Json jsonEffect = CJsonStorage::GetInstance()->LoadJson_ForWork(Pair.second);
					// dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectFolder", L"ProtoVFX_EffectGroup", &jsonEffect))->Start_EffectWork();

					m_CurParticleName = "";
				}

				if (ImGui::Selectable(Pair.first.c_str(), bSelected))
					m_CurParticleName = Pair.first;
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

	ImGui::Separator();
	ImGui::Separator();


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

void CImgui_EffectBrowser::LoadParticles(const char* pParticleDir)
{
	m_mapParticleGroup.clear();

	CGameUtils::ListFiles(pParticleDir, [this](const string& pParticlePath)
		{
			string FileName = CGameUtils::GetFileName(pParticlePath);

			m_mapParticleGroup.emplace(FileName, pParticlePath);
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


