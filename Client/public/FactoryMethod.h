#pragma once
#include "Base.h"

BEGIN(Client)

class CFactoryMethod : public CBase
{
public:
	CFactoryMethod();
	virtual ~CFactoryMethod();

	static HRESULT MakeEnermyPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static HRESULT MakeMonsterExPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static HRESULT MakePlayerPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static HRESULT MakeEffectPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static HRESULT MakeSAS_Portrait_Prototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);// �ҳ�
	static HRESULT MakeKineticPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static HRESULT MakeAIPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	static HRESULT MakeTriggerPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	//UI
	static HRESULT MakeUIPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	// NPC
	static HRESULT MakeNPCPrototypes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	// TeleportableWall
	static HRESULT MakeTWallPrototype(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual void Free() override;
};

END
