#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Timeline.h"

BEGIN(Engine)
class CRenderer;
class CCurveFloatImpl;
END

BEGIN(Client)
class CEffectGroup :	public CGameObject
{
public:
	enum FINISHFUNC { FUNC_PLAYFROMSTART, FUNC_RESET, FUNC_STOP, FUNC_END};
	enum CURVETYPE { CURVE_SCALE, CURVE_INTRODISSOLVE, CURVE_OUTRODISSOLVE, CURVE_COLORCHANGE, CURVE_EMISSIVECHANGE, CURVE_INTROTIME, CURVE_OUTROTIME, CURVE_END };

protected:
	CEffectGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectGroup(const CEffectGroup& rhs);
	virtual ~CEffectGroup() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	void		Load_EffectSystem();
	void		Imgui_RenderEffectSource();

public:
	// For Graph
	void Tick_Scale( _float fValue);
	void Tick_IntroDissolve(_float fValue);
	void Tick_OutroDissolve(_float fValue);
	void Tick_ColorChange(_float fValue);
	void Tick_EmissiveChange(_float fValue);
	void Tick_IntroTime(_float fValue);
	void Tick_OutroTime(_float fValue);

	void LoadAndSetCurve_First(Json* json);
	void LoadAndSetCurve_Second(Json* json);
	void LoadAndSetCurve_Third(Json* json);
	void LoadAndSetCurve_Fourth(Json* json);
	void LoadAndSetCurve_Fifth(Json* json);

	// void Tick_Float4x4s_Value(_uint iParamIndex, _float4x4 fValue);

protected:
	void AddEmptyCurve(string strCurveName);

	// For NumEffectGroup
	void AddEmptyCurve_ForFirst(string strCurveName);
	void AddEmptyCurve_ForSecond(string strCurveName);
	void AddEmptyCurve_ForThird(string strCurveName);
	void AddEmptyCurve_ForFourth(string strCurveName);
	void AddEmptyCurve_ForFifth(string strCurveName);

	void Add_TargetCurve(_uint iSelectEffect, string strTargetCurveName);
protected:
	CRenderer* m_pRendererCom = nullptr;

	CTimeline m_Timeline;
	cmap<CCurveFloatImpl*> m_Curves;

	cmap<CCurveFloatImpl*> m_FirstEffect_Curves;
	cmap<CCurveFloatImpl*> m_SecondEffect_Curves;
	cmap<CCurveFloatImpl*> m_ThirdEffect_Curves;
	cmap<CCurveFloatImpl*> m_FourthEffect_Curves;
	cmap<CCurveFloatImpl*> m_FifthEffect_Curves;

	cmap<cmap<CCurveFloatImpl*>> m_mapEffectCurves; // 파일경로와 이펙트 커브를 보관한다. 


private:
	string		m_First_EffectDirectory = "";
	string		m_Second_EffectDirectory = "";
	string		m_Third_EffectDirectory = "";
	string		m_Fourth_EffectDirectory = "";
	string		m_Fifth_EffectDirectory = "";

	class CEffectSystem* m_pCurSelect_Effect = nullptr;

	class CEffectSystem* m_pFirst_EffectSystem = nullptr;
	class CEffectSystem* m_pSecond_EffectSystem = nullptr;
	class CEffectSystem* m_pThird_EffectSystem = nullptr;
	class CEffectSystem* m_pFourth_EffectSystem = nullptr;
	class CEffectSystem* m_pFifth_EffectSystem = nullptr;

private:
	_float	m_fEndTime = 0.f;
	_int	m_iSelectFinishFunc = 0;
	char*	m_szFuncName[FUNC_END] = { "PlayFromStart",  "Reset", "Stop" };
	char*	m_szCurveTag[CURVE_END] = { "ObjectScale", "Intro_Dissolve", "Outro_Dissolve", "Color_Change", "Emissive_Change", "Intro_Time", "Outro_Time" };

	map<string, class CEffectSystem*> m_mapEffectSystemTag;

	list<char*> ppEffectTag;
public:
	static CEffectGroup*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void Free() override;
};




END
