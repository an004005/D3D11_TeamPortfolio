#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_EnemiesTest final : public CLevel
{
private:
	CLevel_EnemiesTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_EnemiesTest() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Prototypes();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Bullet(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Batch(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Kinetic(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);

	HRESULT Ready_Effect(const _tchar* pLayerTag);

private:
	HRESULT Create_Model(const wstring& pProtoTag, const char* pModelPath);

public:
	static CLevel_EnemiesTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END