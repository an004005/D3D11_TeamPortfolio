#pragma once

#include "Base.h"

BEGIN(Engine)

enum class EAnimUpdateType
{
	NORMAL,
	BLEND,
	ADDITIVE,
	TYPE_END
};

typedef struct tagAnimationEvent
{
	_double EventTime;
	string strEventName;
}ANIM_EVENT;

class ENGINE_DLL CAnimation final : public CBase
{
public:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const char* pAnimFilePath);
	void Update_Bones(_double TimeDelta, EAnimUpdateType eType, _float fRatio = 0.f);
	void Update_BonesAtTime(_double PlayTime, EAnimUpdateType eType, _float fRatio = 0.f);
	void Link_Model(class CModel* pModel);

	const string& GetName() const { return m_strName; }
	_double GetDuration() const { return m_Duration; }
	_double GetPlayTime() const { return m_PlayTime; }
	_double GetTickPerSec() const { return m_TickPerSecond; }
	_float GetPlayRatio() const { return static_cast<_float>(m_PlayTime / m_Duration); }
	_bool	GetInterpolation() { return m_bInterpolation; }
	_bool IsFinished() const { return m_bFinished; }
	_bool IsLooping() const { return m_bLooping; }

	void SetLooping(_bool bLooping) { m_bLooping = bLooping; }
	void SetFinish() { m_bFinished = true; }
	void SetPlayTime(_double PlayTime) { m_PlayTime = PlayTime; }
	void SetTickPerSec(_double TickPerSec) { m_TickPerSecond = TickPerSec; }
	void SetStay(_bool bStay) { m_bStay = bStay; }
	void SetPlayRatio(_double PlayRatio) { m_PlayTime = m_Duration * PlayRatio; }
	void SetInterpolation(_bool bInterp) { m_bInterpolation = bInterp; }
	void Reset();

	void SetLocalRotation(_bool bLocalRotation);

	void SaveModifiedData(Json& json);
	void Imgui_RenderProperty();

public:
	void Update_Bones_SyncRatio(_double PlayTime);
	void Update_Bones_SyncRatio_NonLocalLock(_double PlayTime);

public:
	KEYFRAME*						GetCurKeyFrame();
	_double							GetCurDuration() const { return m_Duration; }
	unordered_map<double, string>	GetEventMap() const { return m_EventMap; }
	_vector&						GetLocalMove() { return m_vLocalMove; }
	_vector&						GetSpecialLocalMove() { return m_vSpecialLocalMove; }
	_vector&						GetLocalRotation() { return m_vLocalRotation; }
	Quaternion&						GetLocalQuaternion() { return m_vLocalQuaternion; }
	pair<_vector, _float>&			GetLocalEular() { return m_vLocalEular; }
	_vector&						GetEffectLocalMove() { return m_vEffectLocalMove; }

public:
	static CAnimation s_NullAnimation;
	static const string s_ModifyFilePath;

private:
	string								m_strName;
	_double								m_Duration = 0.0;

	_double								m_TickPerSecond;

	_double								m_PlayTime = 0.0;

	_bool								m_bFinished = false;
	_bool								m_bLooping = false;
	_bool								m_bStay = false;

	_bool								m_bInterpolation = true;
	_vector								m_vLocalMove;
	_vector								m_vSpecialLocalMove;
	_vector								m_vLocalRotation;
	Quaternion							m_vLocalQuaternion;
	pair<_vector, _float>				m_vLocalEular;
	_vector								m_vEffectLocalMove;

	vector<class CChannel*>				m_Channels;

	/* ?´ë²¤??ë§?*/
	unordered_map<double, string>		m_EventMap;
	vector<ANIM_EVENT>					m_vecEvent;



	class CModel*						m_pModel = nullptr;

public:
	static CAnimation* Create(const char* pAnimFilePath);
	CAnimation* Clone();
	virtual void Free() override;
};

END