#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

// ����

class CEM0900 : public CEnemy
{
private:
	CEM0900(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0900(const CEM0900& rhs);
	virtual ~CEM0900() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void SetUpComponents(void* pArg) override;
	virtual void SetUpSound() override;
	virtual void SetUpAnimationEvent() override;
	virtual void SetUpFSM() override;

	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual _bool IsWeak(CRigidBody* pHitPart);
	virtual void CheckHP(DAMAGE_PARAM& tDamageParams);
	virtual void PlayBC() override;


public:
	//�ൿ ���� �Լ� ����
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_bool IsPlayingSocket() const;

	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();
	void HitWeakProcess(_double TimeDelta);
	_bool CanMove4BC(_float fMinDist);
	_bool PriorityCondition();
private:
	//�浹 ���� �Լ� ����
	void FourAttack_Overlap(_bool Final = false);
	void Feint_SweepSphere();
	void Spin_SweepSphere();

private:
	void Select_Gift();
private:
	class CEM0900_Controller*		m_pController = nullptr;
	class CEM0900_AnimInstance*		m_pASM = nullptr;

	CMaterial* m_pArmor = nullptr;
	CMaterial* m_pWeak = nullptr;

	class CEMBrain* m_pBrain = nullptr;
	class CEMGift* m_pGift = nullptr;
private:
	_float3						 m_vMoveAxis;
	
	
	// Attack
	_bool		m_bAttack = false;
	_double	m_dLoopTick = 0.0;
	EDeBuffType m_ePreGiftType = EDeBuffType::DEBUFF_OIL;


	//Weak
	_int			m_iArmorHp = 0;
	_bool		m_bDestroyArmor = false;
	_bool		m_bArmorProcess = false;
	_bool		m_bWeakProcess = false;

	CController::EHandleInput	m_eInput;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;

	_float4x4	pivot;

	//Brain Crush
	_double		m_BCLoopTime = 0.0;
	_bool		m_CanFullBC = false;

public:
	static CEM0900* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

