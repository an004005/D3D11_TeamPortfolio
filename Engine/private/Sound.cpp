#include "stdafx.h"
#include "Sound.h"
#include "JsonLib.h"
#include "Sound_Manager.h"

CSound::CSound()
{
	m_bClone = false;
}

CSound::CSound(const CSound& rhs)
	: m_pSounds(rhs.m_pSounds)
	, m_strName(rhs.m_strName)
	, m_bLooping(rhs.m_bLooping)
	, m_b2DSound(rhs.m_b2DSound)
	, m_fVolume(rhs.m_fVolume)
	, m_fMinDistance(rhs.m_fMinDistance)
	, m_fMaxDistance(rhs.m_fMaxDistance)
	, m_bClone(true)
{
}

HRESULT CSound::Initialize(FMOD_SYSTEM* pSystem, const Json& soundJson, const string& strSoundFilePath)
{
	m_strName = soundJson["Name"];
	m_bLooping = soundJson["Loop"];
	m_b2DSound = soundJson["2DSound"];
	m_fVolume = soundJson["Volume"];
	if (m_b2DSound == false)
	{
		m_fMinDistance = soundJson["MinDistance"];
		m_fMaxDistance = soundJson["MaxDistance"];
	}

	vector<string> soundFileNames = soundJson["SoundFiles"];
	Assert(soundFileNames.empty() == false);

	FMOD_MODE mode = FMOD_DEFAULT;
	if (m_b2DSound == false)
		mode |= FMOD_3D_LINEARROLLOFF;
	if (m_bLooping)
		mode |= FMOD_LOOP_NORMAL;

	m_pSounds.reserve(soundFileNames.size());

	for (auto soundFileName : soundFileNames)
	{
		string soundFilePath = strSoundFilePath + soundFileName;

		FMOD_SOUND* pFSound = nullptr;
		FMOD_System_CreateSound(pSystem, soundFilePath.c_str(), mode, 0, &pFSound);
		Assert(pFSound != nullptr);
		if (m_b2DSound == false)
		{
			FMOD_Sound_Set3DMinMaxDistance(pFSound, m_fMinDistance, m_fMaxDistance);
		}

		m_pSounds.push_back(pFSound);
	}

	return S_OK;
}

void CSound::Play(_float4* pPos, _float4* pVelocity)
{
	CSound_Manager::GetInstance()->PlaySound(this, pPos, pVelocity);
}

void CSound::Stop()
{
	CSound_Manager::GetInstance()->Stop(this);
}

void CSound::SetVolume(_float fVolume)
{
	m_fVolume = fVolume;
}

_bool CSound::IsPlaying()
{
	return CSound_Manager::GetInstance()->IsPlaying(this);
}

void CSound::Update_3DAttribute(const _float4& vPos, _float4* pVelocity)
{
	CSound_Manager::GetInstance()->Update_3DAttribute(this, vPos, pVelocity);
}

void CSound::PlaySound(FMOD_SYSTEM* pSystem, FMOD_CHANNEL** ppChannel, _float fMasterVolume, _float4* pPos, _float4* pVelocity)
{
	FMOD_SOUND* pFSound = nullptr;
	if (m_pSounds.size() > 1)
		pFSound = m_pSounds[static_cast<size_t>(rand()) % m_pSounds.size()];
	else
		pFSound = m_pSounds.front();

	FMOD_System_PlaySound(pSystem, pFSound, nullptr, TRUE, ppChannel);
	
	if (m_b2DSound == false && pPos)
	{
		FMOD_Channel_Set3DAttributes(*ppChannel, (FMOD_VECTOR*)pPos, (FMOD_VECTOR*)pVelocity);
	}
	FMOD_Channel_SetVolume(*ppChannel, m_fVolume * fMasterVolume);
	FMOD_Channel_SetPaused(*ppChannel, FALSE);
}

CSound* CSound::Create(FMOD_SYSTEM* pSystem, const Json& soundJson, const string& strSoundFilePath)
{
	CSound* pSound = new CSound;
	if (pSound->Initialize(pSystem, soundJson, strSoundFilePath))
	{
		MSG_BOX("Failed to Created : CSound");
		Safe_Release(pSound);
	}

	return pSound;
}

CSound* CSound::Clone()
{
	return new CSound(*this);
}

void CSound::Free()
{
	if (m_bClone == false)
	{
		for (auto pFSound : m_pSounds)
		{
			FMOD_Sound_Release(pFSound);
		}
	}
}
