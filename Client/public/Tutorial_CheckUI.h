#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CTutorial_CheckUI final : public CUI
{
private:
	CTutorial_CheckUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorial_CheckUI(const CTutorial_CheckUI& rhs);

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
	void	Set_OnTutorial() {
		if (false == m_bOffTutorial)
			m_bOnTutorial = true;
	}
	void	Set_OffTutorial() {
		if (false == m_bOnTutorial)
			m_bOffTutorial = true;
	}

private:
	void	Input(const _double & dTImeDelta);
	_bool	Change(const _double & dTImeDelta, const _bool & bTutorial);

private:
	_bool	m_bOnTutorial = { false };
	_bool	m_bSizeGrowLess = { false };
	_bool	m_bOffTutorial = { false };

	_float	m_fAlpha = { 0.0f };
	_float	m_fStartSizeX = { 0.0f };
	_float	m_fChangeSizeX = { 0.0f };

public:
	static CTutorial_CheckUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END