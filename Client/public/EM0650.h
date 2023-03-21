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

// ��(���)�ĸ� :: �浵 �ε�(Bangdo Fandu)

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
	_bool IsPlayingSocket() const;
	void Attack_a1_Loop(_double TimeDelta);

private:
	//�浹 ���� �Լ� ����

private:
	class CEM0650_Controller*		m_pController = nullptr;
	class CEM0650_AnimInstance*		m_pASM = nullptr;

	//�浹
	CRigidBody*					m_pTrigger = nullptr;
	CRigidBody*					m_pRange = nullptr;



	//��ƼŬ
	CParticleGroup*			m_pFallRoseParticle = nullptr;
	CParticleGroup*			m_pShootFlwParticle = nullptr;

	//��ƿ
	CDoOnce m_Laugh;

private:
	//���� ������
	_float						m_fDistance = 0.f;
	_float3						m_vDirection;
	CController::EHandleInput	m_eInput;
public:
	static CEM0650* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

