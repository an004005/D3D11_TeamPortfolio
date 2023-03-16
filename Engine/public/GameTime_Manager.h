#pragma once
#include "Base.h"
#include "Timeline.h"

BEGIN(Engine)

class CGameTime_Manager : public CBase
{
	DECLARE_SINGLETON(CGameTime_Manager)
public:
	CGameTime_Manager();
	virtual ~CGameTime_Manager() = default;

public:
	_double GetTimeRatio() { return (_double)m_TimeRatio; }
	void Tick(_double TimeDelta);

	void ResetTimeRatio();
	void SetTimeRatioCurve(const string& strCurveTag, _bool bStay = false, const vector<wstring>* ExceptLayers = nullptr);
	void SetTimeRatio(_float fTimeRatio, const vector<wstring>* ExceptLayers = nullptr);

	void Imgui_Render();

private:
	_float m_TimeRatio = 1.0;
	CSimpleTimeline m_Timeline;

public:
	virtual void Free() override;
};

END