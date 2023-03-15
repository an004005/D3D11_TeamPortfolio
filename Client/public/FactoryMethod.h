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
	static HRESULT MakeSAS_Portrait_Prototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);// ºÒ³à

public:
	virtual void Free() override;
};

END
