#include "stdafx.h"
#include "..\public\Timeline.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "CurveFloatMapImpl.h"
#include "CurveManager.h"

/******************
 * CCurveFloatSmoothStepRatio
 *****************/
_float CCurveFloatSmoothStepRatio::GetValue(_double Frame)
{
	return CMathUtils::SmoothStep(static_cast<_float>(Frame) / m_fLength);
}

/******************
 * CTimeline
 *****************/
void CTimeline::Tick(_double TimeDelta)
{
	if (m_bPlay == false)
		return;
	
	if (m_bForward)
		m_CurFrame += TimeDelta;
	else
		m_CurFrame -= TimeDelta;

	_bool bFin = false;
	if (m_CurFrame <= 0.0)
	{
		m_bPlay = false;
		m_CurFrame = 0.0;
		bFin = true;
	}
	else if (m_CurFrame >= m_EndTime)
	{
		m_bPlay = false;
		m_CurFrame = m_EndTime;
		bFin = true;
	}

	for (auto& e : m_vecFloatCurve)
		e.first(e.second->GetValue(m_CurFrame));

	for (auto& e : m_vecVec3Curve)
		e.first(e.second->GetValue(m_CurFrame));

	// for (auto& e : m_vecFloatMap)
		// e.first(e.second->GetValue(m_CurFrame));

	if (bFin)
	{
		if (m_FinFunction)
			m_FinFunction();
		if (m_bLoop)
		{
			m_CurFrame = m_bForward ? 0.0 : m_EndTime;
			m_bPlay = true;
		}
	}
}

void CTimeline::Play()
{
	m_bForward = true;
	m_bPlay = true;
}

void CTimeline::PlayFromStart()
{
	m_CurFrame = 0.0;
	Play();
}

void CTimeline::Stop()
{
	m_bPlay = false;
}

void CTimeline::Reverse()
{
	m_bForward = false;
	m_bPlay = true;
}

void CTimeline::ReverseFromEnd()
{
	m_CurFrame = m_EndTime;
	Reverse();
}

void CTimeline::SetNewFrame(_double NewFrame)
{
	m_CurFrame = NewFrame;
}

void CTimeline::Reset()
{
	m_CurFrame = 0.0;
	m_bForward = true;
	m_bPlay = false;
}

void CTimeline::Imgui_RenderEditor()
{
	ImGui::Text("TimeLine");
	ImGui::Checkbox("Loop", &m_bLoop);
	ImGui::SameLine();
	ImGui::Checkbox("Play", &m_bPlay);
	ImGui::SameLine();
	ImGui::Checkbox("Forward", &m_bForward);

	_float _fFrame = m_CurFrame;
	ImGui::InputFloat("Current Frame", &_fFrame);
	m_CurFrame = (_double)_fFrame;

	_float _fEnd = m_EndTime;
	ImGui::InputFloat("EndTime", &_fEnd);
	m_EndTime = (_double)_fEnd;

	
}

void CTimeline::Free()
{
	for (auto& floatCurvePair : m_vecFloatCurve)
		Safe_Release(floatCurvePair.second);
	m_vecFloatCurve.clear();

	for (auto& vec3CurvePair : m_vecVec3Curve)
		Safe_Release(vec3CurvePair.second);
	m_vecVec3Curve.clear();
}

/******************
 * CTimelineEx
 *****************/
void CTimelineEx::Tick(_double TimeDelta)
{
	if (m_pCurve == nullptr)
		return;
	if (m_bPlay == false)
		return;

	auto pCurve = static_cast<CCurveFloatImpl*>(m_pCurve);


	if (m_bForward)
		m_CurFrame += TimeDelta;
	else
		m_CurFrame -= TimeDelta;

	_bool bFin = false;
	if (m_CurFrame <= (_double)pCurve->GetMinX())
	{
		m_bPlay = false;
		m_CurFrame = (_double)pCurve->GetMinX();
		bFin = true;
	}
	else if (m_CurFrame >= (_double)pCurve->GetMaxX())
	{
		m_bPlay = false;
		m_CurFrame = (_double)pCurve->GetMaxX();
		bFin = true;
	}

	m_TickFunction(m_pCurve->GetValue(m_CurFrame));

	if (bFin)
	{
		if (m_FinFunction)
			m_FinFunction();
		if (m_bLoop)
		{
			if (m_bSwing)
				m_bForward = !m_bForward;
			m_CurFrame = m_bForward ? (_double)pCurve->GetMinX() : (_double)pCurve->GetMaxX();
			m_bPlay = true;
		}
	}
}

void CTimelineEx::Play()
{
	m_bForward = true;
	m_bPlay = true;
}

void CTimelineEx::PlayFromStart()
{
	if (m_pCurve)
	{
		auto pCurve = static_cast<CCurveFloatImpl*>(m_pCurve);
		m_CurFrame = (_double)pCurve->GetMinX();
		Play();
	}
}

void CTimelineEx::Stop()
{
	m_bPlay = false;
}

void CTimelineEx::Reverse()
{
	m_bForward = false;
	m_bPlay = true;
}

void CTimelineEx::ReverseFromEnd()
{
	if (m_pCurve)
	{
		auto pCurve = static_cast<CCurveFloatImpl*>(m_pCurve);
		m_CurFrame = (_double)pCurve->GetMaxX();
		Reverse();
	}
}	

void CTimelineEx::SetNewFrame(_double NewFrame)
{
	m_CurFrame = NewFrame;
}

void CTimelineEx::Reset()
{
	if (m_pCurve)
	{
		auto pCurve = static_cast<CCurveFloatImpl*>(m_pCurve);
		m_CurFrame = pCurve->GetMinX();
	}
	else
	{
		m_CurFrame = 0.0;
	}
	m_bForward = true;
	m_bPlay = false;
}

void CTimelineEx::Imgui_RenderEditor()
{
	ImGui::Text("TimeLineEx");
	ImGui::Checkbox("Loop", &m_bLoop);
	ImGui::SameLine();
	ImGui::Checkbox("Swing", &m_bSwing);
	ImGui::SameLine();
	ImGui::Checkbox("Play", &m_bPlay);
	ImGui::SameLine();
	ImGui::Checkbox("Forward", &m_bForward);

	if (ImGui::Button("Play")) PlayFromStart(); ImGui::SameLine();
	if (ImGui::Button("PlayFromStart")) PlayFromStart();

	if (ImGui::Button("Reverse")) Reverse(); ImGui::SameLine();
	if (ImGui::Button("ReverseFromEnd")) ReverseFromEnd();


	_float _fFrame = (_float)m_CurFrame;
	ImGui::InputFloat("Current Frame", &_fFrame);
	m_CurFrame = (_double)_fFrame;

	static char szCurve[MAX_PATH]{};
	ImGui::InputText("Switching Curve", szCurve, MAX_PATH);
	if (ImGui::Button("Switch to"))
	{
		if (auto pCurve = dynamic_cast<CCurveFloatImpl*>(GetCurve(szCurve)))
		{
			Safe_Release(m_pCurve);
			m_pCurve = pCurve;
			Safe_AddRef(m_pCurve);
		}
	}

	if (m_pCurve)
	{
		auto pCurve = static_cast<CCurveFloatImpl*>(m_pCurve);
		ImGui::Text("CurveTag: %s", pCurve->GetName());
		if (ImGui::CollapsingHeader("Show Curve Info"))
		{
			pCurve->Imgui_RenderEditor();
		}
	}
}

void CTimelineEx::Free()
{
	Safe_Release(m_pCurve);
}


CCurveFloat* CTimelineEx::GetCurve(const string& strCurveTag)
{
	return CCurveManager::GetInstance()->GetCurve(strCurveTag);
}

/******************
 * CSimpleTimeline
 *****************/
_bool CSimpleTimeline::Tick(_double TimeDelta, _float& fOut)
{
	if (m_pCurve == nullptr)
		return false;
	if (m_bPlay == false)
		return false;
	
	m_CurFrame += TimeDelta * m_Accel;

	_bool bFin = false;
	if (m_CurFrame <= (_double)m_pCurve->GetMinX())
	{
		m_bPlay = false;
		m_CurFrame = (_double)m_pCurve->GetMinX();
	}
	else if (m_CurFrame >= (_double)m_pCurve->GetMaxX())
	{
		m_bPlay = false;
		m_CurFrame = (_double)m_pCurve->GetMaxX();
		bFin = true;
	}

	fOut = m_pCurve->GetValue(m_CurFrame);

	if (bFin && m_FinFunction)
	{
		m_FinFunction();
	}

	return true;
}

void CSimpleTimeline::PlayFromStart()
{
	if (m_pCurve)
	{
		m_CurFrame = (_double)m_pCurve->GetMinX();
		m_bPlay = true;
	}
}

void CSimpleTimeline::Stop()
{
	m_bPlay = false;
}

void CSimpleTimeline::Imgui_RenderEditor()
{
	if (ImGui::CollapsingHeader("Simple Timeline"))
	{
		if (ImGui::Button("Play"))
		{
			PlayFromStart();
		}

		_float _fFrame = (_float)m_CurFrame;
		ImGui::InputFloat("Current Frame", &_fFrame);
		m_CurFrame = (_double)_fFrame;

		_float fAccel = (_float)m_Accel;
		ImGui::InputFloat("Current Accel", &fAccel);
		m_Accel = (_double)fAccel;

		static char szCurve[MAX_PATH]{};
		ImGui::InputText("Switching Curve", szCurve, MAX_PATH);
		if (ImGui::Button("Switch to"))
		{
			SetCurve(szCurve);
		}

		if (m_pCurve)
		{
			ImGui::Text("CurveTag: %s", m_pCurve->GetName());
			if (ImGui::CollapsingHeader("Show Curve Info"))
			{
				m_pCurve->Imgui_RenderEditor();
			}
		}
	}
}

void CSimpleTimeline::SetCurve(const string& strCurveTag)
{
	Safe_Release(m_pCurve);
	m_pCurve = CCurveManager::GetInstance()->GetCurve(strCurveTag);
	Safe_AddRef(m_pCurve);
}

void CSimpleTimeline::Free()
{
	Safe_Release(m_pCurve);
}

