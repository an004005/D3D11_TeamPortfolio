#pragma once
#include "UI.h"
#include "Client_Defines.h"
#include "Shader.h"

BEGIN(Engine)
class CVIBuffer;
END

BEGIN(Client)

class CUIEx : public CUI
{
protected:
	CUIEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIEx(const CUIEx& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

	ShaderParams& GetParams() { return m_tParams; }
	void SetText(const wstring& strText) { m_strText = strText; }

	void SetOnClick(const function<void()>& OnClick) { m_OnClick = OnClick; }
	void SetOnClicking(const function<void()>& OnClicking) { m_OnClicking = OnClicking; }
	void SetOnHover(const function<void()>& OnHover) { m_OnHover = OnHover; }
	void SetOffHover(const function<void()>& OffHover) { m_OffHover = OffHover; }

private:
	CShader* m_pShaderCom = nullptr;
	CVIBuffer* m_pBuffer = nullptr;
	ShaderParams m_tParams;

	_bool m_bUseShader = true;
	_bool m_bFitParent = false;

	wstring m_strText;
	wstring m_strFontTag;
	_float2 m_vTextPos{0.f, 0.f};
	_float2 m_vTextScale{1.f, 1.f};
	_float4 m_vTextColor{1.f, 1.f, 1.f, 1.f};

	function<void()> m_OnClick = nullptr;
	function<void()> m_OnClicking = nullptr;
	function<void()> m_OnHover = nullptr;
	function<void()> m_OffHover = nullptr;

public:
	DECLARE_CREATE(CUIEx)
	DECLARE_CLONE(CUIEx)
	virtual void Free() override;
};

END