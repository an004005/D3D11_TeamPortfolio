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
	void	KineticComboStateMachine_ReCompile();

private:
	void	Player_Something_Update();

private:
	CPlayer* m_pPlayer = nullptr;

public:
	static CPlayerHotFixer* Create(CPlayer* pPlayer);
	virtual void Free();
};

END