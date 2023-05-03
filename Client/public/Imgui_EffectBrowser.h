#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "GameInstance.h"

BEGIN(Client)
class CImgui_EffectBrowser :	public CImguiObject
{
public:
	enum EFFECTFOLDER
	{
		BLOOD_EFFECT,
		DEFAULT_EFFECT,
		ELEC_EFFECT,
		FIRE_EFFECT,
		ENV_EFFECT,
		MONSTER_EFFECT,
		SAS_EFFECT,
		EFFECT_END
	};

private:
	CImgui_EffectBrowser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderWindow() override;
	static CImgui_EffectBrowser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);

	virtual void Free() override;


public:
	void LoadEffects(const char* pEffectDir);
	void LoadParticles(const char* pParticleDir);

private:
	string m_EffectSystemTag = "../Bin/Resources/VFX/EffectSystem/";
	string m_EffectGroupTag = "../Bin/Resources/Curve/EffectGroup/";
	string m_ParticleSystemTag = "../Bin/Resources/VFX/ParticleSystem/";
	string m_ParticleGroupTag = "../Bin/Resources/Curve/ParticleGroup/";

	unordered_map<string, string>		m_mapEffectGroup;
	string								m_CurEffectName;
	unordered_map<string, string>		m_mapParticleGroup;
	string								m_CurParticleName;
private:
	_int m_iSelectFolder = 0;
	vector < pair<wstring, EFFECTFOLDER>> m_vecEffectName;
	// vector < pair<wstring, EFFECTFOLDER>> m_vecEffectName;

private:
	CGameInstance*		m_pGameInstance = nullptr;
};



END
