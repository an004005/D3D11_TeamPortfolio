#include "stdafx.h"
#include "..\public\CurveFloatMapImpl.h"
#include "MathUtils.h"
#include "JsonLib.h"
#include "ImguiUtils.h"
#include "imgui_curve.hpp"
#include "MathUtils.h"

namespace nlohmann
{
	template <>
	struct adl_serializer<FloatMapKeyFrame>
	{
		static void to_json(json& j, const FloatMapKeyFrame& value)
		{
			j["Time"] = value.Time;
			j["CurveData"] = value.CurveData;
		}

		static void from_json(const json& j, FloatMapKeyFrame& value)
		{
			value.Time = j["Time"];
			value.CurveData = j["CurveData"];
		}
	};
}

CCurveFloatMapImpl::CCurveFloatMapImpl()
{
}

map<string, _float> CCurveFloatMapImpl::GetValue(_double Frame)
{
	map<string, _float> KeyFrame;

	if (Frame >= m_KeyFrames.back().Time)
	{
		KeyFrame = m_KeyFrames.back().CurveData;
	}
	else if (Frame <= m_KeyFrames.front().Time)
	{
		KeyFrame = m_KeyFrames.front().CurveData;
	}
	else
	{
		KeyFrame = m_DefaultKeyFrame;

		_uint iIdx = 0;
		while (Frame >= m_KeyFrames[iIdx + 1].Time)
			++iIdx;

		map<string, _float>& PreData = m_KeyFrames[iIdx].CurveData;
		map<string, _float>& NextData = m_KeyFrames[iIdx + 1].CurveData;

		const _float fRatio = (_float)(Frame - m_KeyFrames[iIdx].Time) / 
			(m_KeyFrames[iIdx + 1].Time - m_KeyFrames[iIdx].Time);

		for (auto& curveData : KeyFrame)
		{
			curveData.second = CMathUtils::Lerp(PreData[curveData.first], NextData[curveData.first], fRatio);
		}

	}

#ifdef _DEBUG
	m_LastUsedValue.Time = Frame;
	m_LastUsedValue.CurveData = KeyFrame;
#endif

	return KeyFrame;
}

void CCurveFloatMapImpl::SaveToJson(Json& json)
{
	json["CurveKeyFrames"] = Json::array();
	for (auto& e : m_KeyFrames)
		json["CurveKeyFrames"].push_back(e);
}

void CCurveFloatMapImpl::LoadFromJson(const Json& json)
{
	for (auto& j : json["CurveKeyFrames"])
		m_KeyFrames.push_back(j);

	if (m_KeyFrames.empty())
		return;

	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [](const FloatMapKeyFrame& left, const FloatMapKeyFrame& right)
	{
		return left.Time < right.Time;
	});

	m_DefaultKeyFrame = m_KeyFrames.front().CurveData;
	for (auto& e : m_DefaultKeyFrame)
		e.second = 0.f;
}

void CCurveFloatMapImpl::Imgui_RenderEditor()
{
	CImguiUtils::FileDialog_FileSelector("Save Curve", ".json", "../Bin/Resources/Level/Curve/",
		[this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});
	ImGui::SameLine();
	CImguiUtils::FileDialog_FileSelector("Load Curve", ".json", "../Bin/Resources/Level/Curve/",
		[this](const string& filePath)
	{
		std::ifstream ifs(filePath);
		Json json = Json::parse(ifs);
		LoadFromJson(json);
	});



}

void CCurveFloatMapImpl::AddKey(const string& key)
{
	m_DefaultKeyFrame.emplace(key, 0.f);
	for (auto& e : m_KeyFrames)
		e.CurveData.emplace(key, 0.f);
}

void CCurveFloatMapImpl::DeleteKey(const string& key)
{
	m_DefaultKeyFrame.erase(key);
	for (auto& e : m_KeyFrames)
		e.CurveData.erase(key);
}

void CCurveFloatMapImpl::Free()
{
}



CCurveFloatImpl::CCurveFloatImpl()
{
}

void CCurveFloatImpl::SaveToJson(Json& json)
{
	json["Frames"] = m_KeyFrames;
	json["Name"] = m_szName;
	json["RangeMin"] = m_RangeMin;
	json["RangeMax"] = m_RangeMax;
}

void CCurveFloatImpl::LoadFromJson(const Json& json)
{
	string name = json["Name"];
	strcpy_s(m_szName, name.c_str());

	m_RangeMin = json["RangeMin"];
	m_RangeMax = json["RangeMax"];

	_int idx = 0;
	for (auto frame : json["Frames"])
	{
		m_KeyFrames[idx] = frame;
		++idx;
		if (idx >= m_iMaxPoint)
			break;
	}
}

_float CCurveFloatImpl::GetValue(_double Frame)
{
	return ImGui::CurveValueSmooth((_float)Frame, m_iMaxPoint, m_KeyFrames);
}

void CCurveFloatImpl::Imgui_RenderEditor()
{
	ImGui::InputText("CurveName", m_szName, MAX_PATH);
	ImGui::InputFloat2("Range Min", (float*)&m_RangeMin);
	ImGui::InputFloat2("Range Max", (float*)&m_RangeMax);

	if (ImGui::Curve(m_szName, ImVec2{400.f, 300.f}, 20, m_KeyFrames, &m_iSelection, m_RangeMin, m_RangeMax))
	{// 그래프 변경시 true 반환됨
	}
}

void CCurveFloatImpl::Free()
{
}

CCurveFloatImpl* CCurveFloatImpl::Create(Json* json)
{
	auto inst = new CCurveFloatImpl();

	if (json)
	{
		inst->LoadFromJson(*json);
	}
	else
	{
		inst->m_KeyFrames[0].x = ImGui::CurveTerminator;
	}

	return inst;
}

CCurveFloatImpl* CCurveFloatImpl::Create(const string& strName)
{
	auto inst = new CCurveFloatImpl();
	inst->m_KeyFrames[0].x = ImGui::CurveTerminator;
	strcpy_s(inst->m_szName, strName.c_str());

	return inst;
}
