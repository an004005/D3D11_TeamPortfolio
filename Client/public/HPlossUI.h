#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CHPlossUI final : public CUI
{
private:
	CHPlossUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHPlossUI(const CHPlossUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

private:
	_bool	m_fAlphaDown = { false };
	_float	m_fAlpha = { 0.0f };

public:
	static CHPlossUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END