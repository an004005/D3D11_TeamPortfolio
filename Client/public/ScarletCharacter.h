#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "SoundStore.h"
#include "HelperClasses.h"
#include "VFX_Manager.h"
#include "TimerHelper.h"

BEGIN(Engine)
class CControlledRigidBody;
class CRigidBody;
END

BEGIN(Client)

enum ECOPYCOLTYPE { COPYCOL_MAIN, COPYCOL_LEFT, COPYCOL_RIGHT, COPYCOL_END };

class CScarletCharacter abstract : public CGameObject
{
protected:
	CScarletCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScarletCharacter(const CScarletCharacter& rhs);
	virtual ~CScarletCharacter() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;

public:
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams){}
	_bool IsOnFloor() const { return m_bOnFloor; }
	// 경직 또는 죽을 때 실행하여 상태 초기화하는 함수
	// ex) 공격 sweep 중단, 상태이상 제거 등등
	virtual void Reset() { }

public:
	_float4	GetColliderPosition();

	// 피직스 무시하는 강제 이동
	void SetWorldMatrix(_float4x4 WorldMatrix);
	void SetForcePos(_float4 vPos);

public:
	EDeBuffType GetDeBuffType() const { return m_eDeBuff; }
protected:
	virtual void Update_DeBuff(_double TimeDelta);
	virtual void DeBuff_End(){}
	virtual void DeBuff_Fire(){}
	virtual void DeBuff_Oil(){}
	virtual void DeBuff_Thunder() {}
	virtual void DeBuff_Water() {}
protected:
	void		Collision_Check_Capsule(CRigidBody*	AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollision = true, ECOLLIDER_TYPE_BIT ColType = CTB_MONSTER, _bool bContinuity = false);
	_bool		Collision_Check_Capsule_Improved(CRigidBody* AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollisionCheck = false, ECOLLIDER_TYPE_BIT ColType = CTB_MONSTER, ECOPYCOLTYPE eCopyType = COPYCOL_MAIN);
	list<CScarletCharacter*>	m_DamagedObjectList;

protected:
	CControlledRigidBody* m_pCollider = nullptr;
	CGameInstance* m_pGameInstance = nullptr;
	_float4 m_vPrePos;
	_bool m_bPreOnFloor = false; // 이전 프레임에서 땅을 밝고 있는지 확인용
	_bool m_bOnFloor = false; // 현 프레임에서 땅 밝고 있는지 여부
	_bool m_bOnSide = false;

	_bool m_bActiveGravity = true;
	_float m_fYSpeed = 0.f;
	_float m_fGravity = 20.f;

	// Debuff
	EDeBuffType m_ePreDeBuff = EDeBuffType::DEBUFF_END;
	EDeBuffType m_eDeBuff = EDeBuffType::DEBUFF_END;
	_float m_fDeBuffTime = 0.f;
	// ~Debuff

	CSoundStore m_SoundStore;

	_bool m_bAwayFromBelowObject = true;

// 재충돌 여부, Out이 아니면 재충돌로 판단
public:
	_bool	Get_CollisionDuplicate() { return m_bCollisionDuplicate; }
	void	Set_CollisionDuplicate(_bool bDuplicate) { m_bCollisionDuplicate = bDuplicate; }

	_bool	Get_CollisionDuplicate(ECOPYCOLTYPE eType) { return m_vecDuplication[eType]; }
	_float	Get_CollisionDuplicateCool(ECOPYCOLTYPE eType) { return m_vecDuplicationCool[eType]; }
	void	Set_CollisionDuplicate(_bool bDuplicate, ECOPYCOLTYPE eType = COPYCOL_MAIN) { m_vecDuplication[eType] = bDuplicate; }	//원본
	void	Set_CollisionDuplicateCool(_float fCool, ECOPYCOLTYPE eType = COPYCOL_MAIN) { m_vecDuplicationCool[eType] = fCool; }	// 내부쿨

protected:
	_bool			m_bCollisionDuplicate = false;		// true이면 여전히 충돌중인것임
	vector<_bool>	m_vecDuplication = { false, false, false };
// ~재충돌 여부

// 충돌 내부쿨, 원인 파악 안되면 이걸로 하기
protected:
	vector<_float>	m_vecDuplicationCool = { 0.f, 0.f, 0.f };
// ~충돌 내부쿨, 원인 파악 안되면 이걸로 하기

protected:
	_uint	m_iHitTargetCount = 0;
	Vector4 m_BeforePos;

public:
	// virtual HRESULT Render_ShadowDepth() override;

public:
	virtual void Free() override;
};

END