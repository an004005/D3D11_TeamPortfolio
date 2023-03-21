#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_PickUI final : public CUI
{
private:
	CMain_PickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_PickUI(const CMain_PickUI& rhs);

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
	_bool	Get_OnButton() {
		return m_vOnButton;
	}
	void	Set_OnButton() {
		m_vOnButton = false;
	}

	_bool	Get_OnAlpha() {
		return m_vOnAlpha;
	}
	void	Set_InitializeAlpha() {
		m_vOnAlpha = true;
	}
	void	Set_OnAlpha() {
		m_vOnAlpha = false;
	}

	void	Set_ColorType(const _uint & iType = 0) {
		m_vColorType = iType;
	}

private:
	void	CurrentPick(const _double & TimeDelta);

private:
	_bool	m_vOnButton = { false };
	_bool	m_vOnAlpha = { false };
	_uint	m_vColorType = { 0 };

	_bool	m_vAlphaDwon = { false };
	_float	m_fAlpha = { 0.0f };

public:
	static CMain_PickUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END