#pragma once
#include <Timeline.h>

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
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Imgui_RenderProperty() override;
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;

public:
	//For Effect
	virtual _float4x4 GetBoneMatrix(const string& strBoneName, _bool bPivotapply = true) override;
	virtual _float4x4 GetPivotMatrix() override;
	//~For Effect
	_bool IsDead() const { return m_bDead; }
	_bool IsActive() const { return m_bActive; }
	virtual void SetActive();

protected:
	_bool CheckDamagedTarget(CScarletCharacter* pTarget);
	void ClearDamagedTarget();
	void HitTargets(physx::PxSweepBuffer& sweepOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END);
	void HitTargets(physx::PxOverlapBuffer& overlapOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END);

	virtual void Update_DeadDissolve(_double TimeDelta);
	virtual void DeBuff_End() override;
	virtual void DeBuff_Fire() override;
	virtual void DeBuff_Oil() override;

	void MoveTransformJson(Json& jsonDest, void* pArg);

	

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	_int m_iHP = 100;
	_int m_iMaxHP = 100;

	_int m_iStamina = 100;
	_int m_iMaxStamina = 100;

	_bool m_bDead = false;

	set<CScarletCharacter*> m_DamagedTargetList;

	CSimpleTimeline m_DeathTimeline;

	_bool m_bActive = false;


public:
	virtual void Free() override;
};

END