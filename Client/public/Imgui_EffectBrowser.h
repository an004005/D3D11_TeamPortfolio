#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "GameInstance.h"

BEGIN(Client)
class CImgui_EffectBrowser :	public CImguiObject
{
private:
	CImgui_EffectBrowser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderWindow() override;
	static CImgui_EffectBrowser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);

	virtual void Free() override;


public:
	void LoadEffects(const char* pEffectDir);
private:
	string m_EffectSystemTag = "../Bin/Resources/VFX/EffectSystem/";
	string m_EffectGroupTag = "../Bin/Resources/Curve/";
	string m_ParticleSystemTag = "../Bin/Resources/VFX/ParticleSystem/";

	unordered_map<string, string>  m_mapEffectGroup;
	string								m_CurEffectName;

private:
	CGameInstance*		m_pGameInstance = nullptr;
};

END