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

public:
	void	InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList);
	void			AttachAnimSocket(const string& strSocName, list<CAnimation*> AnimList);
	const string&	GetCurAnimName() { return m_pASM_Base->GetCurState()->m_Animation->GetName(); } // 현재 재생중인 애니메이션 이름은 모델에서 가져올 것 -> State Machine은 Transition이 겹치면 통과함
	_bool			isLerping() { return m_pASM_Base->isLerping(); }
	_bool			isSocketEmpty(const string& strSocName) { return m_mapAnimSocket[strSocName].empty(); }
	_bool			isSocketAlmostFinish(const string& strSocName);
	_bool			isSocketPassby(const string& strSocName, _float fPlayRatio = 1.f);
	_bool			CheckSocketAnim(const string& strSocName, const string& AnimName) { return (AnimName == m_mapAnimSocket[strSocName].front()->GetName()) ? true : false; }

protected:

	_bool	CheckAnim(const string& szAnimName);

	_float	Get_fSaveRatio() const { return m_fSaveRatio; }

protected:	// 대상의 상태
	_bool			m_bIdle = false;
	
	// Move (4-Direct)
	_bool			m_bMoveF = false;
	_bool			m_bMoveB = false;
	_bool			m_bMoveL = false;
	_bool			m_bMoveR = false;

	_bool			m_bRandomMove = false;
	_bool			m_bCurrentRatioSave = false;

	_bool			m_bAttackStart = false;
	_bool			m_bAttacking = false;
	_bool			m_bAttackEnd = false;

	_bool			m_bThreat = false;

	// Socket
	_bool			m_bDamage = false;
	_bool			m_bDead = false;

	_bool			m_bLerpTurn = false;

	// ASM Control
	_bool			m_bStatic = false;
	_bool			m_bInitialize = false;

	_float			m_fPlayRatio = 0.f;
	_float			m_fSaveRatio = 0.f;

protected:
	CAnimationStateMachine* m_pASM_Base = nullptr;

protected:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.2f;

	_bool	m_bLerp = false;
	_bool	m_bAttach = false;

	Vector3	m_vLocalMove;

public:
	static CSkPd_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END