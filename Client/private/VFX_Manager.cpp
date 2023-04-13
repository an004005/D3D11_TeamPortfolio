#include "stdafx.h"
#include "..\public\VFX_Manager.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "JsonStorage.h"

IMPLEMENT_SINGLETON(CVFX_Manager)



CVFX_Manager::CVFX_Manager()
{
}

HRESULT CVFX_Manager::Initialize(LEVEL eLevel)
{
	FAILED_CHECK(CreateEffect(eLevel));
	FAILED_CHECK(CreateParticle(eLevel));

#ifndef _DEBUG
	for (_uint i = 0; i < EFFECT::EF_END; ++i)
	{
		for (auto& EffectName : Effects[i])
		{
			GetEffect((EFFECT)i, EffectName)->SetDelete();
		}
	}

	for (_uint i = 0; i < PARTICLE::PS_END; ++i)
	{
		for (auto& ParticleName : Particles[i])
		{
			GetParticle((PARTICLE)i, ParticleName)->SetDelete();
		}
	}
#endif

	//GetEffect(EFFECT::EF_HIT, L"Default_Blood_00")->SetDelete();

	return S_OK;
}

CEffectGroup* CVFX_Manager::GetEffect(EFFECT eType, const wstring& wstrEffectTag, const _tchar* wszLayerTag)
{
	return Find_Effect(eType, wstrEffectTag, wszLayerTag);
}

CEffectGroup* CVFX_Manager::Find_Effect(EFFECT eType, const wstring& wstrEffectTag, const _tchar* wszLayerTag)
{
	auto iter = Effects[eType].find(wstrEffectTag);

	if (iter == Effects[eType].end())
		return nullptr;

	string EffectDir = arrayEffectMatch[eType] + ws2s(wstrEffectTag + L".json");

	return Add_Effect(EffectDir, wszLayerTag);
}

CEffectGroup* CVFX_Manager::Add_Effect(const string& strEffectDir, const _tchar* wszLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Json jsonEffect = CJsonStorage::GetInstance()->FindOrLoadJson(strEffectDir);
	CEffectGroup* pEffect = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(wszLayerTag, L"ProtoVFX_EffectGroup", &jsonEffect));

	if(pEffect == nullptr)
	{
		MSG_BOX("Failed to Add Effect");
		return nullptr;
	}

	return pEffect;
}

CParticleGroup* CVFX_Manager::GetParticle(PARTICLE eType, const wstring& wstrParticleTag, const _tchar* wszLayerTag)
{
	return Find_Particle(eType, wstrParticleTag, wszLayerTag);
}

CParticleGroup* CVFX_Manager::Find_Particle(PARTICLE eType, const wstring& wstrParticleTag, const _tchar* wszLayerTag)
{
	auto iter = Particles[eType].find(wstrParticleTag);

	if (iter == Particles[eType].end())
		return nullptr;

	string ParticleDir = arrayParticleMatch[eType] + ws2s(wstrParticleTag + L".json");

	return Add_Particle(ParticleDir, wszLayerTag);
}

CParticleGroup* CVFX_Manager::Add_Particle(const string& strParticleDir, const _tchar* wszLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Json jsonParticle = CJsonStorage::GetInstance()->FindOrLoadJson(strParticleDir);
	CParticleGroup* pParticle =	dynamic_cast<CParticleGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(wszLayerTag, L"ProtoVFX_ParticleGroup", &jsonParticle));
	
	if (pParticle == nullptr)
	{
		MSG_BOX("Failed to Add Particle");
		return nullptr;
	}

	return pParticle;
}

HRESULT CVFX_Manager::CreateEffect(LEVEL eLevel)
{
	LoadEffectData(eLevel);

	return S_OK;
}

void CVFX_Manager::LoadEffectData(LEVEL eLevel)
{
	for( _uint i = 0; i < EF_END; ++i)
	{
		string Dirpath = arrayEffectMatch[i];

		CGameUtils::ListFiles(Dirpath, [this, &i](const string& pEffectPath)
		{
			const string& strFileName = CGameUtils::GetFileName(pEffectPath);
			wstring ProtoTag = CGameUtils::s2ws(strFileName);
			Effects[i].insert(ProtoTag);
		});
	}
}

HRESULT CVFX_Manager::CreateParticle(LEVEL eLevel)
{
	LoadParticleData(eLevel);

	return S_OK;
}

void CVFX_Manager::LoadParticleData(LEVEL eLevel)
{
	for (_uint i = 0; i < PS_END; ++i)
	{
		string Dirpath = arrayParticleMatch[i];

		CGameUtils::ListFiles(Dirpath, [this, &i](const string& pParticlePath)
		{
			const string& strFileName = CGameUtils::GetFileName(pParticlePath);
			wstring ProtoTag = CGameUtils::s2ws(strFileName);
			Particles[i].insert(ProtoTag);
		});
	}
}



void CVFX_Manager::Free()
{
	
}
