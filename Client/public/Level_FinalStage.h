#pragma once
#include "Level_StageDefault.h"
#include "Client_Defines.h"
#include "SoundStore.h"
#include "HelperClasses.h"

BEGIN(Client)

class CLevel_FinalStage : public CLevel_StageDefault
{
private:
	CLevel_FinalStage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_FinalStage() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Ready_Lights() override;

	virtual void Tick(_double TimeDelta) override;

private:
	CSoundStore m_BGM;
	CDoOnce m_BGMOnce;

	string m_MainSound = "Karen";

public:
	static CLevel_FinalStage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
	
};

END