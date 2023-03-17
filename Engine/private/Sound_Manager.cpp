#include "stdafx.h"
#include "Sound_Manager.h"
#include "JsonStorage.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize(const char* pFilePath)
{
	if (pFilePath == nullptr)
		return E_FAIL;

	FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	FMOD_System_Init(m_pSystem, MAX_CHANNEL_COUNT, FMOD_INIT_NORMAL, NULL);
	Load_SoundFile(pFilePath);

	return S_OK;
}

void CSound_Manager::Tick(_float fTimeDelta, const _float4x4* pListenerWorldMatrix)
{
	if (pListenerWorldMatrix)
	{
		FMOD_System_Set3DListenerAttributes(m_pSystem, 0,
			(FMOD_VECTOR*)&pListenerWorldMatrix->m[3][0],
			nullptr,
			(FMOD_VECTOR*)&pListenerWorldMatrix->m[2][0],
			(FMOD_VECTOR*)&pListenerWorldMatrix->m[1][0]);		
	}


	for (auto& SoundQ : m_SoundQs)
	{
		if (SoundQ.second.empty())
			continue;

		auto& pCurrentSound = SoundQ.second.front();

		if (IsPlaying(pCurrentSound.pSound) == false)
		{
			Safe_Release(pCurrentSound.pSound);
			SoundQ.second.pop();

			if (SoundQ.second.empty() == false)
			{
				ReservedSound& tReserved = SoundQ.second.front();
				_float4* vPos = tReserved.bPos ? &tReserved.vPos : nullptr;
				_float4* vVelocity = tReserved.bVelocity ? &tReserved.vVelocity : nullptr;
				PlaySound(tReserved.pSound, vPos, vVelocity);
			}
		}
	}

	FMOD_BOOL bPlay = false;
	for (auto itr = m_PlayingSounds.begin(); itr != m_PlayingSounds.end();)
	{
		FMOD_Channel_IsPlaying(itr->second.pChannel, &bPlay);
		if (!bPlay)
		{
			auto pSound = itr->first;
			Safe_Release(pSound);
			itr = m_PlayingSounds.erase(itr);
		}
		else
			++itr;
	}

	for (auto& playingSound : m_PlayingSounds)
	{
		playingSound.second.m_fTime += fTimeDelta;
	}

	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlaySound(CSound* pSound, _float4* pPos, _float4* pVelocity)
{
	 // 빈 채널 없으면 제일 오래된거 제거
	if (m_PlayingSounds.size() + m_LoopingSounds.size() >= MAX_CHANNEL_COUNT)
	{
		auto old = m_PlayingSounds.begin();
		for (auto itr = m_PlayingSounds.begin(); itr != m_PlayingSounds.end(); ++itr)
		{
			if (old->second.m_fTime < itr->second.m_fTime)
				old = itr;
		}

		FMOD_Channel_Stop(old->second.pChannel);
		auto pSound = old->first;
		Safe_Release(pSound);
		m_PlayingSounds.erase(old);
	}

	Assert(m_PlayingSounds.size() + m_LoopingSounds.size() < MAX_CHANNEL_COUNT);// 여유분 1개 항상 필요

	const auto itr = m_PlayingSounds.find(pSound);
	if (itr != m_PlayingSounds.end()) // 현재 실행 중인 사운드가 들어옴 -> 재실행
	{
		FMOD_Channel_Stop(itr->second.pChannel);
		itr->first->PlaySound(m_pSystem, &itr->second.pChannel, m_fMasterVolume, pPos, pVelocity);
	}
	else // 새로 실행
	{
		Safe_AddRef(pSound);
		EngineChannel tChannel;
		ZeroMemory(&tChannel, sizeof(EngineChannel));
		pSound->PlaySound(m_pSystem, &tChannel.pChannel, m_fMasterVolume, pPos, pVelocity);

		if (pSound->IsLooping())
			m_LoopingSounds.emplace(pSound, tChannel);
		else
			m_PlayingSounds.emplace(pSound, tChannel);
	}
}

void CSound_Manager::PlaySound_Q(const string& QName, CSound* pSound, _float4* pPos, _float4* pVelocity)
{
	Assert(pSound != nullptr);
	if (pSound->IsLooping())
		return;

	auto itr = m_SoundQs.find(QName);
	if (itr == m_SoundQs.end())
		return;

	ReservedSound tReservedSound;
	ZeroMemory(&tReservedSound, sizeof(ReservedSound));

	tReservedSound.pSound = pSound;
	tReservedSound.bPos = pPos != nullptr;
	if (tReservedSound.bPos)
		tReservedSound.vPos = *pPos;
	tReservedSound.bVelocity = pVelocity != nullptr;
	if (tReservedSound.bVelocity)
		tReservedSound.vVelocity = *pVelocity;
	Safe_AddRef(tReservedSound.pSound);

	if (itr->second.empty())
	{
		PlaySound(pSound, pPos, pVelocity);
	}
	itr->second.push(tReservedSound);
}

HRESULT CSound_Manager::Load_SoundFile(const char* pJsonFilePath)
{
	const Json& json = CJsonStorage::GetInstance()->FindOrLoadJson(pJsonFilePath);
	const string soundFilePath = json["SoundFilePath"];

	for (auto soundInfo : json["Sounds"])
	{
		CSound* pSound = CSound::Create(m_pSystem, soundInfo, soundFilePath);
		m_Sounds.emplace(pSound->GetName(), pSound);
	}

	return S_OK;
}

_bool CSound_Manager::IsPlaying(CSound* pSound)
{
	Assert(pSound != nullptr);
	FMOD_BOOL bPlay = false;

	auto itr = m_LoopingSounds.find(pSound);
	if (itr != m_LoopingSounds.end())
	{
		FMOD_Channel_IsPlaying(itr->second.pChannel, &bPlay);
		if (bPlay) return true;
	}

	itr = m_PlayingSounds.find(pSound);
	if (itr != m_PlayingSounds.end())
	{
		FMOD_Channel_IsPlaying(itr->second.pChannel, &bPlay);
		if (bPlay) return true;
	}

	return false;
}

void CSound_Manager::Update_3DAttribute(CSound* pSound, const _float4& vPos, _float4* pVelocity)
{
	if (pSound->Is2DSound())
		return;

	if (FMOD_CHANNEL* pChannel = FindChannel(pSound))
	{
		FMOD_Channel_Set3DAttributes(pChannel, (FMOD_VECTOR*)&vPos, (FMOD_VECTOR*)pVelocity);
	}
}

FMOD_CHANNEL* CSound_Manager::FindChannel(CSound* pSound)
{
	FMOD_BOOL bPlay = false;

	auto itr = m_LoopingSounds.find(pSound);
	if (itr != m_LoopingSounds.end())
	{
		FMOD_Channel_IsPlaying(itr->second.pChannel, &bPlay);
		if (bPlay) return itr->second.pChannel;
	}

	itr = m_PlayingSounds.find(pSound);
	if (itr != m_PlayingSounds.end())
	{
		FMOD_Channel_IsPlaying(itr->second.pChannel, &bPlay);
		if (bPlay) return itr->second.pChannel;
	}

	return nullptr;
}

void CSound_Manager::Stop_All()
{
	for (auto& playingSound : m_PlayingSounds)
	{
		FMOD_Channel_Stop(playingSound.second.pChannel);
		auto pSound = playingSound.first;
		Safe_Release(pSound);
	}
	m_PlayingSounds.clear();

	for (auto& loopingSound : m_LoopingSounds)
	{
		FMOD_Channel_Stop(loopingSound.second.pChannel);
		auto pSound = loopingSound.first;
		Safe_Release(pSound);
	}
	m_LoopingSounds.clear();

	for (auto& soundQPair : m_SoundQs)
	{
		auto& SoundQ = soundQPair.second;
		while (SoundQ.empty() == false)
		{
			auto pSound = SoundQ.front().pSound;
			Safe_Release(pSound);
			SoundQ.pop();
		}
	}
}

void CSound_Manager::Stop(CSound* pSound)
{
	auto itr = m_LoopingSounds.find(pSound);
	if (itr != m_LoopingSounds.end())
	{
		FMOD_Channel_Stop(itr->second.pChannel);
		auto pSound = itr->first;
		Safe_Release(pSound);
		m_LoopingSounds.erase(itr);
		return;
	}

	itr = m_PlayingSounds.find(pSound);
	if (itr != m_PlayingSounds.end())
	{
		FMOD_Channel_Stop(itr->second.pChannel);
		auto pSound = itr->first;
		Safe_Release(pSound);
		m_PlayingSounds.erase(itr);
		return;
	}
}

void CSound_Manager::SetVolume(CSound* pSound, _float fVolume)
{
	pSound->SetVolume(fVolume);

	FMOD_CHANNEL* pChannel = FindChannel(pSound);
	if (pChannel)
	{
		FMOD_Channel_SetPaused(pChannel, TRUE);
		FMOD_Channel_SetVolume(pChannel, fVolume * m_fMasterVolume);
		FMOD_Channel_SetPaused(pChannel, FALSE);
	}
}

CSound* CSound_Manager::CloneSound(const string& soundName)
{
	auto itr = m_Sounds.find(soundName);
	if (itr != m_Sounds.end())
		return itr->second->Clone();

	return nullptr;
}

void CSound_Manager::Free()
{
	Stop_All();

	for (auto& sound : m_Sounds)
		Safe_Release(sound.second);
	m_Sounds.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
