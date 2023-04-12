#pragma once
#include "Camera.h"

BEGIN(Engine)

typedef struct tagCamAnimationEvent
{
	_double EventTime;
	string strEventName;
} CAM_ANIM_EVENT;

typedef struct tagCamKeyframe
{
	double		Time;
	_float4	vRotation;
	_float3	vPosition;
	_float		fFOV;
}CAM_KEYFRAME;

typedef struct tagCamLookAt
{
	double		Time;
	_float3	vPosition;
}CAM_LOOKAT;

class ENGINE_DLL CCamAnimation : public CBase
{
public:
	CCamAnimation();
	virtual ~CCamAnimation() = default;

	void Initialize(const char* pAnimFilePath);
	void SaveToJson(Json& json);

	const string& GetName() const { return m_strName; }
	_double GetDuration() const { return m_Duration; }
	_double GetPlayTime() const { return m_PlayTime; }
	_double GetTickPerSec() const { return m_TickPerSecond; }
	_float GetPlayRatio() const { return static_cast<_float>(m_PlayTime / m_Duration); }
	_bool IsFinished() const { return m_bFinished; }
	void SetFinish(_bool bFinish) { m_bFinished = bFinish; }
	 
	void SetFinish() { m_bFinished = true; }
	void SetPlayTime(_double PlayTime) { m_PlayTime = PlayTime; }
	void SetTickPerSec(_double TickPerSec) { m_TickPerSecond = TickPerSec; }
	void SetName(const string& strName) { m_strName = strName; }
	void Reset();

	_matrix Update_Animation(_double TimeDelta, _float& fFOV, list<string>& EventNames);
	_vector Update_LookAtPos(_double TimeDelta);

	CAM_KEYFRAME GetFirstFrame() const;
	CAM_KEYFRAME GetLastFrame() const;
	_float GetNear() const { return m_fNear; }

	void AddFrame(_double Time, _matrix WorldMatrix, _float fFOV);
	void AddFrame(_matrix WorldMatrix, _float fFOV);
	void PopFrame() { if (m_KeyFrames.empty() == false) m_KeyFrames.pop_back(); }
	void AddEvent(_double Time, const string& strEventName);
	void AddLookAt(_double Time, _fvector vPosition);

	void Imgui_Render();
	_float GetPlayTime()		{ return m_PlayTime; }
	_float GetDuration()		{ return m_Duration; }
	_float GetTickPerSecond()	{ return m_TickPerSecond; }

	vector<CAM_KEYFRAME>& GetKeyFrames() { return m_KeyFrames; }
	vector<CAM_ANIM_EVENT>& GetEvents() { return m_vecEvent; }
	vector<CAM_LOOKAT>& GetCamLookAt() { return m_vecCamLookAt; }

private:
	string					m_strName;
	_double					m_Duration = 0.0;
	_double					m_TickPerSecond = 1.0;
	_double					m_PlayTime = 0.0;
	_bool					m_bFinished = false;
	_float					m_fNear = 0.1f;

	vector<CAM_KEYFRAME>	m_KeyFrames;
	vector<CAM_ANIM_EVENT>  m_vecEvent;
	vector<CAM_LOOKAT>		m_vecCamLookAt;

public:
	static CCamAnimation* Create(const char* pAnimFilePath);
	static CCamAnimation* Create();
	virtual void Free() override;
};

/******************
 * AnimCam
 *****************/

class ENGINE_DLL CAnimCam : public CCamera
{
private:
	CAnimCam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimCam(const CCamera& rhs);
	virtual ~CAnimCam() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Imgui_RenderProperty() override;

	void Tick_Move(_double TimeDelta);

	_bool IsFinished() const { return m_bStart == false; }
	_bool IsAnimFinished() const { return m_pCamAnimation && m_pCamAnimation->IsFinished(); }
	void StartCamAnim_Return(CCamAnimation* pCamAnim, CCamera* pPreCam, _float4x4 WorldMatrix, _float fStartLerpTime = 0.f, _float fEndLerpTime = 0.f, _bool bRotLerp = false);
	void StartCamAnim(CCamAnimation* pCamAnim, _float4x4 PreCamWorldMatrix, _float4x4 WorldMatrix, _float fStartLerpTime = 0.f, _float fEndLerpTime = 0.f);
	void AddEvent(const string& strEventName, const function<void()>& Callback);
	list<string>& GetRequestedEvents() { return m_RequestedEvents; }

	void StartCamAnim_Return_Update(CCamAnimation* pCamAnim, CCamera* pPreCam, CTransform* pTransform, _float fStartLerpTime = 0.f, _float fEndLerpTime = 0.f, _bool bRotLerp = false);
	void StartCamAnim_Update(CCamAnimation* pCamAnim, _float4x4 PreCamWorldMatrix, CTransform* pTransform, _float fStartLerpTime = 0.f, _float fEndLerpTime = 0.f);

private:
	_float		m_fMouseSpeed = 0.03f;
	_float		m_fSpeedPerSec = 10.f;
	_bool m_bEditMode = false;
	_bool m_bStop = false;


	_bool m_bStart = false;
	_float4x4 m_PreCamPivotMatrix;
	_float m_fStartLerpTime = 0.f;
	_float m_fMaxStartLerpTime = 0.f;
	_float m_fEndLerpTime = 0.f;
	_float m_fMaxEndLerpTime = 0.f;

	CCamAnimation* m_pCamAnimation = nullptr;
	CCamera* m_pPreCam = nullptr;
	CTransform* m_pTargetTransform = nullptr;

	unordered_map<string, function<void()>> m_Events;
	list<string> m_RequestedEvents;

private:
	_int	m_iLookAtPos_Index = 0;
	_float4x4	m_LookAtPivot = XMMatrixIdentity();

	_int	m_iKeyFrame_Index = 0;
	_float4x4	m_KeyFramePivot = XMMatrixIdentity();

public:
	static CAnimCam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END