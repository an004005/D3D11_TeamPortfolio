#include "stdafx.h"
#include "..\public\TimerHelper.h"

/******************
 * CTimerHelper
 *****************/
void CTimerHelper::Tick(_double TimeDelta)
{
	if (m_bPause) return;

	if (m_Time <= m_CurTime)
	{
		if (m_Callback) 
			m_Callback();
		Reset();
		if (m_bLoop == false) 
			Pause(true);
	}
	else
	{
		m_CurTime += TimeDelta;
	}
}

/******************
 * CCoolTimeHelper
 *****************/
CCoolTimeHelper::CCoolTimeHelper(_double Time, bool bFirstUse)
{
	Initialize(Time, bFirstUse);
}

void CCoolTimeHelper::Initialize(_double Time, bool bFirstUse)
{
	m_Time = Time;
	if (bFirstUse == false)
		m_RemainTime = m_Time;
}

void CCoolTimeHelper::Tick(_double TimeDelta)
{
	if (m_RemainTime > 0.0)
		m_RemainTime -= TimeDelta;
}

bool CCoolTimeHelper::CanUse() const
{
	return m_RemainTime <= 0.0;
}

bool CCoolTimeHelper::Use()
{
	if (CanUse())
	{
		if (m_Callback) m_Callback();
		m_RemainTime = m_Time;
		return true;
	}

	return false;
}
