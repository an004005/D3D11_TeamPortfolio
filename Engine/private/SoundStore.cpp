#include "stdafx.h"
#include "..\public\SoundStore.h"
#include "Sound_Manager.h"
#include "Transform.h"

CSoundStore::CSoundStore()
{
}


CSoundStore::~CSoundStore()
{
	StopAllLoop();

	for (auto& sound : m_Sounds)
		Safe_Release(sound.second);
}

void CSoundStore::CloneSound(const string& SoundName)
{
	auto pSound = CSound_Manager::GetInstance()->CloneSound(SoundName);
	Assert(pSound != nullptr);

	m_Sounds.emplace(SoundName, pSound);
}

void CSoundStore::PlaySound(const string& SoundName, _float4* pPos, _float4* pVelocity)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	CSound_Manager::GetInstance()->PlaySound(itr->second, pPos, pVelocity);
}

void CSoundStore::PlaySoundQueue(const string& SoundName, const string& QueueName, _float4* pPos, _float4* pVelocity)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	CSound_Manager::GetInstance()->PlaySound_Q(QueueName, itr->second, pPos, pVelocity);
}

void CSoundStore::Update_3DAttribute(const string& SoundName, const _float4& vPos, _float4* pVelocity)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	CSound_Manager::GetInstance()->Update_3DAttribute(itr->second, vPos, pVelocity);
}

_bool CSoundStore::IsPlaying(const string& SoundName)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	return CSound_Manager::GetInstance()->IsPlaying(itr->second);
}

void CSoundStore::Stop(const string& SoundName)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	CSound_Manager::GetInstance()->Stop(itr->second);
}

void CSoundStore::StopAll()
{
	for (auto& sound : m_Sounds)
		CSound_Manager::GetInstance()->Stop(sound.second);
}

void CSoundStore::StopAllLoop()
{
	for (auto& sound : m_Sounds)
		if (sound.second->IsLooping())
			CSound_Manager::GetInstance()->Stop(sound.second);
}

void CSoundStore::SetVolume(const string& SoundName, _float fVolume)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	CSound_Manager::GetInstance()->SetVolume(itr->second, fVolume);
}

void CSoundStore::PlaySound(const string& SoundName, CTransform* pTransform)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	_float4 vPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	CSound_Manager::GetInstance()->PlaySound(itr->second, &vPos, nullptr);
}

void CSoundStore::Update_3DAttribute(const string& SoundName, CTransform* pTransform)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	_float4 vPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	CSound_Manager::GetInstance()->Update_3DAttribute(itr->second, vPos, nullptr);
}

CSound* CSoundStore::GetSound(const string& SoundName)
{
	auto itr = m_Sounds.find(SoundName);
	Assert(itr != m_Sounds.end());

	return itr->second;
}
