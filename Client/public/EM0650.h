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

// 스커미 풀

class CEM0650 : public CEnemy
{
private:
	CEM0650(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0650(const CEM0650& rhs);
	virtual ~CEM0650() = default;

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

public:
	//행동 관련 함수 정의
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;

private:
	void Play_LightHitAnim();
	void Play_MidHitAnim();

	void Create_Bullet();
private:
	//충돌 관련 함수 정의

private:
	class CEM0650_Controller*		m_pController = nullptr;
	class CEM0650_AnimInstance*		m_pASM = nullptr;

	//충돌
	CRigidBody*					m_pRange = nullptr;

private:
	//원시 데이터
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
	_bool m_bHitAir = false;

	_vector	m_LastSpotTargetPos;
	_bool		m_bHitMove = false;

	CController::EHandleInput	m_eInput;
public:
	static CEM0650* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

