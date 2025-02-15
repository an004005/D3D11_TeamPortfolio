#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Timeline.h"

/************************************************
 *												*
 *												*
 * Emissive = g_float_2							*
 * Dissolve = g_float_3							*
 * 												*
 *												*
 *												*
 ************************************************/


BEGIN(Engine)
class CRenderer;
class CCurveFloatImpl;
END

BEGIN(Client)
class CEffectGroup :	public CGameObject
{
public:
	enum FINISHFUNC { FUNC_PLAYFROMSTART, FUNC_RESET, FUNC_STOP, FUNC_REVERSE, FUNC_DELETE, FUNC_END};
	enum CURVETYPE { CURVE_SCALE_ALL, CURVE_SCALE_Y, CURVE_SCALE_X, CURVE_FLOATS_0, CURVE_FLOATS_1, CURVE_FLOATS_2, CURVE_FLOATS_3, CURVE_FLOATS_4, CURVE_FLOATS_5, CURVE_FLOATS_6, CURVE_FLOATS_7, CURVE_INTROTIME, CURVE_OUTROTIME, CURVE_COLORCHANGE,CURVE_INTS_0, CURVE_END };

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
	void		Imgui_RenderEffectSource(_int iSelectEffect);

	void		SetStop();
	void		SetPlay();
	void		SetReverse();
	_bool		CheckPlay();
	void		VisibleUpdate();
	void		Set_Transform(_fmatrix matSocket);

public:
	void		Start_EffectWork();

	void		Start_OnlyPos_Pivot(_float4 vPos, _float4x4 Pivot, _bool trueisUpdate = false);
	void		Start_AttachOnlyPos(_float4 vPos, _bool trueisUpdate = false);
	void		Start_NoAttachPivot(CGameObject* pOwner, _float4x4 PivotMatrix, _bool trueisUpdate = false, _bool trueisRemoveScale = false);
	void		Start_NoAttach(CGameObject* pOwner, _bool trueisUpdate = false, _bool trueisRemoveScale = false);

	void		Start_Attach(CGameObject* pOwner, string BoneName, _bool trueisUpdate = false, _bool trueisRemoveScale = false);
	void		Start_AttachPivot(CGameObject* pOwner, _float4x4 PivotMatrix, string BoneName,_bool usepivot = false, _bool trueisUpdate = false, _bool trueisRemoveScale = false);
	// void		Start_NoAttach(CGameObject* pOwner, _bool trueisUpdate = false);
	// void		Start_Attach(CGameObject* pOwner, string BoneName = "", _bool trueisUpdate = false);
	// void		Start_AttachPivot(CGameObject* pOwner, _float4x4 PivotMatrix, string BoneName,_bool usepivot = false, _bool trueisUpdate = false);
	void		Start_AttachPosition(CGameObject* pOwner, _float4 vPosition, _float4 vDirection, _bool trueisUpdate = false);
	void		Start_AttachPositionMove(CGameObject* pOwner, _float4 vPosition, _float4 vDirection, _bool trueisUpdate = false);

	void		Start_AttachSword(CGameObject* pWeapon, _bool trueisUpdate = false);
	void		Start_AttachMove(CGameObject* pOwner, string BoneName, _float4 vDirection, _bool trueisUpdate = false, _bool trueisRemoveScale = false);
	void		Start_AttachPivotMove(CGameObject* pOwner, _float4x4 PivotMatrix, string BoneName, _float4 vDirection, _bool usepivot = false, _bool trueisUpdate = false, _bool trueisRemoveScale = false);
	void		Start_AttachPosition_Scale(CGameObject* pOwner, _float4 vPosition, _float4 vDirection, _float4 vScale, _bool trueisUpdate = false);

	void		Start_Attach_Vector(CGameObject* pOwner, _fvector vVector, string BoneName, _bool trueisUpdate = false);
	void		Start_AttachPivot_Vector(CGameObject* pOwner, _fvector vVector, _float4x4 PivotMatrix, string BoneName, _bool usepivot = false, _bool trueisUpdate = false);

	void		Call_Event();
	class CEffectSystem* Get_FirstEF();
public:
	// For Graph
	// void Tick_Scale_All( _float fValue);
	// void Tick_Floats_0(_float fValue);
	// void Tick_Floats_1(_float fValue);
	// void Tick_Floats_2(_float fValue);
	// void Tick_Floats_3(_float fValue);
	// void Tick_IntroTime(_float fValue);
	// void Tick_OutroTime(_float fValue);

	void LoadAndSetCurve_First(Json* json);
	void LoadAndSetCurve_Second(Json* json);
	void LoadAndSetCurve_Third(Json* json);
	void LoadAndSetCurve_Fourth(Json* json);
	void LoadAndSetCurve_Fifth(Json* json);

	// void Tick_Float4x4s_Value(_uint iParamIndex, _float4x4 fValue);

	class CEffectSystem* GetFirstEffect() { return m_pFirst_EffectSystem; }
	class CEffectSystem* GetSecondEffect() { return m_pSecond_EffectSystem; }
	class CEffectSystem* GetThirdEffect() { return m_pThird_EffectSystem; }
	class CEffectSystem* GetFourthEffect() { return m_pFourth_EffectSystem; }
	class CEffectSystem* GetFifthEffect() { return m_pFifth_EffectSystem; }

	void Set_GroupVisible(const _bool bVisible) { m_bGroupVisible = bVisible; }

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
	_float4 m_vEFGroupPos = { 0.f,0.f,0.f,0.f };

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
	_bool	m_bUseOnlyPosition = false;
	_float4 m_vPosition = _float4::Zero;
	_bool	m_bUpdate = false;
	_bool	m_bUsePivot = false;
	_bool	m_bRemoveScale = false;
	string m_BoneName = "";
	_float4x4 m_PivotMatrix = XMMatrixIdentity();

	_matrix m_OriginMoveMatrix = XMMatrixIdentity();
	_float4 m_vMoveDir = _float4::Zero;

	_bool	m_bGroupVisible = { true };



private:
	CGameObject* m_pAttachWeapon = nullptr;

private:
	_float	m_fEndTime = 0.f;
	_int	m_iSelectFinishFunc = 0;
	char*	m_szFuncName[FUNC_END] = { "PlayFromStart",  "Reset", "Stop", "Reverse", "Delete" };
	char*	m_szCurveTag[CURVE_END] = { "ObjectScale_All", "ObjectScale_Y","ObjectScale_X", "Floats_0", "Floats_1", "Floats_2", "Floats_3","Floats_4","Floats_5","Floats_6", "Floats_7",  "Intro_Time", "Outro_Time", "Color_Change", "Ints_0" };

	unordered_map<string, class CEffectSystem*> m_mapEffectSystemTag;

	list<char*> ppEffectTag;
public:
	static CEffectGroup*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void Free() override;
};




END
