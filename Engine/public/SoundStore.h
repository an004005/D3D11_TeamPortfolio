#pragma once

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CSoundStore
{
public:
	CSoundStore();
	~CSoundStore();

public:
	void CloneSound(const string& SoundName);
	void PlaySound(const string& SoundName, _float4* pPos = nullptr, _float4* pVelocity = nullptr);
	void PlaySoundQueue(const string& SoundName, const string& QueueName, _float4* pPos = nullptr, _float4* pVelocity = nullptr);
	void Update_3DAttribute(const string& SoundName, const _float4& vPos, _float4* pVelocity = nullptr);
	_bool IsPlaying(const string& SoundName);
	void Stop(const string& SoundName);
	void StopAll();
	void StopAllLoop();
	void SetVolume(const string& SoundName, _float fVolume);

	void PlaySound(const string& SoundName, CTransform* pTransform);
	void Update_3DAttribute(const string& SoundName, CTransform* pTransform);


	class CSound* GetSound(const string& SoundName);

private:
	unordered_map<string, class CSound*> m_Sounds;
};

END