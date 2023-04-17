
#pragma once
#include "Level.h"
#include "Client_Defines.h"
#include "HelperClasses.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END
BEGIN(Client)

class CLevel_StageDefault : public CLevel
{
protected:
	CLevel_StageDefault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_StageDefault() = default;

public:
	HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	HRESULT Render() override;

protected:
	virtual HRESULT Ready_Prototypes();
	virtual HRESULT Ready_Lights();
	virtual HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	virtual HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	virtual HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	virtual HRESULT Ready_Layer_Map(const _tchar* pLayerTag);
	virtual HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	virtual HRESULT Ready_Layer_SASPortrait(const _tchar* pLayerTag);
	virtual HRESULT Ready_Layer_AI(const _tchar* pLayerTag);

	template <typename T>
	T* FindGameObjectInLayer(const _tchar* pLayerTag)
	{
		if (auto pLayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, pLayerTag))
		{
			for (auto pObj : pLayer->GetGameObjects())
			{
				if (auto pType = dynamic_cast<T*>(pObj))
				{
					return pType;
				}
			}
		}

		return nullptr;
	}

protected:
	wstring m_strLevelName;
	string m_strShadowCamJsonPath;
	string m_strMapJsonPath;

	_bool m_bPlayerSpawn = true;

	CDoOnce m_SetMainCamOnce;
};

END

