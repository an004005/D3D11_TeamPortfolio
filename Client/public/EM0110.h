#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CFSMComponent;
class CGameInstance;
class CAnimation;
class CRigidBody;
class CCurveMananger;
END

BEGIN(Client)

// flower_Hands :: ���� ����

class CEM0110 : public CEnemy
{
private:
	CEM0110(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0110(const CEM0110& rhs);
	virtual ~CEM0110() = default;

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
	void AfterLocal180Turn();
	void Define_TurningKickAnim();
	void Adjust_MoveAxis(_double TimeDelta);

private:
	void Play_HeavbyHitAnim();
	void SweepCapsule(CRigidBody* pCol);

private:
	//�浹 ���� �Լ� ����
	void Rush_Overlap();
	void Rush_SweepCapsule();
	void Kick_SweepSphere();

private:
	class CEM0110_Controller*		m_pController = nullptr;
	class CEM0110_AnimInstance*		m_pASM = nullptr;

	//�浹
	CRigidBody*					m_pRange = nullptr;
	CRigidBody*					m_pBody = nullptr;
	CRigidBody*					m_pHindLegs = nullptr;

	CEffectGroup*				m_pSwingEffect = nullptr;

	// Swing Attack
	list<CScarletCharacter*> m_CollisionList;
private:
	//���� ������
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	//rush
	_float3 m_vRushDirection;
	_bool	m_bRush = false;

	// Attack
	_bool		m_bAttack = false;
	
	// Run
	_bool m_bRun = false;

	CController::EHandleInput	m_eInput;
public:
	static CEM0110* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

