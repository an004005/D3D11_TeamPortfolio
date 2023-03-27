#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CFSMComponent;
class CGameInstance;
class CAnimation;
class CRigidBody;
END

BEGIN(Client)

// �����
class CEM0800 : public CEnemy
{
private:
	CEM0800(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0800(const CEM0800& rhs);
	virtual ~CEM0800() = default;

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

public:
	//�ൿ ���� �Լ� ����
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_bool IsPlayingSocket() const;
	_bool	IsTurn() { return m_bTurn; }
	EBaseTurn GetBaseTurn() { return m_eTurn; }

	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();

	EBaseTurn FindTargetDirection();
private:
	//�浹 ���� �Լ� ����
	void Bite_Overlap();
	void Submergence_Overlap();
	void Laser_SweepSphere();

private:
	class CEM0800_Controller*		m_pController = nullptr;
	class CEM0800_AnimInstance*		m_pASM = nullptr;

	CRigidBody*					m_pRange = nullptr;
	CRigidBody*					m_pTrunk = nullptr;
	CRigidBody*					m_pWeak[2]{ nullptr };

	CEffectGroup*				m_pWaterPool = nullptr;

private:
	_float3						m_vMoveAxis;
	EBaseTurn					m_eTurn = EBaseTurn::TURN_END;
	_bool						m_bTurn = false;


	// Attack
	_bool m_bLaser = false;
	_float	m_fLaserTime = 0.f;
	_bool m_bComeUp = false;
	
	//WaterPool
	_bool		m_bHasWaterPool = false;


	CController::EHandleInput	m_eInput;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;
public:
	static CEM0800* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

