#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CDistanceUI final : public CUI
{
private:
	CDistanceUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDistanceUI(const CDistanceUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

public:
	static CDistanceUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END