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

// 경견페리

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

	_bool IsCanSpecial() const { return m_bCanSpecial; }

protected:
	virtual _bool IsWeak(CRigidBody* pHitPart) override;
	virtual void CheckHP(DAMAGE_PARAM& tDamageParams) override;
	void FireWaterBall();

//	virtual void SetActive() override;
private:
	void SmokeEffectCreate();
	void JitabataSmokeEffect();
	void CreateWeakExplosionEffect();

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
	_bool m_bCanSpecial = false;
	//

	CController::EHandleInput m_eInput;

	_float4 m_vSweepPrePos;
	EAttackStateType m_eEnemyAttackType = EAttackStateType::STATE_END;

	vector<CMaterial*> m_BodyMtrls;
	CMaterial* m_pWeakMtrl = nullptr;
	CMaterial* m_pGlassMtrl = nullptr;
	CMaterial* m_pWaterMtrl = nullptr;
	_float m_fWeakHitFlash = 0.f;
	_float m_fWaterRegenTime = 0.f;
		
	// Add PJW
	_vector	m_LastSpotTargetPos;
	_bool	m_b2ndPhase = false;
	_int	m_iPreHP;

	//UI
//	class CCanvas_BossHpMove*		m_pUI_BossHP = nullptr;


	_float m_fWeakExplosionTickTime = 0.3f;
	_uint m_fWeakExplosionCnt = 0;

	_float4x4 pivot;

	_bool	m_bWeakTalk = { false };

public:
	static CEM0320* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END