#pragma once
#include "Base.h"
#include "Client_Defines.h"

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
	CPlayer* m_pPlayer = nullptr;

public:
	static CPlayerHotFixer* Create(CPlayer* pPlayer);
	virtual void Free();
};

END