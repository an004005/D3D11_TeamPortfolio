#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_OnePickUI final : public CUI
{
private:
	CMain_OnePickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_OnePickUI(const CMain_OnePickUI& rhs);

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
		return m_bOnButton;
	}

private:
	void	CurrentPick(const _double & TimeDelta);

private:
	_bool	m_bOnButton = { false };

	_bool	m_bAlphaDwon = { false };
	_float	m_fAlpha = { 0.0f };

public:
	static CMain_OnePickUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END