#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Client)

// ��Ŀ�� Ǯ

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

private:
	void Play_LightHitAnim();
	void Play_MidHitAnim();

	void Create_Bullet();
	void Make_Decal(_double TimeDelta);
	_bool PriorityCondition();

private:
	//�浹 ���� �Լ� ����
	void	HeavyAttackPushStart();

private:
	class CEM0650_Controller*		m_pController = nullptr;
	class CEM0650_AnimInstance*		m_pASM = nullptr;

private:
	//���� ������
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
	_bool	m_bHitAir = false;

	_vector	m_LastSpotTargetPos;
	_bool		m_bHitMove = false;

	//��Į
	_double		m_dDecalTime = 0.0;

	_bool	m_bDown = false;

	CController::EHandleInput	m_eInput = CController::EHandleInput::HANDLE_END;

	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;
public:
	static CEM0650* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

