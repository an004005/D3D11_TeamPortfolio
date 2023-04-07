#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CFullUI final : public CUI
{
private:
	CFullUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFullUI(const CFullUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

public:
	static CFullUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END