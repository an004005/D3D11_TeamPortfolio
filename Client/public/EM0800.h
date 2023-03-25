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

// ��(���)�ĸ� :: �浵 �ε�(Bangdo Fandu)

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
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;

	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();

private:
	//�浹 ���� �Լ� ����

private:
	class CEM0800_Controller*		m_pController = nullptr;
	class CEM0800_AnimInstance*		m_pASM = nullptr;

	//�浹
	CRigidBody*				m_pRange = nullptr;
	CRigidBody*				m_pBody = nullptr;

private:
	_float3						m_vMoveAxis;
	_float						m_fTurnRemain = 0.f;
	_bool						m_bHitAir = false;
	
	// HitDir
	_bool		m_bHitMove = false;

	// Attack
	_bool m_bAtkBite = false;
	_bool m_bAtkLaser = false;
	_float	m_fLaserTime = 0.f;

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

