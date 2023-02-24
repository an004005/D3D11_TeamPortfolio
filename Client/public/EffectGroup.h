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
	void Tick_Scale(_uint iParamIndex, _float fValue);
	void Tick_IntroDissolve(_float fValue);
	void Tick_OutroDissolve(_float fValue);
	void Tick_ColorChange(_float fValue);
	void Tick_EmissiveChange(_float fValue);
	void Tick_IntroTime(_float fValue);
	void Tick_OutroTime(_float fValue);


	// void Tick_Float4x4s_Value(_uint iParamIndex, _float4x4 fValue);

protected:
	void AddEmptyCurve(string strCurveName);


protected:
	CRenderer* m_pRendererCom = nullptr;

	CTimeline m_Timeline;
	cmap<CCurveFloatImpl*> m_Curves;

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

public:
	static CEffectGroup*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void Free() override;
};
END