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

// flower_Hands :: ���� ����

class CEM0210 : public CEnemy
{
private:
	CEM0210(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0210(const CEM0210& rhs);
	virtual ~CEM0210() = default;

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
	_bool IsRun() const { return m_bRun; }

private:
	void Dodge_VelocityCalc();
	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void SweepCapsule(CRigidBody* pCol);

private:
	//�浹 ���� �Լ� ����

private:
	class CEM0210_Controller*		m_pController = nullptr;
	class CEM0210_AnimInstance*		m_pASM = nullptr;

	//�浹
	CRigidBody*					m_pTailCol = nullptr;
	CRigidBody*					m_pWeaponCol = nullptr;
	CRigidBody*					m_pRange = nullptr;

	CEffectGroup*				m_pSwingEffect = nullptr;

	// Swing Attack
	list<CScarletCharacter*> m_CollisionList;
private:
	//���� ������
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
	_bool m_bHitAir = false;

	_bool		m_bHitMove = false;

	// Attack
	_bool		m_bAttack = false;
	
	// Run
	_bool m_bRun = false;

	//dodge
	_float4 m_vDodgeDir;
	_float3 m_vDodgeVelocity;
	_bool m_bDodge = false;

	//Jump
	_float3 m_vOnJumpMoveVelocity;

	CController::EHandleInput	m_eInput;
public:
	static CEM0210* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

