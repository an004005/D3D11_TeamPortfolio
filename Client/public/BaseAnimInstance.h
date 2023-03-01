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
	void			AttachAnimSocket(const string& strSocName, list<CAnimation*> AnimList);
	const string&	GetCurAnimName() { return m_pASM_Base->GetCurState()->m_Animation->GetName(); } // ���� ������� �ִϸ��̼� �̸��� �𵨿��� ������ �� -> State Machine�� Transition�� ��ġ�� �����
	_bool			isLerping() { return m_pASM_Base->isLerping(); }
	_bool			isSocketEmpty(const string& strSocName) { return m_mapAnimSocket[strSocName].empty(); }
	_bool			isSocketAlmostFinish(const string& strSocName);
	_bool			CheckSocketAnim(const string& strSocName, const string& AnimName) { return (AnimName == m_mapAnimSocket[strSocName].front()->GetName()) ? true : false; }

protected:
	_bool	CheckAnim(const string& szAnimName);

protected:	// ����� ����
	_bool	m_bAir = false;
	_bool	m_bPreAir = false;

	_bool	m_bMove = false;
	_bool	m_bWalk = false;

	_bool	m_bLeftClick = false;
	_bool	m_bDash = false;

	_bool	m_bJump = false;

	_bool	m_bNonCharge = false;
	_bool	m_bCharge = false;

	_bool	m_bSeperateAnim = false;

	_bool	m_bOnFloor = false;

	_float	m_fPlayRatio = 0.f;

	string	m_szCurAnimName = "";

	_vector m_vLookPoint;

	Vector3	m_vMoveDir;		// ���� �̵��Ϸ� �ϴ� ����

	Vector3	m_vLocalMove;	// ���� ������ �̵��ϰ� �ִ� ����
	Matrix	m_matCamRot;

	_uint	m_eMoveDir;

	CAnimationStateMachine* m_pASM_Base = nullptr;

protected:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.1f;
	_bool	m_bLerp = false;
	_bool	m_bAttach = false;

public:
	static CBaseAnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END