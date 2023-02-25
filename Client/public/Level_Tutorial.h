#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Tutorial : public CLevel
{
private:
	CLevel_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tutorial() = default;

public:
	HRESULT Initialize() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;

private:
	HRESULT Ready_Prototypes();
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);

public:
	static CLevel_Tutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
	
};

END