#pragma once
#include <unordered_set>

#include "Client_Defines.h"
#include "Base.h"
#include "ParticleGroup.h"
#include "EffectGroup.h"

BEGIN(Client)

enum EFFECT
{
	EF_DEFAULT_ATTACK,
	EF_FIRE_ATTACK,
	EF_ELEC_ATTACK,
	EF_SAS,
	EF_HIT,
	EF_MONSTER,
	EF_ENVIRENMENT,
	EF_UI,
	EF_END
};

const static array<string, EFFECT::EF_END> arrayEffectMatch{
	"../Bin/Resources/Curve/EffectGroup/Default_Attack/",
	"../Bin/Resources/Curve/EffectGroup/Fire_Attack/",
	"../Bin/Resources/Curve/EffectGroup/Elec_Attack/",
	"../Bin/Resources/Curve/EffectGroup/Sas_Effect/",
	"../Bin/Resources/Curve/EffectGroup/Blood_Effect/",
	"../Bin/Resources/Curve/EffectGroup/Monster_Effect/",
	"../Bin/Resources/Curve/EffectGroup/Envirenment_Effect/",
	"../Bin/Resources/UI/UI_InGameDataGroup/"
};

enum PARTICLE
{
	PS_DEFAULT_ATTACK,
	PS_FIRE_ATTACK,
	PS_ELEC_ATTACK,
	PS_SAS,
	PS_HIT,
	PS_MONSTER,
	PS_ENVIRENMENT,
	PS_END
};

const static array<string, PARTICLE::PS_END> arrayParticleMatch{
	"../Bin/Resources/Curve/ParticleGroup/Default_Particle/",
	"../Bin/Resources/Curve/ParticleGroup/Fire_Particle/",
	"../Bin/Resources/Curve/ParticleGroup/Elec_Particle/",
	"../Bin/Resources/Curve/ParticleGroup/Sas_Particle/",
	"../Bin/Resources/Curve/ParticleGroup/Blood_Particle/",
	"../Bin/Resources/Curve/ParticleGroup/Monster_Particle/",
	"../Bin/Resources/Curve/ParticleGroup/Envirenment_Particle/",
};

const static _tchar* strEffectLayerTag{ L"Layer_VFX_EF" };
const static _tchar* strParticleLayerTag{ L"Layer_VFX_PS" };


class CVFX_Manager :	public CBase
{
	DECLARE_SINGLETON(CVFX_Manager)
public:
	CVFX_Manager();
	virtual ~CVFX_Manager() = default;

public:
	HRESULT Initialize(LEVEL eLevel);

public:
	// For Effect
	CEffectGroup*	GetEffect(EFFECT eType, const wstring& wstrEffectTag, const _tchar* wszLayerTag = strEffectLayerTag);
	CEffectGroup*	Find_Effect(EFFECT eType, const wstring& wstrEffectTag, const _tchar* wszLayerTag);
	CEffectGroup*	Add_Effect(const string& strEffectDir, const _tchar* wszLayerTag);

private:
	HRESULT			CreateEffect(LEVEL eLevel);
	void			LoadEffectData(LEVEL eLevel);
	//~ For Effect
public:
	// For Particle
	CParticleGroup* GetParticle(PARTICLE eType, const wstring& wstrParticleTag, const _tchar* wszLayerTag = strParticleLayerTag);
	CParticleGroup* Find_Particle(PARTICLE eType, const wstring& wstrParticleTag, const _tchar* wszLayerTag);
	CParticleGroup* Add_Particle(const string& strParticleDir, const _tchar* wszLayerTag);

private:
	HRESULT			CreateParticle(LEVEL eLevel);
	void			LoadParticleData(LEVEL eLevel);
	//~ For Particle
private:
	unordered_set<wstring> m_setEffectProtoTag;
	array<unordered_set<wstring>, EFFECT::EF_END> Effects;

	unordered_set<wstring> m_setParticleProtoTag;
	array<unordered_set<wstring>, PARTICLE::PS_END> Particles;

public:
	virtual void Free();
};

END