#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)

class CLevel_KineticEditor : public CLevel
{
private:
	CLevel_KineticEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_KineticEditor() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Prototypes();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);

public:
	static CLevel_KineticEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END