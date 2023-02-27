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

protected:	// 대상의 상태
	_bool			m_bIdle = false;
	
	// Move (4-Direct)
	_bool			m_bMoveF = false;
	_bool			m_bMoveB = false;
	_bool			m_bMoveL = false;
	_bool			m_bMoveR = false;
	_bool			m_bRandomMove = false;

	_bool			m_bAttackStart = false;
	_bool			m_bAttacking = false;
	_bool			m_bAttackEnd = false;

	_bool			m_bThreat = false;

	// Socket
	_bool			m_bDamage = false;
	_bool			m_bDead = false;

	// ASM Control
	_bool			m_bStatic = false;
	_bool			m_bInitialize = false;

	_float			m_fPlayRatio = 0.f;

protected:
	CAnimationStateMachine* m_pASM_Base = nullptr;

protected:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.2f;

public:
	static CSkPd_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END