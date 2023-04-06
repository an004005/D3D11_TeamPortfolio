#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Client)

// 초(고속)파리 :: 방도 팡뒤(Bangdo Fandu)

class CEM0700 : public CEnemy
{
private:
	CEM0700(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0700(const CEM0700& rhs);
	virtual ~CEM0700() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void SetUpComponents(void* pArg) override;
	virtual void SetUpSound() override;
	virtual void SetUpAnimationEvent() override;
	virtual void SetUpFSM() override;
	virtual void SetUpUI() override;

	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual void PlayBC() override;

public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_bool IsPlayingSocket() const;

private:
	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();
	_bool CanMove4BC(_float fMinDist);

	void SelectRandomMoveAnim();
	void Create_Bullet();
	void Check_Height();
	void Move_YAxis(_double);
	void	AfterLocal180Turn();
private:
	//충돌 관련 함수 정의
	void SelectEscapeAnim_Overlap();
	void Rush_StaticCheckSweep();
	void Rush_SweepSphere();
private:
	class CEM0700_Controller*		m_pController = nullptr;
	class CEM0700_AnimInstance*		m_pASM = nullptr;
	class CSuperSpeedTrail*			m_pTrail = nullptr;
	class CEMBrain*					m_pBrain = nullptr;

	class CEffectGroup* m_pRushEffect = nullptr;
private:
	_float3						 m_vMoveAxis;
	_bool						m_bHitAir = false;

	_float						m_fHeight = 0.f;
	_float						m_fMaxHeight = 1.f;

	//Rush
	_float						m_fRushTime = 1.f;

	CController::EHandleInput	m_eInput;

	_double		m_BCLoopTime = 0.0;
	_bool		m_CanFullBC = false;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;

public:
	static CEM0700* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

