#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CItem_GaugeUI final : public CUI
{
private:
	CItem_GaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_GaugeUI(const CItem_GaugeUI& rhs);

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
	void	Set_CooldownTimeStart() {
		m_bCooldownTimeStart = true;
	}

private:
	void	Gauge_Tick(const _double & dTimeDelta);

private:
	_bool	m_bCooldownTimeStart = { false };

public:
	static CItem_GaugeUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END