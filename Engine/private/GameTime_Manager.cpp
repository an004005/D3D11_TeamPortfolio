#include "stdafx.h"
#include "..\public\GameTime_Manager.h"
#include "CurveManager.h"
#include "Object_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CGameTime_Manager);

CGameTime_Manager::CGameTime_Manager()
{
}

void CGameTime_Manager::Tick(_double TimeDelta)
{
	_float fOut;
	if (m_Timeline.Tick(TimeDelta, fOut))
	{
		m_TimeRatio = fOut;
	}
	else
	{
		m_TimeRatio = 1.f;
	}
}

void CGameTime_Manager::ResetTimeRatio()
{
	m_Timeline.Stop();
	m_TimeRatio = 1.f;
	m_Timeline.ReleaseCurve();
	m_Timeline.SetStay(false);

	for (auto& layer : CObject_Manager::GetInstance()->GetLayers(LEVEL_NOW))
		layer.second->SetUseTimeRatio(false);
}

void CGameTime_Manager::SetTimeRatioCurve(const string& strCurveTag, _bool bStay, const vector<wstring>* ExceptLayers)
{
	auto pCurve = CCurveManager::GetInstance()->GetCurve(strCurveTag);
	if (pCurve == nullptr)
		return;

	m_Timeline.SetCurve(strCurveTag);
	m_Timeline.SetStay(bStay);

	m_Timeline.SetFinishFunction([this]
	{
		ResetTimeRatio();
	});
	m_Timeline.PlayFromStart();

	for (auto& layer : CObject_Manager::GetInstance()->GetLayers(LEVEL_NOW))
		layer.second->SetUseTimeRatio(true);

	if (ExceptLayers)
	{
		for (auto& layerTag : *ExceptLayers)
		{
			if (auto pLayer = CObject_Manager::GetInstance()->GetLayer(LEVEL_NOW, layerTag.c_str()))
			{
				pLayer->SetUseTimeRatio(false);
			}
		}
	}
}

void CGameTime_Manager::Imgui_Render()
{
	ImGui::InputFloat("TimeRatio", &m_TimeRatio);
	static char szTimeRatioCurveName[MAX_PATH]{};
	ImGui::InputText("TimeRatio CurveName", szTimeRatioCurveName, MAX_PATH);
	if (ImGui::Button("Run Curve"))
	{
		SetTimeRatioCurve(szTimeRatioCurveName);
	}
	if (ImGui::Button("Reset"))
	{
		ResetTimeRatio();
	}
}

void CGameTime_Manager::Free()
{
}

