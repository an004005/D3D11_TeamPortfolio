#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCurveManager : public CBase
{
	DECLARE_SINGLETON(CCurveManager)
public:
	CCurveManager();
	virtual ~CCurveManager() = default;

public:
	class CCurveFloatImpl* GetCurve(const string& strCurveTag);

	void LoadCurves(const char* pCurveJsonPath);
	void SaveCurves();
	void Imgui_Render();

	_float2 GetCurveEditorSize() const { return m_CurveEditorSize; }

private:
	unordered_map<string, class CCurveFloatImpl*> m_Curves;
	string m_strCurveJsonPath;
	_float2 m_CurveEditorSize{400.f, 300.f};

public:
	virtual void Free() override;
};

END