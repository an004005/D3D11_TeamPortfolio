#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

/******************
 * CTimerHelper
 *****************/
class ENGINE_DLL CTimerHelper final
{
public:
	CTimerHelper() = default;

	template<typename T>
	void Initialize(T* obj, void (T::*memFunc)(), _double Time, _bool bLoop = true)
	{
		m_Callback = [obj, memFunc]() { (obj->*memFunc)(); };
		m_Time = Time;
		m_bLoop = bLoop;
	}

	void Initialize(function<void()> Callback, _double Time, _bool bLoop = true)
	{
		m_Callback = Callback;
		m_Time = Time;
		m_bLoop = bLoop;
	}

	void Tick(_double TimeDelta);
	void Pause(_bool bPause) { m_bPause = bPause; }
	void Reset() { m_CurTime = 0.0; m_bPause = false; }
	_bool IsPause() const { return m_bPause; }
	_float GetCurPlayTime() const { return (_float)m_CurTime; }
	_float GetPlayRatio() const { return (_float)m_CurTime / (_float)m_Time; }
	
private:
	function<void()> m_Callback = nullptr;
	_double m_Time = 0.0;
	_double m_CurTime = 0.0;
	_bool m_bLoop = false;
	_bool m_bPause = true;
};

/******************
 * CCoolTimeHelper
 *****************/
class ENGINE_DLL CCoolTimeHelper final
{
public:
	CCoolTimeHelper() = default;
	CCoolTimeHelper(_double Time, bool bFirstUse = true);

	void Initialize(_double Time, bool bFirstUse = true);
	void Tick(_double TimeDelta);
	bool CanUse() const;
	bool Use();

	template<typename T>
	void SetUseFunction(T* obj, void (T::*memFunc)())
	{
		m_Callback = [obj, memFunc]() { (obj->*memFunc)(); };
	}

	_double GetRemainTime() const { return m_RemainTime; }
	_float GetRemainRatio() const { return static_cast<_float>(m_RemainTime) / static_cast<_float>(m_Time); }

private:
	function<void()> m_Callback = nullptr;
	_double m_Time = 0.0;
	_double m_RemainTime = 0.0;
};

END