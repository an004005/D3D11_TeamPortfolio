#pragma once
#include "Base.h"

BEGIN(Client)

class CFactoryMethod : public CBase
{
public:
	CFactoryMethod();
	virtual ~CFactoryMethod();

	static HRESULT MakeEnermyPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static HRESULT MakePlayerPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static HRESULT MakeEffectPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual void Free() override;
};

END
