#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CSound : public CBase
{
private:
	CSound();
	CSound(const CSound& rhs);
	virtual ~CSound() = default;

public:	
	HRESULT Initialize(FMOD_SYSTEM* pSystem, const Json& soundJson, const string& strSoundFilePath);

	void Play(_float4* pPos = nullptr, _float4* pVelocity = nullptr);
	void Stop();
	void SetVolume(_float fVolume);
	_bool IsPlaying();
	void Update_3DAttribute(const _float4& vPos, _float4* pVelocity = nullptr);

	void PlaySound(FMOD_SYSTEM* pSystem, FMOD_CHANNEL** ppChannel, _float fMasterVolume, _float4* pPos = nullptr, _float4* pVelocity = nullptr);

public:
	const string& GetName() const { return m_strName; }
	_bool IsLooping() const { return m_bLooping; }
	_bool Is2DSound() const { return m_b2DSound; }

private:
	vector<FMOD_SOUND*> m_pSounds;
	string m_strName;
	_bool m_bLooping = false;
	_bool m_b2DSound = true;
	_float m_fVolume = 1.f;
	_float m_fMinDistance = 5.f;
	_float m_fMaxDistance = 50.f;
	_bool m_bClone = false;

public:
	static CSound* Create(FMOD_SYSTEM* pSystem, const Json& soundJson, const string& strSoundFilePath);
	CSound* Clone();
	virtual void Free() override;
};
END
