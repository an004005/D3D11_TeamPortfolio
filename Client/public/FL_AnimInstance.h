#pragma once
#include "Client_Defines.h"
#include "AnimationInstance.h"

BEGIN(Client)

class CFL_AnimInstance : public CAnimationInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void UpdateTargetState(_double TimeDelta) override;

	virtual void Imgui_RenderState() override;

	const string& GetCurStateName() { return m_pASM_Base->GetCurState()->m_strName; }

public:
	_bool isSocketEmpty(const string& strSocName) { return m_mapAnimSocket[strSocName].empty(); }
	_bool isSocketPassby(const string& strSocName, _float fPlayRatio = 1.f);
	void InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList);
	void AttachAnimSocket(const string& strSocName, const list<CAnimation*>& AnimList);
	_bool CheckSocketAnim(const string& strSocName, const string& AnimName) { return (AnimName == m_mapAnimSocket[strSocName].front()->GetName()) ? true : false; }

private:
	CAnimationStateMachine* m_pASM_Base = nullptr;
	
	_bool			m_bAir = false;
	_bool			m_bPreAir = false;
	_bool			m_bMove = false;
	_bool			m_bRun = false;
	
	_float3 m_vMoveAxis;
	EBaseAxis m_eMoveAxis = EBaseAxis::AXIS_END;
	EBaseAxis m_ePreMoveAxis = EBaseAxis::AXIS_END;
	EBaseTurn m_eTurn = EBaseTurn::TURN_END;
	_float m_fTurnRemain = 0.f;
	
private:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.1f;
	_bool	m_bLerp = false;
	_bool m_bAttach = false;

public:
	static CFL_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END