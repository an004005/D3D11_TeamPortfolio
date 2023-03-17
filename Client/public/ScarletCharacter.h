#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "SoundStore.h"

BEGIN(Engine)
class CControlledRigidBody;
class CRigidBody;
END

BEGIN(Client)

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
	_float4	GetColliderPosition(); // 

public:
	EDeBuffType GetDeBuffType() const { return m_eDeBuff; }
protected:
	virtual void Update_DeBuff(_double TimeDelta);
	virtual void DeBuff_End(){}
	virtual void DeBuff_Fire(){}
	virtual void DeBuff_Oil(){}

protected:
	void		Collision_Check_Capsule(CRigidBody*	AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollision = true, ECOLLIDER_TYPE_BIT ColType = CTB_MONSTER, _bool bContinuity = false);
	_bool		Collision_Check_Capsule_Improved(CRigidBody* AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollisionCheck = false, ECOLLIDER_TYPE_BIT ColType = CTB_MONSTER);
	list<CScarletCharacter*>	m_DamagedObjectList;

protected:
	CControlledRigidBody* m_pCollider = nullptr;
	CGameInstance* m_pGameInstance = nullptr;
	_float4 m_vPrePos;
	_bool m_bOnFloor = false;
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

// 재충돌 여부, Out이 아니면 재충돌로 판단
public:
	_bool	Get_CollisionDuplicate() { return m_bCollisionDuplicate; }
	void	Set_CollisionDuplicate(_bool bDuplicate) { m_bCollisionDuplicate = bDuplicate; }

protected:
	_bool	m_bCollisionDuplicate = false;		// true이면 여전히 충돌중인것임
// ~재충돌 여부

protected:
	_uint	m_iHitTargetCount = 0;
	Vector4 m_BeforePos;

public:
	// virtual HRESULT Render_ShadowDepth() override;

public:
	virtual void Free() override;
};

END