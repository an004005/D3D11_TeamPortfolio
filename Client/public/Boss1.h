#pragma once
#include "Monster.h"
#include "Client_Defines.h"
#include "Timeline.h"

BEGIN(Engine)
class CFSMComponent;
class CAnimation;
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

class CBoss1 : public CMonster
{
	enum CBoss1_AttackStateType
	{
		LEFT_SWEEP,
		RIGHT_SWEEP,
		SPIN,
		JUMP,
		STATE_END
	};

private:
	CBoss1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss1(const CBoss1& rhs);
	virtual ~CBoss1() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void SetUp_UI() override;
	
	virtual void Imgui_RenderProperty() override;

	virtual void AfterPhysX() override;
	
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	virtual _float4	GetKineticTargetPos() override;

	_matrix AttachCollider(CRigidBody* pRigidBody);

	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;

	void Start_AttackState(CBoss1_AttackStateType eState);
	void Tick_AttackState();
	void End_AttackState();
	virtual void Reset() override;

	virtual void DeBuff_End() override;
	virtual void DeBuff_Fire() override;
	virtual void DeBuff_Oil() override;

//	virtual void SetActive() override;
private:
	void Create_BossUI();

	void SmokeEffectCreate();
	void JitabataSmokeEffect();

private:
	class CBoss1_AnimationInstance* m_pASM = nullptr;
	class CBoss1_AIController*		m_pController = nullptr;

	class CRigidBody* m_pWeak = nullptr;

	class CRigidBody* m_pHead = nullptr;

	class CRigidBody* m_pLeftArm = nullptr;
	class CRigidBody* m_pRightArm = nullptr;
	

	CRigidBody* m_pRange = nullptr;

	CScarletCharacter* m_pTarget = nullptr;

	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	_bool m_bDown = false;
	_bool m_bMiddleDown = false;

	_bool m_bTest = false;

	_bool m_bJumpAttack = false;
	_float3 m_vOnJumpMoveVelocity;
	_float m_fJumpMoveTime = 0.f;
	_int m_iJitabaCnt = 0;

	CBoss1_AttackStateType m_eAttackType = STATE_END;
	_float4 m_vSweepPrePos;

	CAnimation* m_pAtk_R = nullptr;
	CAnimation* m_pAtk_L = nullptr;
	CAnimation* m_pAtk_Spin = nullptr;
	CAnimation* m_pAtk_WaterBall = nullptr;
	CAnimation* m_pMiddleDown = nullptr;
	CAnimation* m_pDownStart = nullptr;
	CAnimation* m_pDown = nullptr;

	CAnimation* m_pJumpStart = nullptr;
	CAnimation* m_pJumpEnd = nullptr;
	CAnimation* m_pJumpLand = nullptr;
	CAnimation* m_pJumpJitabata = nullptr;

	CAnimation* m_pDeadAnim = nullptr;

	vector<CMaterial*> m_BodyMtrls;
	CMaterial* m_pWeakMtrl = nullptr;
	CMaterial* m_pGlassMtrl = nullptr;
		
	// Add PJW
	_vector	m_LastSpotTargetPos;
	_bool	m_b2ndPhase = false;
	_int	m_iPreHP;

	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;

	//UI
	class CCanvas_Alarm*		m_pUI_Alarm = nullptr;
	class CCanvas_BossHpMove*	m_pUI_BossHP = nullptr;

public:
	static CBoss1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END