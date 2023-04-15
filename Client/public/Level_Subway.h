#pragma once
#include "Level_StageDefault.h"
#include "Client_Defines.h"
#include "SoundStore.h"
#include "HelperClasses.h"

BEGIN(Client)

class CLevel_Subway : public CLevel_StageDefault
{
private:
	CLevel_Subway(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Subway() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;

private:
	CSoundStore m_BGM;
	CDoOnce m_BGMOnce;

	_bool m_bBossBGM = false;
	_bool m_bMiddleBGM = false;
	CDoOnce m_BGMChange;

	string m_MainSound = "The OSF -Advance";
public:
	static CLevel_Subway* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
	
};

END