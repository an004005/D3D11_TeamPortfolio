#pragma once
#include "Client_Defines.h"
#include "Enemy_AnimInstance.h"

BEGIN(Client)

class CEM1200_AnimInstance : public CEnemy_AnimInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void UpdateTargetState(_double TimeDelta) override;

private:
	void Make2PhaseASM();

private:
	_bool			m_bMove = false;
	_bool			m_bRun = false;
	_bool			m_bChangePhase = false;
	EBaseTurn		m_eTurn = EBaseTurn::TURN_END;

public:
	static CEM1200_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END