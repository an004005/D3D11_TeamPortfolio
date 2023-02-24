#pragma once
#include "Client_Defines.h"
#include "AnimationInstance.h"

BEGIN(Client)

class CBaseAnimInstance : public CAnimationInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void UpdateTargetState(_double TimeDelta) override;

	virtual void Imgui_RenderState() override;

public:
	void			InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList);
	const string&	GetCurAnimName() { return m_pASM_Base->GetCurState()->m_Animation->GetName(); } // 현재 재생중인 애니메이션 이름은 모델에서 가져올 것 -> State Machine은 Transition이 겹치면 통과함

protected:
	_bool	CheckAnim(const string& szAnimName);

protected:	// 대상의 상태
	_bool	m_bAir = false;
	_bool	m_bPreAir = false;

	_bool	m_bMove = false;
	_bool	m_bWalk = false;

	_bool	m_bLeftClick = false;

	_float	m_fPlayRatio = 0.f;

	string	m_szCurAnimName = "";

	_uint	m_eMoveDir;

	CAnimationStateMachine* m_pASM_Base = nullptr;

protected:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.2f;
	_bool	m_bLerp = false;

public:
	static CBaseAnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END