#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_BrainGaugeUI final : public CUI
{
private:
	CMain_BrainGaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_BrainGaugeUI(const CMain_BrainGaugeUI& rhs);

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
	_bool	Get_GaugeFull() {
		return m_bGaugeFull;
	}
	void	Set_GaugeFull() {
		m_bGaugeFull = false;
	}

	void	Set_ChargeGauge(const _bool bGauge) {
		m_bVisible = bGauge;
		m_bChargeGauge = bGauge;
	}
	
private:
	_bool	m_bChargeGauge = { false };
	_bool	m_bGaugeFull = { false };

public:
	static CMain_BrainGaugeUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END