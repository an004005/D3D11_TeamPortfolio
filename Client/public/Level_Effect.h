#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CLevel_Effect : public CLevel
{
private:
	CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Prototypes();
	HRESULT Ready_Layer(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);

	void	LoadEffects(const char* pEffectDir);
private:
	string m_EffectSystemTag = "../Bin/Resources/VFX/EffectSystem/";
	string m_EffectGroupTag = "../Bin/Resources/Curve/";


	unordered_map<string, string>  m_mapEffectGroup;
	string								m_CurEffectName;

public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};



END
