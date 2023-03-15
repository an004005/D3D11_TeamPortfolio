#include "stdafx.h"
#include "..\public\CurveManager.h"
#include "CurveFloatMapImpl.h"
#include "JsonStorage.h"
#include "GameUtils.h"

IMPLEMENT_SINGLETON(CCurveManager);

CCurveManager::CCurveManager()
{
}

CCurveFloatImpl* CCurveManager::GetCurve(const string& strCurveTag)
{
	auto itr = m_Curves.find(strCurveTag);
	if (itr != m_Curves.end())
		return itr->second;

	return nullptr;
}

void CCurveManager::LoadCurves(const char* pCurveJsonDirPath)
{
	m_strCurveJsonDirPath = pCurveJsonDirPath;

	CGameUtils::ListFiles(m_strCurveJsonDirPath, [this](const string& filePath)
	{
		std::ifstream ifs(filePath);
		Json curveJson = Json::parse(ifs);
		auto pCurve = CCurveFloatImpl::Create(&curveJson);
		m_Curves.emplace(pCurve->GetName(), pCurve);
	});
}

void CCurveManager::SaveCurves()
{
	for (auto curve : m_Curves)
	{
		if (auto pCurveImpl = curve.second)
		{
			Json curveJson;
			pCurveImpl->SaveToJson(curveJson);
			string curveFilePath = m_strCurveJsonDirPath + pCurveImpl->GetName() + ".json";
			std::ofstream file(curveFilePath);
			file << curveJson;
		}
	}
}

void CCurveManager::Imgui_Render()
{
	ImGui::InputFloat2("Curve Editor Size", (float*)&m_CurveEditorSize);

	static string strSelectCurve;
	if (ImGui::BeginListBox("Curve list"))
	{
		static char szSeachCurve[MAX_PATH] = "";
		ImGui::InputText("Curve Search", szSeachCurve, MAX_PATH);

		const string strSearch = szSeachCurve;
		const _bool bSearch = strSearch.empty() == false;

		for (auto& Pair : m_Curves)
		{
			if (bSearch)
			{
				if (Pair.first.find(strSearch) == string::npos)
					continue;
			}

			const bool bSelected = strSelectCurve == Pair.first;
			if (bSelected)
				ImGui::SetItemDefaultFocus();

			if (ImGui::Selectable(Pair.first.c_str(), bSelected))
				strSelectCurve = Pair.first;
		}

		ImGui::EndListBox();
	}

	if (auto pCurve = GetCurve(strSelectCurve))
	{
		pCurve->Imgui_RenderEditor();
	}

	if (ImGui::Button("Delete Select Curve"))
	{
		if (MessageBox(NULL, L"진짜 삭제?", L"System Message", MB_YESNO) == IDYES)
		{
			if (auto pCurve = GetCurve(strSelectCurve))
			{
				m_Curves.erase(strSelectCurve);
				Safe_Release(pCurve);
				strSelectCurve.clear();
			}
		}
	}

	static char szAddCurve[MAX_PATH] = "";
	ImGui::InputText("Add Curve Name", szAddCurve, MAX_PATH);
	if (ImGui::Button("Add Curve") && strcmp(szAddCurve, "") != 0)
	{
		auto pCurve = GetCurve(szAddCurve);
		if (pCurve == nullptr)
		{
			m_Curves.emplace(szAddCurve, CCurveFloatImpl::Create(szAddCurve));
		}
	}

	if (ImGui::Button("Update CurveName"))
	{
		list<string> oldCurveNames;
		for (auto& Pair : m_Curves)
		{
			if (Pair.first != Pair.second->GetName())
				oldCurveNames.push_back(Pair.first);
		}

		for (auto& oldCurveName : oldCurveNames)
		{
			if (auto pCurve = GetCurve(oldCurveName))
			{
				m_Curves.erase(oldCurveName);
				m_Curves.emplace(pCurve->GetName(), pCurve);
			}
		}
	}

	if (ImGui::Button("Save Curves"))
	{
		SaveCurves();
	}
}

void CCurveManager::Free()
{
	for (auto& curve : m_Curves)
		Safe_Release(curve.second);
}

