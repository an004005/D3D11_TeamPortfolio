#include "stdafx.h"
#include "..\public\Timeline.h"

#include "GameInstance.h"
#include "MathUtils.h"

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
		m_bReverseFinish = true;
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

