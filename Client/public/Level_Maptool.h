#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CComponent;
END;

enum PROTOINFO { NON_INSTANCE, INSTANCE, KINETIC};

BEGIN(Client)
class CLevel_Maptool : public CLevel
{
private:
	CLevel_Maptool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Prototypes();
	HRESULT Ready_Layer_BackGround(const wstring& pLayerTag);
	HRESULT Ready_Layer_Camera(const wstring& pLayerTag);
	HRESULT Ready_Layer_Map(const wstring& pLayerTag);

private:
	HRESULT Create_Model(const wstring& pProtoTag, const char* pModelPath, PROTOINFO eProtoInfo);
	HRESULT Create_Model_Instance(const wstring& pProtoTag, const char* pModelPath);
private:
	vector<pair<wstring, PROTOINFO>> m_pProtosTags;

public:
	static CLevel_Maptool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END