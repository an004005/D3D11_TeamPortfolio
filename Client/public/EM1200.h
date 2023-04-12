#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

// 나오미 랜드

class CEM1200 : public CEnemy
{
private:
	CEM1200(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM1200(const CEM1200& rhs);
	virtual ~CEM1200() = default;

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

public:
	_bool	IsChangePhase() const { return m_bChangePhase; }
public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_bool IsRun() const { return m_bRun; }
	EBaseTurn GetBaseTurn() const { return m_eTurn; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_bool IsPlayingSocket() const;
	class CEM1200_AnimInstance* GetASM() { return m_pASM; }
	CModel* Get_Model() { return m_pModelCom; }

public:
	void Play_LightHitAnim();
	//void Play_MidHitAnim();
	void HeavyAttackPushStart();
	void HitWeakProcess(_double TimeDelta);
	void FogControl(_double TimeDelta);
public:
	//공격
	void Fall_Overlap();
	void Shout1_Overlap();
	void Shout2_Overlap();
	void Stamp_Overlap();
	void Swing_SweepSphere(const string& BoneName);
	void Rush_SweepSphere();

private:
	class CEM1200_Controller*		m_pController = nullptr;
	class CEM1200_AnimInstance*		m_pASM = nullptr;
	CMaterial* m_pWeak = nullptr;

private:
	_float3						m_vMoveAxis;
	EBaseTurn					m_eTurn = EBaseTurn::TURN_END;
	_bool						m_bRun = false;
	_bool						m_bChangePhase = false;

	//Attack
	_double						m_dLoopTime = 0.0;
	_double						m_dLoopTick = 0.0;
	_bool						m_bAttack = false;
	_uint						m_iAttackCount = 0;
	_int						m_iPreAttackCount = -1;
	_double				m_dFogTime = 0.0;

	_float4					m_SaveTargetPos;
	//Dodge
	_bool						m_bDodge = false;
	_float3						m_vOnJumpMoveVelocity;

	_bool						m_bWeakProcess = false;

	CController::EHandleInput	m_eInput;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;

	_float4x4		pivot;

	CDoOnce		m_bCableTalk;
	CDoOnce		m_bFogTalk;

	_bool	m_bWeakTalk = { false };

public:
	static CEM1200* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

