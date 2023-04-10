#pragma once
#include "Level_StageDefault.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Hospital_1F : public CLevel_StageDefault
{
private:
	CLevel_Hospital_1F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Hospital_1F() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;

public:
	static CLevel_Hospital_1F* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
	
};

END