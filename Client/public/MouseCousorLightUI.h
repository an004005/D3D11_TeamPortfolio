#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMouseCousorLightUI final : public CUI
{
private:
	CMouseCousorLightUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMouseCousorLightUI(const CMouseCousorLightUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_MouseLight(const _bool	bLight) {
		m_fMouseLight = bLight;
		m_bVisible = bLight;
	}

private:
	_bool	m_fMouseLight = { false };
	_bool	m_fAlphaDown = { false };
	_float	m_fAlpha = { 0.0f };

public:
	static CMouseCousorLightUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END