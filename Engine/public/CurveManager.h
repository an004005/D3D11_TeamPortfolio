#pragma once
#include "Base.h"

BEGIN(Engine)

class CCurveManager : public CBase
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

private:
	unordered_map<string, class CCurveFloatImpl*> m_Curves;
	string m_strCurveJsonPath;

public:
	virtual void Free() override;
};

END