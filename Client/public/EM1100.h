#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

// 물보스

class CEM1100 : public CEnemy
{
private:
	CEM1100(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM1100(const CEM1100& rhs);
	virtual ~CEM1100() = default;

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
	void	Set_RunStart(_bool bRunStart) { m_bRun_Start = bRunStart; }
	_bool	Get_RunStart() { return m_bRun_Start; }

public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_bool IsRun() const { return m_bRun; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_bool IsPlayingSocket() const;
	void Dodge_VelocityCalc();
	void AfterLocal180Turn();
	void Adjust_MoveAxis(_double TimeDelta);

	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();
	void Create_Bullet();
	void HitWeakProcess(_double TimeDelta);
	_bool CanMove4BC(_float fMinDist);

private:
	//충돌 관련 함수 정의
	void Rush_SweepSphere();
	void TailSwing_SweepSphere();
	void Stamp_Overlap();
	void WaterStart_Overlap();
	void WaterLoop_Overlap();

private:
	class CEM1100_Controller*		m_pController = nullptr;
	class CEM1100_AnimInstance*		m_pASM = nullptr;
	class CEMBrain*					m_pBrain = nullptr;
	CMaterial* m_pWeak = nullptr;
	class CEffectGroup* m_pRushEffect = nullptr;
private:
	_float3						 m_vMoveAxis;
	EBaseAxis				m_eMoveAxis = EBaseAxis::AXIS_END;
	_bool						m_bHitAir = false;

	_bool						m_bRun = false;
	//달리기 전 준비모션이 있어서 진짜 움직여야하는 타이밍
	_bool						m_bRun_Start = false;

	//Attack
	_bool						m_bRush = false;
	_bool						m_bTailSwing = false;
	_double					m_dLoopTime = 0.0;
	//Dodge
	_bool						m_bDodge = false;
	_float3						m_vOnJumpMoveVelocity;

	_bool						m_bWeakProcess = false;

	CController::EHandleInput	m_eInput;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;

	_float4x4	pivot;

	//Brain Crush
	_double		m_BCLoopTime = 0.0;
	_bool		m_CanFullBC = false;

	_bool	m_bWeakTalk = { false };

public:
	static CEM1100* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

