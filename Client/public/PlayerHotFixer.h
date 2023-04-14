#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "HelperClasses.h"
#include "Timeline.h"

BEGIN(Client)

class CPlayer;

class CPlayerHotFixer : public CBase
{
	CPlayerHotFixer();
	CPlayerHotFixer(const CPlayerHotFixer& rhs) = default;
	virtual ~CPlayerHotFixer() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer);
	void			Tick();

private:
	void	BrainCrashStateMachine_ReCompoile();
	void	BrainFieldStateMachine_ReCompile();
	void	DeBuffStateMachine_ReCompile();
	void	HitStateMachine_ReCompile();

private:
	void	Player_Something_Update();
	void	Additive_Test();

private:
	_bool	m_bShakeSmall = false;
	_bool	m_bShakeMiddle = false;
	_bool	m_bShakeHeavy = false;

	_bool	m_bAdditiveOn = false;

	_bool	m_bBrainMap_Combo4 = false;
	_bool	m_bBrainMap_Air = false;
	_bool	m_bBrainMap_BF = false;

private:
	_float4x4 m_Pivot = XMMatrixIdentity();

private:
	CPlayer* m_pPlayer = nullptr;

private:
	CDoOnce	EffectTest_01;
	CDoOnce	EffectTest_02;
	CDoOnce	EffectTest_03;
	CDoOnce	EffectTest_04;
	CDoOnce	EffectTest_05;

private:
	CSimpleTimeline	m_ActionCamStart;
	_float			m_fActionCamOut = 0.f;

private:
	_float			m_fCamDistance = 4.f;

private:
	_bool			m_bHanabiActive = false;
	_bool			m_bTsugumiActive = false;

public:
	static CPlayerHotFixer* Create(CPlayer* pPlayer);
	virtual void Free();
};

END