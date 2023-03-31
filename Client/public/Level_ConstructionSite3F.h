#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_ConstructionSite3F : public CLevel
{
private:
	CLevel_ConstructionSite3F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_ConstructionSite3F() = default;

public:
	HRESULT Initialize() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;

private:
	HRESULT Ready_Prototypes();
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SASPortrait();

public:
	static CLevel_ConstructionSite3F* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END