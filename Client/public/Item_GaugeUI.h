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
	
public:
	_bool	Get_ItemUseStatuse() {
		if (1.0f == m_tParams.Floats[0])
			return true;
		else
			return false;
	}

	void	Set_CooldownTimeStart() {
		m_fRatioDown = true;
		m_bCooldownTimeStart = true;
	}

private:
	_bool	m_bCooldownTimeStart = { false };
	_bool	m_fRatioDown = { false };
	_float	m_fRatio = { 0.0f };

public:
	static CItem_GaugeUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END