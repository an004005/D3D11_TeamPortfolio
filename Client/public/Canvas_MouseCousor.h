#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_MouseCousor : public CCanvas
{
protected:
	CCanvas_MouseCousor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_MouseCousor(const CCanvas_MouseCousor& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;

public:
	static CCanvas_MouseCousor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END