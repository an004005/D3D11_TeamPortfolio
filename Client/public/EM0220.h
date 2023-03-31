#pragma once
#include "Enemy.h"
#include "Controller.h"


BEGIN(Client)

// 다리 숨김 :: 베이스 포즈

class CEM0220 : public CEnemy
{
private:
	CEM0220(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0220(const CEM0220& rhs);
	virtual ~CEM0220() = default;

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

	virtual void CheckHP(DAMAGE_PARAM& tDamageParams);
	void	SetUp_Lantern();
public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;


private:
	void Play_HeavbyHitAnim();
	void Update_LanternMatrix();
	void CounterAttack(_double TimeDelta);
	void Create_Bullet();

private:
	//충돌 관련 함수 정의

private:
	class CEM0220_Controller*		m_pController = nullptr;
	class CEM0220_AnimInstance*		m_pASM = nullptr;

	//충돌
	CEffectGroup*				m_pSwingEffect = nullptr;

	vector<class CEM0221*>	m_pLanterns;
private:
	//원시 데이터
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	_bool		m_bHitMove = false;

	// Attack
	_bool		m_bAttack = false;
	_double		m_dFallCount = 0.0;
	_uint		m_iFallIndex = 0;


	//무적상태
	_bool		m_Unbeatable = false;

	//플레이어  순간이동 상태 확인
	//_bool		m_bTeleport = false;

	CController::EHandleInput	m_eInput;
	
public:
	static CEM0220* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

