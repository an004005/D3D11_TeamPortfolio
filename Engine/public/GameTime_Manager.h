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
	// 일시적으로 시간 흐름을 변하면서 조절할때(히트렉, 포트레잇)
	void SetTimeRatioCurve(const string& strCurveTag, _bool bStay = false, const vector<wstring>* ExceptLayers = nullptr);

	void ResetDefaultTimeRatio();
	// 일정 기간동안 시간 흐름을 다르게 할때(초고속)
	void SetTimeRatio(_float fTimeRatio, const vector<wstring>* ExceptLayers = nullptr);

	// 특정 레이어의 시간흐름을 조절할 때(드라이브모드)
	void SetLayerTimeRatio(_float fLayerTimeRatio, const wstring& strLayerTag);

	void ClearAllTimeRatio();

	void Imgui_Render();

private:
	_float m_TimeRatio = 1.0;
	CSimpleTimeline m_Timeline;

	// 
	_float m_DefaultTimeRatio = 1.f;
	vector<wstring> m_DefaultExceptLayers;

public:
	virtual void Free() override;
};

END