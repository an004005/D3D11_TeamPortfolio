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
	const string&	GetCurAnimName() { return m_pASM_Base->GetCurState()->m_Animation->GetName(); } // 현재 재생중인 애니메이션 이름은 모델에서 가져올 것 -> State Machine은 Transition이 겹치면 통과함
	const string&	GetCurStateName() { return m_pASM_Base->GetCurState()->m_strName; } // 현재 상태 이름
	const string&	GetCurSocketAnimName();
	_bool			isLerping() { return m_pASM_Base->isLerping(); }
	_bool			isSocketLerping() { return m_bBaseTickCheck; }
	_bool			isSocketEmpty(const string& strSocName) { return m_mapAnimSocket[strSocName].empty(); }
	_bool			isSocketAlmostFinish(const string& strSocName);
	_bool			isSocketPassby(const string& strSocName, _float fPlayRatio = 1.f);
	_bool			CheckSocketAnim(const string& strSocName, const string& AnimName) { return (AnimName == m_mapAnimSocket[strSocName].front()->GetName()) ? true : false; }
	_bool			isSocketExactlyEmpty();	// 모든 소켓이 비어있는지?
	_uint			GetSocketSize(const string& strSocName) { return m_mapAnimSocket[strSocName].size(); }
	void			ClearAnimSocket(const string& strSocName = "");

	void			SetCurState(const string& szStateName) { m_pASM_Base->SetCurState(szStateName); }

protected:
	void			SpairAnimationManager();

protected:
	_bool	CheckAnim(const string& szAnimName);

protected:	// 대상의 상태
	_bool	m_bAir = false;
	_bool	m_bPreAir = false;

	_bool	m_bMove = false;
	_bool	m_bWalk = false;
	_bool	m_bOptionalMove = false;

	_bool	m_bLeftClick = false;
	_bool	m_bDash = false;

	_bool	m_bJump = false;
	_bool	m_bUpper = false;

	_bool	m_bNonCharge = false;
	_bool	m_bCharge = false;

	_bool	m_bSeperateAnim = false;
	_bool	m_bOnBattle = false;
	_bool	m_bBefSasUsingCheck[8];

	_bool	m_bOnFloor = false;

	_float	m_fPlayRatio = 0.f;

	string	m_szCurAnimName = "";

	_vector m_vLookPoint;

	Vector3	m_vMoveDir;		// 실제 이동하려 하는 방향

	Vector3	m_vLocalMove;	// 로컬 축으로 이동하고 있는 방향
	Matrix	m_matCamRot;

	_uint	m_eMoveDir;

	_float	m_fYSpeed = 0.f;

	CAnimationStateMachine* m_pASM_Base = nullptr;

protected:
	string	m_szNullString = "";

protected:
	_bool	FloorCheck();
	_float	m_fFloorCheck = 0.f;

protected:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.2f;
	_float	m_fSeperateLerpTime = 0.f;
	_bool	m_bLerp = false;
	_bool	m_bAttach = false;
	_bool	m_bSeperateSwitch = false;
	_bool	m_bBaseTickCheck = false;

public:
	static CBaseAnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END