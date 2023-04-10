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
		//if (m_fRatio - 1.0f <= 0.000001f)	
		if (m_fRatio == 1.0f)
			return true;		// 사용 가능
		else
			return false;	// 사용 불 가능
	}
	_bool	Get_Light() {
		return m_bLight;
	}

	void	Set_CooldownTimeStart() {
		m_fRatioDown = true;
		m_bCooldownTimeStart = true;
	}

	void	Set_Color(const _float4& fColor) {
		m_tParams.Float4s[0] = fColor;
	}

private:
	_bool	m_bCooldownTimeStart = { false };
	_bool	m_fRatioDown = { false };
	_float	m_fRatio = { 1.0f };

	_bool	m_bLight = { false };

public:
	static CItem_GaugeUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END