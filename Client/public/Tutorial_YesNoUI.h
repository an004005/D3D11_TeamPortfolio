#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CTutorial_YesNoUI final : public CUI
{
private:
	CTutorial_YesNoUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorial_YesNoUI(const CTutorial_YesNoUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	_bool	Get_InputYes() {
		return m_bYes;
	}
	_bool	Get_InputNo() {
		return m_bNo;
	}
	_bool	Get_Invisible() {
		return m_bInvisible;
	}

	void	Set_Invisible() {
		m_bInvisible = false;
	}

	// 아이콘
	_int	Get_OneRenderCount() {
		return m_iOne_RenderCount;
	}
	_bool	Get_TwoRnederEnd() {
		return m_bTowRnederEnd;
	}
	_bool	Get_AlphaEnd() {
		return m_bAlphaEnd;
	}

	void	Set_TwoRneder() {
		++m_iOne_RenderCount;
		m_bTwo_Rneder = true;
	}
	void	Set_OneTwoAlpha() {
		m_bTowRnederEnd = false;
		m_bOneTwo_Alpha = true;
	}
	
	void	Set_OneReset() {
		m_bAlphaEnd = false;
		m_dOne_TimeAcc = 0.0;
		m_iOne_RenderCount = 0;
		m_tParams.Float4s[0].w = 1.0f;
	}
	void	Set_TwoReset() {
		m_bAlphaEnd = false;
		m_bTwo_Rneder = false;
		m_bTowRnederEnd = false;
		m_tParams.Float4s[0].w = 0.0f;
	}

	void	Set_ZeroShader() {
		m_bZero_Shader = true;
	}

private:
	void	Object_Tick(const _double & TimeDelta);

	void	ObjectZero_Tick(const _double & TimeDelta);

	void	ObjectOne_Tick(const _double & TimeDelta);
	void	ObjectTwo_Tick(const _double & TimeDelta);
	void	Icon_Alpha(const _double & TimeDelta);

	void	ObjectYes_Tick();
	void	ObjectNo_Tick();
	void	InvisibleBox();

private:
	_int	m_iObjectNumber = { 0 };

	_bool	m_bYes = { false };
	_bool	m_bNo = { false };
	_bool	m_bInvisible = { false };

	// 아이콘
	// 0
	_bool	m_bGlowUp = { false };
	_bool	m_bGlowDown = { false };
	_bool	m_bZero_Shader = { false };

	// 1
	_int	m_iOne_RenderCount = { 0 };
	_double m_dOne_TimeAcc = { 0.0 };

	// 2
	_bool	m_bTowRnederEnd = { false };
	_bool	m_bTwo_Rneder = { false };

	// 1, 2
	_bool	m_bAlphaEnd = { false };
	_bool	m_bOneTwo_Alpha = { false };
	_float	m_fAlpha = { 1.0f };

public:
	static CTutorial_YesNoUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END