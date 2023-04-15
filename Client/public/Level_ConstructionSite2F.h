#pragma once
#include "Client_Defines.h"
#include "Level_StageDefault.h"
#include "SoundStore.h"
#include "HelperClasses.h"

BEGIN(Client)

class CLevel_ConstructionSite2F : public CLevel_StageDefault
{
private:
	CLevel_ConstructionSite2F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_ConstructionSite2F() = default;

public:
	HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;

private:
	CSoundStore m_BGM;
	CDoOnce m_BGMOnce;

	_bool m_bBossBGM = false;
	_bool m_bMiddleBGM = false;
	CDoOnce m_BGMChange;

public:
	static CLevel_ConstructionSite2F* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	void Free() override;
};

END