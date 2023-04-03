#pragma once
#include "Enemy.h"
#include "Controller.h"

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

	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();
	void Create_Bullet();
private:
	//충돌 관련 함수 정의
	void Rush_SweepSphere();
	void TailSwing_SweepSphere();
	void Stamp_Overlap();

private:
	class CEM1100_Controller*		m_pController = nullptr;
	class CEM1100_AnimInstance*		m_pASM = nullptr;
	
	class CEffectGroup* m_pRushEffect = nullptr;
private:
	_float3						 m_vMoveAxis;
	_bool						m_bHitAir = false;

	_bool						m_bRun = false;
	//달리기 전 준비모션이 있어서 진짜 움직여야하는 타이밍
	_bool						m_bRun_Start = false;

	//Attack
	_bool						m_bRush = false;
	_bool						m_bTailSwing = false;
	_float4						vTailPos;
	//Dodge
	_bool						m_bDodge = false;
	_float3						m_vOnJumpMoveVelocity;

	
	CController::EHandleInput	m_eInput;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;

	_float4x4	pivot;
public:
	static CEM1100* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

