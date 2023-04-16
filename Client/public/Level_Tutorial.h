#pragma once
#include "Level_StageDefault.h"
#include "Client_Defines.h"
#include "SoundStore.h"
#include "HelperClasses.h"

BEGIN(Client)

class CLevel_Tutorial : public CLevel_StageDefault
{
private:
	CLevel_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tutorial() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;

private:
	CSoundStore m_BGM;
	CDoOnce m_BGMOnce;

	string m_MainSound = "Tutorial";

public:
	static CLevel_Tutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
	
};

END