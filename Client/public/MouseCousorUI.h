#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMouseCousorUI final : public CUI
{
private:
	CMouseCousorUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMouseCousorUI(const CMouseCousorUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;

private:
	class CMouseCousorLightUI*	m_pMouseLight = { nullptr };

public:
	static CMouseCousorUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END