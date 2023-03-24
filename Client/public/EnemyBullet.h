#pragma once
#include "Client_Defines.h"
#include "Bullet.h"
#include "ParticleGroup.h"
#include "EffectGroup.h"
BEGIN(Engine)
class CGameInstance;
class CModel;
class CRenderer;
class CFSMComponent;
class CRigidBody;
END

BEGIN(Client)

class CEnemyBullet : public CBullet
{
private:
	CEnemyBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyBullet(const CEnemyBullet& rhs);
	virtual ~CEnemyBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	virtual void AfterPhysX();

private:
	CFSMComponent*			m_pFSM = nullptr;
	// Bullet Loop
	CParticleGroup*	m_pTrailParticle = nullptr;
	CEffectGroup*	m_pBulletEffect = nullptr;

private:
	_vector					m_vPrePos;
	_bool					m_bPrePos = false;

public:
	static CEnemyBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END