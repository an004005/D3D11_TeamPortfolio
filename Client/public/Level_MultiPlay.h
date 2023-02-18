#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "CorePch.h"

BEGIN(Client)

class CLevel_MultiPlay final : public CLevel
{
private:
	CLevel_MultiPlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MultiPlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Prototypes();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player();
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_VFX(const _tchar* pLayerTag);

public:
	static CLevel_MultiPlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};



END
