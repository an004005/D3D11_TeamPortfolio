#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

// 물보스

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
	virtual void PlayBC() override;

public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_bool IsPlayingSocket() const;

	void Play_LightHitAnim();
	void HeavyAttackPushStart();
	void Create_Bullet();
	void HitWeakProcess(_double TimeDelta);
	_bool CanMove4BC(_float fMinDist);

private:
	//충돌 관련 함수 정의
	void Rush_SweepSphere();
	void TailSwing_SweepSphere();
	void Stamp_Overlap();

private:
	class CEM0900_Controller*		m_pController = nullptr;
	class CEM0900_AnimInstance*		m_pASM = nullptr;

	CMaterial* m_pArmor = nullptr;
	CMaterial* m_pWeak = nullptr;

	class CEMBrain* m_pBrain = nullptr;
private:
	_float3						 m_vMoveAxis;
	
	
	// Attack
	_bool		m_bAttack = false;
	_double	m_dLoopTick = 0.0;


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

