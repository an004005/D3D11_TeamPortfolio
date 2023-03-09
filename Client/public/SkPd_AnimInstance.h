#pragma once
#include "Client_Defines.h"
#include "AnimationInstance.h"

BEGIN(Client)

class CSkPd_AnimInstance : public CAnimationInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void UpdateTargetState(_double TimeDelta) override;
	virtual void Imgui_RenderState() override;
	const string& GetCurStateName() { return m_pASM_Base->GetCurState()->m_strName; }

public:
	void			AttachAnimSocket(const string& strSocName, list<CAnimation*> AnimList);
	void			InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList);

	const string&	GetCurAnimName() { return m_pASM_Base->GetCurState()->m_Animation->GetName(); } // 현재 재생중인 애니메이션 이름은 모델에서 가져올 것 -> State Machine은 Transition이 겹치면 통과함
	_bool			isLerping() { return m_pASM_Base->isLerping(); }
	_bool			isSocketEmpty(const string& strSocName) { return m_mapAnimSocket[strSocName].empty(); }

	_bool			isSocketPassby(const string& strSocName, _float fPlayRatio = 1.f);
	_bool			CheckSocketAnim(const string& strSocName, const string& AnimName) { return (AnimName == m_mapAnimSocket[strSocName].front()->GetName()) ? true : false; }

private:
	CAnimationStateMachine* m_pASM_Base = nullptr;

	_bool			m_bIdle = false;
	_bool			m_bAir = false;
	_bool			m_bPreAir = false;
	// Move
	_bool			m_bMove = false;

	_float3 m_vMoveAxis;
	EBaseAxis m_eMoveAxis = EBaseAxis::AXIS_END;
	EBaseAxis m_ePreMoveAxis = EBaseAxis::AXIS_END;
	EBaseTurn m_eTurn = EBaseTurn::TURN_END;
	_float m_fTurnRemain = 0.f;

private:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.1f;
	_bool	m_bLerp = false;
	_bool	m_bAttach = false;

public:
	static CSkPd_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END