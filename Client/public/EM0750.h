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

// 파리 :: 스커미 팡뒤(SkummyPandou)

class CEM0750 : public CEnemy
{
private:
	CEM0750(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0750(const CEM0750& rhs);
	virtual ~CEM0750() = default;

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
	_bool IsPlayingSocket() const;

	void Play_LightHitAnim();
	void Play_MidHitAnim();
	void HeavyAttackPushStart();

private:
	//충돌 관련 함수 정의
	void Rush_StaticCheckSweep();
	void Rush_SweepSphere();
	void Check_Height();
	void Move_YAxis(_double TimeDelta);
private:
	class CEM0750_Controller*		m_pController = nullptr;
	class CEM0750_AnimInstance*		m_pASM = nullptr;

private:
	_float3						 m_vMoveAxis;
	_bool						m_bHitAir = false;

	_float						m_fHeight = 0.f;
	_float						m_fMaxHeight = 1.3f;

	//Rush
	_bool						m_bRush = false;
	_float						m_fRushTime = 1.f;
	_bool						m_bChangeDir = false;
	_float4						m_vStartDir;
	_float4						m_vEndDir;


	CController::EHandleInput	m_eInput;

	//Heavy coll
	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;
public:
	static CEM0750* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

