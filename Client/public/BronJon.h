#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
class CAnimation;
class CRigidBody;
class CGameInstance;
END

BEGIN(Client)

class CBronJon : public CMonster
{
private:
	CBronJon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBronJon(const CBronJon& rhs);
	virtual ~CBronJon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	virtual void AfterPhysX();

	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;

	_matrix AttachCollider(CRigidBody* pRigidBody);

	void	Atk_BiteSweep();

	void	Atk_LaserSweep();

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom = nullptr;

	class CBrJ_Controller*		m_pController = nullptr;
	class CBrJ_AnimInstance*	m_pASM = nullptr;	

	CRigidBody*					m_pTrigger = nullptr;
	CRigidBody*					m_pJawRBody = nullptr;		// Bite + Head HitBox
	CRigidBody*					m_pLaserEffect = nullptr;	// Laser
	CRigidBody*					m_pLeftArm = nullptr;
	CRigidBody*					m_pRightArm = nullptr;

private:
	CScarletCharacter* m_pTarget = nullptr;

	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	// 소켓 애니메이션 보관
	
	CAnimation* m_pAtk_LaserStart = nullptr;		// 레이저 공격
	CAnimation* m_pAtk_LaserLoop = nullptr;
	CAnimation* m_pAtk_LaserEnd = nullptr;
	CAnimation* m_pAtk_Bite = nullptr;		// 물기 공격

	CAnimation* m_pThreat = nullptr;

	CAnimation* m_pDamage_L_F = nullptr;
	CAnimation* m_pDamage_L_B = nullptr;

	// Heavy (염력 데미지)
	CAnimation* m_pDamage_M_F = nullptr;
	CAnimation* m_pDamage_M_B = nullptr;
	CAnimation* m_pDamage_M_L = nullptr;
	CAnimation* m_pDamage_M_R = nullptr;
	
	// Groggy (염력 5회 타격당했을 때)
	CAnimation* m_pDownStart = nullptr;
	CAnimation* m_pDownLoop = nullptr;
	CAnimation* m_pGetUp = nullptr;

	CAnimation* m_pDeadAnim = nullptr;
	
	// Attack
	_bool m_bAtkBite = false;
	_bool m_bAtkLaser = false;
	list<CScarletCharacter*> m_CollisionList;


	// Damage
	_bool		m_bStruck = false;
	_bool		m_bGroggy = false; // AirStruck이 없는 대신에 Groggy 상태가 존재함.
	_bool		m_bDown = false;
	
	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;

	_uint		m_iGroggyCnt = 0;
	_uint		m_iGroggy_Able = 5;
	
public:
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;

	public:
		static CBronJon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
		virtual CGameObject* Clone(void* pArg = nullptr) override;
		virtual void Free() override;
};

END