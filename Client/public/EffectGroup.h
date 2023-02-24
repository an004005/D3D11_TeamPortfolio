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
protected:
	

protected:
	CEffectGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectGroup(const CEffectGroup& rhs);
	virtual ~CEffectGroup() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;

	void		LoadEffectData();
	void		Imgui_RenderEffectSource();
public:
	void Tick_Scale(_uint iParamIndex, _float fValue);
	// void Tick_Ints_Value(_uint iParamIndex, _int iValue);
	void Tick_Floats_Value(_uint iParamIndex, _float fValue);
	// void Tick_Float2s_Value(_uint iParamIndex, _float2 fValue);
	void Tick_Float4s_Value(_uint iParamIndex, _float4 fValue);
	// void Tick_Float4x4s_Value(_uint iParamIndex, _float4x4 fValue);

protected:
	void AddEmptyCurve(string strCurveName);


protected:
	CRenderer* m_pRendererCom = nullptr;

	CTimeline m_Timeline;
	cmap<CCurveFloatImpl*> m_Curves;

private:
	string		m_First_EffectProtoTag = "";
	string		m_Second_EffectProtoTag = "";
	string		m_Third_EffectProtoTag = "";
	string		m_Fourth_EffectProtoTag = "";
	string		m_Fifth_EffectProtoTag = "";

	class CEffectSystem* m_pCurSelect_Effect = nullptr;

	class CEffectSystem* m_pFirst_EffectSystem = nullptr;
	class CEffectSystem* m_pSecond_EffectSystem = nullptr;
	class CEffectSystem* m_pThird_EffectSystem = nullptr;
	class CEffectSystem* m_pFourth_EffectSystem = nullptr;
	class CEffectSystem* m_pFifth_EffectSystem = nullptr;

public:
	static CEffectGroup*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void Free() override;
};
END