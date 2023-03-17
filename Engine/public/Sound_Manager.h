#pragma once
#include "Sound.h"

#define MAX_CHANNEL_COUNT		32

BEGIN(Engine)

struct EngineChannel
{
	FMOD_CHANNEL* pChannel;
	_float m_fTime;
};

struct ReservedSound
{
	CSound* pSound;
	_float4 vPos;
	_bool bPos;
	_float4 vVelocity;
	_bool bVelocity;
};

class CSound_Manager : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize(const char *pFilePath);
	void Tick(_float fTimeDelta, const _float4x4* pListenerWorldMatrix = nullptr);

	void PlaySound(CSound* pSound, _float4* pPos = nullptr, _float4* pVelocity = nullptr);
	void PlaySound_Q(const string& QName, CSound* pSound, _float4* pPos = nullptr, _float4* pVelocity = nullptr);
	void Set_MasterVolume(_float fVolume) { m_fMasterVolume = fVolume; }
	_bool IsPlaying(CSound* pSound);
	void Update_3DAttribute(CSound* pSound, const _float4& vPos, _float4* pVelocity = nullptr);
	FMOD_CHANNEL* FindChannel(CSound* pSound);

	void Stop_All();
	void Stop(CSound* pSound);
	void SetVolume(CSound* pSound, _float fVolume);

	FMOD_SYSTEM* GetSystem() { return m_pSystem; }

	CSound* CloneSound(const string& soundName);
	void AddSoundQ(const string& QName)
	{
		m_SoundQs.insert({ QName, {} });
	}

private:
	HRESULT Load_SoundFile(const char *pJsonFilePath);

private:
	unordered_map<string, CSound*> m_Sounds;
	unordered_map<CSound*, EngineChannel> m_PlayingSounds;
	unordered_map<CSound*, EngineChannel> m_LoopingSounds;

	unordered_map<string, queue<ReservedSound>> m_SoundQs;

	
	FMOD_SYSTEM* m_pSystem = nullptr;
	_float m_fMasterVolume = 1.f;

public:
	virtual void Free() override;


};
END
