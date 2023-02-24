#include "stdafx.h"
#include "..\public\EffectGroup.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "CurveFloatMapImpl.h"
#include "JsonLib.h"
#include "ImguiUtils.h"
#include "EffectSystem.h"
#include "GameUtils.h"
#include "Material.h"
#include "MaterialPreview.h"

CEffectGroup::CEffectGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectGroup::CEffectGroup(const CEffectGroup& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectGroup::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	if(pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		for(auto curveJson : json["Curves"])
		{
			auto pCurve = CCurveFloatImpl::Create(&curveJson);
			m_Curves.emplace(pCurve->GetName(), pCurve);
		}
	}

	m_Timeline.SetTimelineLength(3.0);
	m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);

	if (m_Curves.empty())
	{
		_int i = 0;
		AddEmptyCurve("Scale"); // 스케일 변경
		
		AddEmptyCurve("IntroDissolve"); // 나타날 때
		AddEmptyCurve("OutroDissolve"); // 죽을 때

		AddEmptyCurve("ColorChange"); // 색상 그대로 받기 (_float4)

		AddEmptyCurve("EmissiveChange"); // HDR 강도 변경할 때

		AddEmptyCurve("IntroTime"); // 디졸브 안 쓸 때
		AddEmptyCurve("OutroTime");

		//Todo:: 회전축 어떻게 잡을지 고민
		// AddEmptyCurve("Rotation"); // 회전값 변경, 축 어떻게 전달할지 결정하기
	}

	// void Tick_Scale(_float fValue);
	// void Tick_Floats_Value(_uint iParamIndex, _float fValue);
	// void Tick_Float4s_Value(_uint iParamIndex, _float4 fValue);

	// m_Timeline.SetCurve(this, &CEffectGroup::Tick_Scale, m_Curves.find("Scale")->second);
	// Safe_AddRef(m_Curves.find("Scale")->second);
	//
	// m_Timeline.SetCurve(this, &CEffectGroup::Tick_Floats_Value, m_Curves.find("IntroDissolve")->second);
	// Safe_AddRef(m_Curves.find("IntroDissolve")->second);
	//
	// m_Timeline.SetCurve(this, &CEffectGroup::Tick_Floats_Value, m_Curves.find("OutroTime")->second);
	// Safe_AddRef(m_Curves.find("OutroTime")->second);

	m_Timeline.PlayFromStart();

	// CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	// {
	// 	char szFileName[MAX_PATH]{};
	// 	_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	// 	CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	// });


	return S_OK;
}

void CEffectGroup::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	m_Timeline.Tick(TimeDelta);
}

void CEffectGroup::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	LoadEffectData();

	ImGui::Separator();

	m_Timeline.Imgui_RenderEditor();
	ImGui::Separator();

	static const char* pCurName = "";
	if (ImGui::BeginListBox("Curves"))
	{
		for (auto pCurve : m_Curves)
		{
			const bool bSelected = strcmp(pCurve.first, pCurName) == 0;
			if (bSelected)
				ImGui::SetItemDefaultFocus();
			if (ImGui::Selectable(pCurve.first, bSelected))
				pCurName = pCurve.first;
		}

		ImGui::EndListBox();
	}

	auto itr = m_Curves.find(pCurName);

	if (itr != m_Curves.end())
	{
		itr->second->Imgui_RenderEditor();
	}

	CImguiUtils::FileDialog_FileSelector("Save Curves And Group", ".json", "../Bin/Resources/Curve/",
		[this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});

	Imgui_RenderEffectSource();
}

void CEffectGroup::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);

	json["Curves"] = Json::array();
	for (auto pCurve : m_Curves)
	{
		Json curveJson;
		pCurve.second->SaveToJson(curveJson);
		json["Curves"].push_back(curveJson);
	}
	// json["TimelineLength"] = 
}

void CEffectGroup::LoadEffectData()
{
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_First_EffectProtoTag.c_str());
		ImGui::InputText("1_EffectDir", EffectProtoTag, MAX_PATH);
		m_First_EffectProtoTag = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load First"))
		{
			if (m_pFirst_EffectSystem != nullptr)
				Safe_Release(m_pFirst_EffectSystem);

			if (m_First_EffectProtoTag == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json First_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pFirst_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &First_Effect));
		}
	}
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_Second_EffectProtoTag.c_str());
		ImGui::InputText("2_EffectDir", EffectProtoTag, MAX_PATH);
		m_Second_EffectProtoTag = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load Second"))
		{
			if (m_pSecond_EffectSystem != nullptr)
				Safe_Release(m_pSecond_EffectSystem);

			if (m_Second_EffectProtoTag == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json Secon_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pSecond_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Secon_Effect));
		}
	}
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_Third_EffectProtoTag.c_str());
		ImGui::InputText("3_EffectDir", EffectProtoTag, MAX_PATH);
		m_Third_EffectProtoTag = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load Third"))
		{
			if (m_pThird_EffectSystem != nullptr)
				Safe_Release(m_pThird_EffectSystem);

			if (m_Third_EffectProtoTag == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json Third_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pThird_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Third_Effect));
		}
	}
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_Fourth_EffectProtoTag.c_str());
		ImGui::InputText("4_EffectDir", EffectProtoTag, MAX_PATH);
		m_Fourth_EffectProtoTag = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load Fourth"))
		{
			if (m_pFourth_EffectSystem != nullptr)
				Safe_Release(m_pFourth_EffectSystem);

			if (m_Fourth_EffectProtoTag == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json Fourth_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pFourth_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Fourth_Effect));
		}
	}
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_Fifth_EffectProtoTag.c_str());
		ImGui::InputText("5_EffectDir", EffectProtoTag, MAX_PATH);
		m_Fifth_EffectProtoTag = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load Fifth"))
		{
			if (m_pFifth_EffectSystem != nullptr)
				Safe_Release(m_pFifth_EffectSystem);

			if (m_Fourth_EffectProtoTag == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json Fifth_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pFifth_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Fifth_Effect));
		}
	}
}

void CEffectGroup::Imgui_RenderEffectSource()
{
	ImGui::BeginTabBar("EffectGroup_Sources");
	if(m_pFirst_EffectSystem != nullptr)
	{
		ImGui::Begin("First_Effect");
		m_pFirst_EffectSystem->Imgui_RenderProperty();
		ImGui::End();
	}
	if (m_pSecond_EffectSystem != nullptr)
	{
		ImGui::Begin("Second_Effect");
		m_pSecond_EffectSystem->Imgui_RenderProperty();
		ImGui::End();
	}
	if (m_pThird_EffectSystem != nullptr)
	{
		ImGui::Begin("Third_Effect");
		m_pThird_EffectSystem->Imgui_RenderProperty();
		ImGui::End();
	}
	if (m_pFourth_EffectSystem != nullptr)
	{
		ImGui::Begin("Fourth_Effect");
		m_pFourth_EffectSystem->Imgui_RenderProperty();
		ImGui::End();
	}
	if (m_pFifth_EffectSystem != nullptr)
	{
		ImGui::Begin("Fifth_Effect");
		m_pFifth_EffectSystem->Imgui_RenderProperty();
		ImGui::End();
	}

	ImGui::EndTabBar();

}


void CEffectGroup::AddEmptyCurve(string strCurveName)
{
	auto itr = m_Curves.find(strCurveName.c_str());
	if (itr != m_Curves.end())
		return;

	auto pCurve = CCurveFloatImpl::Create(strCurveName);
	m_Curves.emplace(pCurve->GetName(), pCurve);
}

void CEffectGroup::Tick_Scale(_uint iParamIndex, _float fValue)
{
	fValue *= 2.f;
	m_pTransformCom->Set_Scaled(_float3(fValue, fValue, fValue));
}

void CEffectGroup::Tick_Floats_Value(_uint iParamIndex, _float fValue)
{
	

	m_pCurSelect_Effect->GetParams().Floats[iParamIndex] = fValue;
}

void CEffectGroup::Tick_Float4s_Value(_uint iParamIndex, _float4 fValue)
{
	m_pCurSelect_Effect->GetParams().Float4s[iParamIndex] = fValue;
}

CEffectGroup* CEffectGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffectGroup*		pInstance = new CEffectGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffectGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffectGroup::Clone(void* pArg)
{
	CEffectGroup*		pInstance = new CEffectGroup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffectGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffectGroup::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);

	for (auto pCurve : m_Curves)
		Safe_Release(pCurve.second);
	m_Curves.clear();

	if(m_pFirst_EffectSystem != nullptr)
		Safe_Release(m_pFirst_EffectSystem);

	if (m_pSecond_EffectSystem != nullptr)
		Safe_Release(m_pSecond_EffectSystem);

	if (m_pThird_EffectSystem != nullptr)
		Safe_Release(m_pThird_EffectSystem);

	if (m_pFourth_EffectSystem != nullptr)
		Safe_Release(m_pFourth_EffectSystem);

	if (m_pFifth_EffectSystem != nullptr)
		Safe_Release(m_pFifth_EffectSystem);
}
