#pragma once
#include "Client_Defines.h"
#include "ScarletCharacter.h"

BEGIN(Engine)
class CRenderer;
class CModel;
END

BEGIN(Client)

class CMonster abstract : public CScarletCharacter
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CScarletCharacter& rhs);
	virtual ~CMonster() = default;

public:
	virtual void Tick(_double TimeDelta) override;

protected:
	_bool CheckDamagedTarget(CScarletCharacter* pTarget);
	void ClearDamagedTarget();
	void HitTargets(physx::PxSweepBuffer& sweepOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END);
	void HitTargets(physx::PxOverlapBuffer& overlapOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END);

	virtual void Update_DeadDissolve(_double TimeDelta);
	virtual void DeBuff_End() override;
	virtual void DeBuff_Fire() override;
	virtual void DeBuff_Oil() override;

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	_int m_iHP = 100;
	_int m_iMaxHP = 100;

	_int m_iStamina = 100;
	_int m_iMaxStamina = 100;

	_bool m_bDead = false;
	_float m_fDeadDissolve = 0.5f;

	set<CScarletCharacter*> m_DamagedTargetList;


public:
	virtual void Free() override;
};

END