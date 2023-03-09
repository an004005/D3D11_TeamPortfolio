#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

/******************
 * CCurveFloat
 *****************/
class ENGINE_DLL CCurveFloat abstract : public CBase
{
public:
	virtual _float GetValue(_double Frame)
	{
		return 0.f;
	}
};

/******************
 * CCurveVector3
 *****************/
class ENGINE_DLL CCurveVector3 abstract : public CBase
{
public:
	virtual _float3 GetValue(_double Frame)
	{
		return _float3();
	}
};

/******************
 * CCurveFloatMap
 *****************/
class ENGINE_DLL CCurveFloatMap abstract : public CBase
{
public:
	virtual map<string, _float> GetValue(_double Frame)
	{
		return {};
	}
};

/******************
 * CCurveFloatLinearRatio
 *****************/
class ENGINE_DLL CCurveFloatLinearRatio final : public CCurveFloat
{
private:
	CCurveFloatLinearRatio() = delete;
	CCurveFloatLinearRatio(_float fLength) : m_fLength(fLength){}

public:
	virtual _float GetValue(_double Frame) override
	{
		return static_cast<_float>(Frame) / m_fLength;
	}

private:
	const _float m_fLength;

public:
	static CCurveFloatLinearRatio* Create(_float fLength)
	{
		return new CCurveFloatLinearRatio(fLength);
	}
	virtual void Free() override{}
};

/******************
 * CCurveFloatSmoothStepRatio
 *****************/
class ENGINE_DLL CCurveFloatSmoothStepRatio final : public CCurveFloat
{
private:
	CCurveFloatSmoothStepRatio() = delete;
	CCurveFloatSmoothStepRatio(_float fLength) : m_fLength(fLength){}

public:
	virtual _float GetValue(_double Frame) override;

private:
	const _float m_fLength;

public:
	static CCurveFloatSmoothStepRatio* Create(_float fLength)
	{
		return new CCurveFloatSmoothStepRatio(fLength);
	}
	virtual void Free() override{}
};

/******************
 * CTimeline
 *****************/
class ENGINE_DLL CTimeline final : public CBase
{
public:
	~CTimeline()
	{
		Free();
	}

public:
	void Tick(_double TimeDelta);

	void Play();
	void PlayFromStart();
	void Stop();
	void Reverse();
	void ReverseFromEnd();
	void SetNewFrame(_double NewFrame);
	void Reset();

	void SetTimelineLength(_double TimeLength) { m_EndTime = TimeLength; }
	_bool IsForward() const { return m_bForward; }

	void Imgui_RenderEditor();

	_bool IsPlay() { return m_bPlay; }
	_bool Check_ReverseFin() { return m_bReverseFinish; }




	template<typename T>
	void SetFinishFunction(T* obj, void (T::*memFunc)())
	{
		Assert(obj != nullptr);
		m_FinFunction = [obj, memFunc]() { (obj->*memFunc)(); };
	}

	template<typename T>
	void SetCurve(T* obj, void (T::*memFunc)(_float), CCurveFloat* curve)
	{
		Assert(curve != nullptr);
		Assert(obj != nullptr);

		auto callback = [obj, memFunc](_float f) { (obj->*memFunc)(f); };
		m_vecFloatCurve.push_back({callback, curve});
	}

	template<typename T>
	void SetCurve(T* obj, void (T::*memFunc)(_float3), CCurveVector3* curve)
	{
		Assert(curve != nullptr);
		Assert(obj != nullptr);

		auto callback = [obj, memFunc](_float3 f3) { (obj->*memFunc)(f3); };
		m_vecVec3Curve.push_back({callback, curve});
	}

private:
	_double m_EndTime = 0.0;
	_double m_CurFrame = 0.0;
	_bool m_bForward = true;
	_bool m_bPlay = false;
	_bool m_bLoop = false;
	_bool m_bFreed = false;
	_bool m_bReverseFinish = false;

	function<void()> m_FinFunction = nullptr;
	vector<pair<function<void(_float)>, CCurveFloat*>> m_vecFloatCurve;
	vector<pair<function<void(_float3)>, CCurveVector3*>> m_vecVec3Curve;

public:
	virtual void Free() override;
};
END
