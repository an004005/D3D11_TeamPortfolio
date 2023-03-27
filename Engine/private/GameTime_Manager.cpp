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
		m_TimeRatio = m_DefaultTimeRatio;
	}
}

void CGameTime_Manager::ResetTimeRatio()
{
	m_Timeline.Stop();
	m_TimeRatio = m_DefaultTimeRatio;
	m_Timeline.ReleaseCurve();
	m_Timeline.SetStay(false);

	for (auto& layer : CObject_Manager::GetInstance()->GetLayers(LEVEL_NOW))
		layer.second->SetUseTimeRatio(true);

	for (auto& layerTag : m_DefaultExceptLayers)
	{
		if (auto pLayer = CObject_Manager::GetInstance()->GetLayer(LEVEL_NOW, layerTag.c_str()))
		{
			pLayer->SetUseTimeRatio(false);
		}
	}
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

void CGameTime_Manager::ResetDefaultTimeRatio()
{
	m_DefaultTimeRatio = 1.f;
	m_DefaultExceptLayers.clear();
	//for (auto& layer : CObject_Manager::GetInstance()->GetLayers(LEVEL_NOW))
	//	layer.second->SetUseTimeRatio(true);
}

void CGameTime_Manager::SetTimeRatio(_float fTimeRatio, const vector<wstring>* ExceptLayers)
{
	m_TimeRatio = fTimeRatio;
	m_DefaultTimeRatio = fTimeRatio;

	for (auto& layer : CObject_Manager::GetInstance()->GetLayers(LEVEL_NOW))
		layer.second->SetUseTimeRatio(true);

	if (ExceptLayers)
	{
		m_DefaultExceptLayers = *ExceptLayers;
		for (auto& layerTag : *ExceptLayers)
		{
			if (auto pLayer = CObject_Manager::GetInstance()->GetLayer(LEVEL_NOW, layerTag.c_str()))
			{
				pLayer->SetUseTimeRatio(false);
			}
		}
	}
	else
	{
		m_DefaultExceptLayers.clear();
	}
}

void CGameTime_Manager::SetLayerTimeRatio(_float fLayerTimeRatio, const wstring& strLayerTag)
{
	if (auto pLayer = CObject_Manager::GetInstance()->GetLayer(LEVEL_NOW, strLayerTag.c_str()))
	{
		pLayer->SetLayerTimeRatio(fLayerTimeRatio);
	}
}

void CGameTime_Manager::ClearAllTimeRatio()
{
	ResetDefaultTimeRatio();
	ResetTimeRatio();
	for (auto& layer : CObject_Manager::GetInstance()->GetLayers(LEVEL_NOW))
		layer.second->SetLayerTimeRatio(1.f);
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

