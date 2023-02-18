#pragma once
#include "Timeline.h"

BEGIN(Engine)

struct FloatMapKeyFrame
{
	_double Time;
	map<string, _float> CurveData;
};

class CCurveFloatMapImpl : public CCurveFloatMap
{
private:
	CCurveFloatMapImpl();
	virtual ~CCurveFloatMapImpl() = default;

public:
	virtual map<string, _float> GetValue(_double Frame) override;
	void SaveToJson(Json& json);
	void LoadFromJson(const Json& json);
	void Imgui_RenderEditor();

	void AddKey(const string& key);
	void DeleteKey(const string& key);
	// void EditKeyFrame(c)

private:
	vector<FloatMapKeyFrame> m_KeyFrames;
	map<string, _float> m_DefaultKeyFrame;

	FloatMapKeyFrame m_LastUsedValue;
public:
	virtual void Free() override;
	// static CCurveFloatMapImpl* Create();
};


class ENGINE_DLL CCurveFloatImpl : public CCurveFloat
{
private:
	CCurveFloatImpl();
	virtual ~CCurveFloatImpl() = default;

public:
	virtual _float GetValue(_double Frame) override;
	void SaveToJson(Json& json);
	void LoadFromJson(const Json& json);
	void Imgui_RenderEditor();

	const char* GetName() { return m_szName; }

private:
	// ImVec2* m_pKeyFrames = nullptr;
	ImVec2 m_KeyFrames[30];
	const _int m_iMaxPoint = 30;
	char m_szName[MAX_PATH] = "NonNamed";
	_int m_iSelection = -1;

	ImVec2 m_RangeMin{0.f, 0.f};
	ImVec2 m_RangeMax{ 1.f, 1.f };

public:
	virtual void Free() override;
	static CCurveFloatImpl* Create(Json* json = nullptr);
	static CCurveFloatImpl* Create(const string& strName);
};

END