#pragma once
#include <Timeline.h>

#include "Client_Defines.h"
#include "ScarletCharacter.h"
//#include "ParticleGroup.h"
//#include "EffectGroup.h"

BEGIN(Engine)
class CRenderer;
class CModel;
END

BEGIN(Client)

enum MONSTER_UIPIVOT { INFOBAR, FINDEYES, UIPIVOT_END};
class CMonster abstract : public CScarletCharacter
{
public:
	enum MONSTER_NAME { BRONJON, SKUMMYPANDOU, SKUMMYPOOL, BUDDYLUMI, FLOWERLEG, GODLYFERRY, MONSTERNAME_END };

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
	virtual _float4	GetKineticTargetPos() { return GetColliderPosition(); }

public:
	_uint GetHP() { return m_iHP; }
	_uint GetMaxHP() { return m_iMaxHP; }
	_bool GetHasName() { return m_bHasName; }
	_float4x4	Get_UIPivotMatrix(MONSTER_UIPIVOT eUIPivot) {
		return m_UI_PivotMatrixes[eUIPivot];
	}

public:
	//UI
	void	TurnEyesOut();
	virtual void SetUp_UI() {}
	void	Set_HasName() { m_bHasName = true; }

	_int Get_MonsterLevel() {
		return iMonsterLevel;
	}
	_int Get_MonsterName() {
		return _int(m_eMonsterName);
	}

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

protected:	// 피격 이펙트 색상
	vector<wstring>			m_vecDefaultHit;
	vector<wstring>			m_vecFireHit;
	vector<wstring>			m_vecElecHit;
	

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	_int m_iHP = 100;
	_int m_iMaxHP = 100;

	_int m_iStamina = 100;
	_int m_iMaxStamina = 100;

	_bool m_bDead = false;
	_bool m_bActive = false;
	_bool m_bHasName = false;

	array<_float4x4, UIPIVOT_END> m_UI_PivotMatrixes;

	set<CScarletCharacter*> m_DamagedTargetList;
	CSimpleTimeline m_DeathTimeline;
	_uint	iMonsterLevel = { 0 };
	MONSTER_NAME m_eMonsterName = { MONSTERNAME_END };
	


public:
	virtual void Free() override;
};

END