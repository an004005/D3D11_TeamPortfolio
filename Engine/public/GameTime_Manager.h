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
	// �Ͻ������� �ð� �帧�� ���ϸ鼭 �����Ҷ�(��Ʈ��, ��Ʈ����)
	void SetTimeRatioCurve(const string& strCurveTag, _bool bStay = false, const vector<wstring>* ExceptLayers = nullptr);

	void ResetDefaultTimeRatio();
	// ���� �Ⱓ���� �ð� �帧�� �ٸ��� �Ҷ�(�ʰ��)
	void SetTimeRatio(_float fTimeRatio, const vector<wstring>* ExceptLayers = nullptr);

	// Ư�� ���̾��� �ð��帧�� ������ ��(����̺���)
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