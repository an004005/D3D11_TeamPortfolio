#pragma once
#include "Client_Defines.h"
#include "AnimationInstance.h"

BEGIN(Client)

class CAI_CH0300_AnimInstance : public CAnimationInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void UpdateTargetState(_double TimeDelta) override;

	virtual void Imgui_RenderState() override;

public:
	string GetCurStateName() const { return m_pASM_Base->GetCurState()->m_strName; }

private:
	void SpairAnimationManager(_bool bBattle);

private:
	_bool CheckAnim(const string& strAnimName);
	_bool CheckSpairExist(const string& strAnimName);
	_bool isAnimFinish(const string& strAnimName);

private:
	_float	DistanceCheck();

private:
	CAnimationStateMachine* m_pASM_Base = nullptr;

private:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.2f;
	_float	m_fSeperateLerpTime = 0.f;
	_bool	m_bLerp = false;
	_bool	m_bAttach = false;

private:
	_bool	m_bAir = false;
	_bool	m_bWalk = false;
	_bool	m_bOnBattle = false;
	_bool	m_bOnFloor = false;
	_bool	m_bBattle = false;

private:
	_float	m_fDistance_toPlayer = -1.f;
	_float	m_fDistance_toEnemy = -1.f;

public:
	static CAI_CH0300_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END