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

// 버디 루미

class CEM0400 : public CEnemy
{
private:
	CEM0400(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0400(const CEM0400& rhs);
	virtual ~CEM0400() = default;

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
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;
	_bool IsRun() const { return m_bRun; }

private:
	void Dodge_VelocityCalc();
	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void Swing_SweepCapsule(_bool bCol);
private:
	//충돌 관련 함수 정의

private:
	class CEM0400_Controller*		m_pController = nullptr;
	class CEM0400_AnimInstance*		m_pASM = nullptr;

	//충돌
	CRigidBody*					m_pWeaponCollider = nullptr;
	CRigidBody*					m_pRange = nullptr;

	CEffectGroup*				m_pSwingEffect = nullptr;

	// Swing Attack
	list<CScarletCharacter*> m_CollisionList;
private:
	//원시 데이터
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
	_bool m_bHitAir = false;

	_bool		m_bHitMove = false;

	// Attack
	_bool		m_bAtkSwitch = false;
	_bool		 m_bOneHit = false;

	// Run
	_bool m_bRun = false;

	//dodge
	_float4 m_vDodgeDir;
	_float3 m_vDodgeVelocity;
	_bool m_bDodge = false;

	string		m_strBoneName;

	CController::EHandleInput	m_eInput;
public:
	static CEM0400* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

