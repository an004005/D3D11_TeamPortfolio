#pragma once
#include "Enemy.h"
#include "Client_Defines.h"
#include "Controller.h"
#include "Timeline.h"

BEGIN(Engine)
class CFSMComponent;
class CAnimation;
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

class CEM0320 : public CEnemy
{
	enum EAttackStateType
	{
		LEFT_SWEEP,
		RIGHT_SWEEP,
		SPIN,
		JUMP,
		STATE_END
	};
private:
	CEM0320(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0320(const CEM0320& rhs);
	virtual ~CEM0320() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void SetUpComponents(void* pArg) override;
	virtual void SetUpSound() override;
	virtual void SetUpAnimationEvent() override;
	virtual void SetUpFSM() override;

	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	// virtual void SetUp_UI() override;
	
	virtual void Imgui_RenderProperty() override;

	virtual void AfterPhysX() override;
	
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	virtual _float4	GetKineticTargetPos() override;

	_matrix AttachCollider(CRigidBody* pRigidBody);

	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;

	void Start_AttackState(EAttackStateType eState);
	void Tick_AttackState();
	void End_AttackState();
	void Reset();

	virtual void DeBuff_End() override;
	virtual void DeBuff_Fire() override;
	virtual void DeBuff_Oil() override;

protected:
	virtual _bool IsWeak(CRigidBody* pHitPart) override;
	virtual void CheckHP(DAMAGE_PARAM& tDamageParams) override;
	void FireWaterBall();

//	virtual void SetActive() override;
private:

	void Create_BossUI();

	void SmokeEffectCreate();
	void JitabataSmokeEffect();

private:
	class CEM320_AnimInstance* m_pASM = nullptr;
	class CEM0320_Controller*		m_pController = nullptr;

	class CRigidBody* m_pWeak = nullptr;

	class CRigidBody* m_pHead = nullptr;

	class CRigidBody* m_pLeftArm = nullptr;
	class CRigidBody* m_pRightArm = nullptr;
	CRigidBody* m_pRange = nullptr;
	_float fangle = 10.f;
	//
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
	//

	_bool m_bDown = false;
	_bool m_bMiddleDown = false;

	//
	_bool m_bJumpAttack = false;
	_float3 m_vOnJumpMoveVelocity;
	_float m_fJumpMoveTime = 0.f;
	_int m_iJitabaCnt = 0;
	//

	CController::EHandleInput m_eInput;

	_float4 m_vSweepPrePos;
	EAttackStateType m_eEnemyAttackType = EAttackStateType::STATE_END;

	vector<CMaterial*> m_BodyMtrls;
	CMaterial* m_pWeakMtrl = nullptr;
	CMaterial* m_pGlassMtrl = nullptr;
	_float m_fWeakHitFlash = 0.f;
		
	// Add PJW
	_vector	m_LastSpotTargetPos;
	_bool	m_b2ndPhase = false;
	_int	m_iPreHP;


	//UI
	class CCanvas_Alarm*		m_pUI_Alarm = nullptr;
	class CCanvas_BossHp*		m_pUI_BossHP = nullptr;
public:
	static CEM0320* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END