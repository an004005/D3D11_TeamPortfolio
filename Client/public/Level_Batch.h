#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CComponent;
class CGameObject;
END;

enum BATCHTYPE { MONSTER, TUTOTIAL, TYPE_END };
BEGIN(Client)
class CLevel_Batch : public CLevel
{
private:
	CLevel_Batch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

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
	HRESULT Ready_Layer_Batch(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);

private:
	//for input listbox in batch
	HRESULT	Push_Back_Prototype(const _tchar* pLayerTag, CGameObject* pGameObject, BATCHTYPE eType);
private:
	vector<pair<wstring, BATCHTYPE>>	m_ProtosInfo;

public:
	static CLevel_Batch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END