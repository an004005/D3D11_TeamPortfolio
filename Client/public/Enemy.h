#pragma once
#include "ScarletCharacter.h"
#include "Client_Defines.h"
#include "Timeline.h"
#include "MathUtils.h"
#include "EMUI.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CFSMComponent;
END

BEGIN(Client)
using RigidBodies = unordered_map<string, CRigidBody*>;

class CEnemy abstract : public CScarletCharacter
{

protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CScarletCharacter& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void Imgui_RenderProperty() override;
	virtual HRESULT Render_ShadowDepth() override;

	virtual void SetUpComponents(void* pArg);
	virtual void SetUpSound();
	virtual void SetUpAnimationEvent(){}
	virtual void SetUpFSM() {}
	virtual void SetUpUI();

	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	virtual void SetEnemyBatchDataStat(ENEMY_STAT tStat);
	virtual ENEMY_STAT GetEnemyBatchDataStat();
	
public:
	//0 ~ 1 사이값
	_float GetHpRatio() const { return (_float)m_iHP / (_float)m_iMaxHP; }
	_float GetCrushGageRatio() const { return (_float)m_iCrushGauge / (_float)m_iMaxCrushGauge; }
	CEMUI* GetEnemyUI() const { return m_pEMUI; }
	const _int	GetEnemyLevel() const { return iEemeyLevel; }
	const EEnemyName	GetEnemyName() const { return m_eEnemyName; }


	_bool IsDead() const { return m_bDead; }
	virtual _float4	GetKineticTargetPos() { return GetColliderPosition(); }

	void HealFullHp() { m_iHP = m_iMaxHP; }
	_bool	HasCrushGauge() { return m_bHasCrushGauge; }
	_bool CanBC() { return m_bCrushStart; }

	//몬스터마다 브레인 크러쉬 시작할때 위치를 잡아주고, 언제 끝날껀지 결정해야 함
	 virtual void	PlayBC() { m_bBrainCrush = true; }

public:
	virtual _float4x4 GetBoneMatrix(const string& strBoneName, _bool bPivotapply = true) override;
	virtual _float4x4 GetPivotMatrix() override;

	//몬스터마다 특정 조건을 만들고싶을때
	virtual _bool		Exclude() { return false; }

	//Target 방향 확인
	_bool IsTargetFront(_float fAngle = 90.f);
	_bool IsTargetRight(_float fAngle = 90.f);

//public:
//	void Set_Gravity(_bool bGravity) { m_bActiveGravity = bGravity; }

protected:
	// take damage 관련 함수
	void CheckHitPositoin(DAMAGE_PARAM& tDamageParams);
	virtual void HitEffect(DAMAGE_PARAM& tDamageParams);
	virtual void CheckDeBuff(EDeBuffType eDeBuff);
	virtual _bool IsWeak(CRigidBody* pHitPart) { return false; }
	virtual void CheckCrushGage(DAMAGE_PARAM& tDamageParams);
	virtual void CheckHP(DAMAGE_PARAM& tDamageParams);
	virtual _bool CheckSASType(ESASType eSASType);
	// Tick의 제일 마지막에서 실행한다.
	void ResetHitData();
	// ~

	// 몬스터가 죽으면 실행해야할 코드들 넣기
	virtual void SetDead();

	virtual void FindTarget();
	virtual void Update_DeadDissolve(_double TimeDelta);

	virtual void Update_DeBuff(_double TimeDelta) override;
	virtual void DeBuff_End() override;
	virtual void DeBuff_Fire() override;
	virtual void DeBuff_Oil() override;
	virtual void MoveJsonData(Json& jsonDest, void* pArg);

	_bool CheckDamagedTarget(CScarletCharacter* pTarget);
	void ClearDamagedTarget();
	void HitTargets(physx::PxSweepBuffer& sweepOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END);
	void HitTargets(physx::PxOverlapBuffer& overlapOut, _int iDamage, EAttackType eAtkType, EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END);

	//로컬움직임 적용. 다른 애니메이션을 넣어주기 전에 ClearSocketAnim 꼭 해줘야함!!
	void SocketLocalMove(class CEnemy_AnimInstance* pASM);

	//RigidBoyd 관리
	void	Add_RigidBody(const string& KeyName, void* pArg = nullptr);
	CRigidBody* GetRigidBody(const string& KeyName);

private:
	void CreateSpawnEffect();

protected:
	static vector<wstring>			s_vecDefaultBlood;
	static vector<wstring>			s_vecFireBlood;
	static vector<wstring>			s_vecElecBlood;

	static vector<wstring>			s_vecDefaultHit;
	static vector<wstring>			s_vecFireHit;
	static vector<wstring>			s_vecElecHit;

	static vector<wstring>			s_vecDefaultDecal;
	static vector<wstring>			s_vecFireDecal;
	static vector<wstring>			s_vecElecDecal;

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CScarletCharacter*		m_pTarget = nullptr;
	CFSMComponent*			m_pFSM = nullptr;
	RigidBodies				m_pRigidBodies;
	CEMUI* m_pEMUI = nullptr;

	//

	_bool m_bFindTestTarget = false;

	_int m_iAtkDamage = 50;
	_int m_iHP = 100;
	_int m_iMaxHP = 100;

	_int m_iCrushGauge = 100;
	_int m_iMaxCrushGauge = 100;
	_bool m_bHasCrushGauge = false;

	//플레이어가 set해줌
	_bool	m_bBrainCrush = false;

	_bool		m_bCrushStart = false;
	_double	m_dDeadTime = 0.0;

	_bool m_bDead = false;
	set<CScarletCharacter*> m_DamagedTargetList;
	CSimpleTimeline m_DeathTimeline;

	_uint	iEemeyLevel = { 0 };
	EEnemyName m_eEnemyName = { EEnemyName::ENEMY_NAME_END };

	// 
	EAttackType m_eCurAttackType = EAttackType::ATK_END; // 현 프레임에서 받은 공격 타입
	EBaseAxis m_eHitFrom = EBaseAxis::AXIS_END;
	ESimpleAxis m_eSimpleHitFrom = ESimpleAxis::AXIS_END;
	_bool m_bHitWeak = false;
	_bool m_bAirToDown = false;
	//

	_float m_fFireResist = 0.6f; // 높을 수록 안걸림
	_float m_fThunderResist = 0.6f; // 높을 수록 안걸림
	CCoolTimeHelper m_FireTick;

	string m_strImpactTag = "fx_impact_flesh";
	string m_strDeathSoundTag;
	string m_strImpactVoiceTag;



	_bool m_bSpawnEffect = true;
	_float4x4 m_SpawnEffectPivot;
	_float m_fSpawnDistortionDistancePivot = 0.f;

public:
	virtual void Free() override;
};

END