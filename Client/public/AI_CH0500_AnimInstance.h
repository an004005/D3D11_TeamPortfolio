#pragma once
#include "Client_Defines.h"
#include "AnimationInstance.h"
#include "AI_CH0500.h"

BEGIN(Client)

class CAI_CH0500_AnimInstance : public CAnimationInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void UpdateTargetState(_double TimeDelta) override;

	virtual void Imgui_RenderState() override;

public:
	string GetCurStateName() const { return m_pASM_Base->GetCurState()->m_strName; }
	void   SetCurState(const string& strStateName) { m_pASM_Base->SetCurState(strStateName); }

private:
	void SpairAnimationManager(_bool bBattle);

private:
	_bool CheckAnim(const string& strAnimName);
	_bool CheckSpairExist(const string& strAnimName);
	_bool isAnimFinish(const string& strAnimName);

private:
	_float	DistanceCheck();
	void	Reset_HitCheck();

private:
	CAnimationStateMachine* m_pASM_Base = nullptr;

private:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.2f;
	_float	m_fSeperateLerpTime = 0.f;
	_bool	m_bLerp = false;
	_bool	m_bAttach = false;

private:
	CAI_CH0500::DAMAGE_DESC m_tDamageDesc;

private:
	_bool	m_bAir = false;
	_bool	m_bWalk = false;
	_bool	m_bOnBattle = false;
	_bool	m_bOnFloor = false;
	_bool	m_bBattle = false;
	_bool	m_bHit = false;
	_bool	m_bJump = false;

private:
	_float	m_fDistance_toPlayer = -1.f;
	_float	m_fDistance_toEnemy = -1.f;

public:
	static CAI_CH0500_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END