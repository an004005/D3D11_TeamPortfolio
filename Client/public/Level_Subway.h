#pragma once
#include "Level_StageDefault.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Subway : public CLevel_StageDefault
{
private:
	CLevel_Subway(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Subway() = default;

public:
	virtual HRESULT Initialize() override;

public:
	static CLevel_Subway* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
	
};

END