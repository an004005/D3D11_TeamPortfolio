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
	_bool IsFinished() const { return m_bFinished; }
	_bool IsLooping() const { return m_bLooping; }

	void SetLooping(_bool bLooping) { m_bLooping = bLooping; }
	void SetFinish() { m_bFinished = true; }
	void SetPlayTime(_double PlayTime) { m_PlayTime = PlayTime; }
	void SetTickPerSec(_double TickPerSec) { m_TickPerSecond = TickPerSec; }
	void SetStay(_bool bStay) { m_bStay = bStay; }
	void Reset();

	void SaveModifiedData(Json& json);
	void Imgui_RenderProperty();

public:
	KEYFRAME*						GetCurKeyFrame();
	_double							GetCurDuration() const { return m_Duration; }
	unordered_map<double, string>	GetEventMap() const { return m_EventMap; }
	_vector&						GetLocalMove() { return m_vLocalMove; }

public:
	static CAnimation s_NullAnimation;
	static const string s_ModifyFilePath;

private:
	string								m_strName;
	/* 애니메이션을 재생하는데 거리른ㄴ 전체 시간. */
	_double								m_Duration = 0.0;

	/* 초당 재생해야할 애니메이션의 속도이다. */
	_double								m_TickPerSecond;

	_double								m_PlayTime = 0.0;

	_bool								m_bFinished = false;
	// 애니메이션 끝나면 처음부터 다시 시작
	_bool								m_bLooping = false;
	// 애니메이션 끝나면 마지작 프레임 유지
	_bool								m_bStay = false;

	// 로컬 좌표 이동
	_vector								m_vLocalMove;

	/* 이 애니메이션을 재생하기위해 갱신해야하는 뼈들. */
	vector<class CChannel*>				m_Channels;

	/* 이벤트 맵 */
	unordered_map<double, string>		m_EventMap;
	vector<ANIM_EVENT>					m_vecEvent;



	// 상호참조로 레퍼런스 카운트 증가하지 않는다.
	class CModel*						m_pModel = nullptr;

public:
	static CAnimation* Create(const char* pAnimFilePath);
	CAnimation* Clone();
	virtual void Free() override;
};

END