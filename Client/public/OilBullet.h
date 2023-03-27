#pragma once
#include "Client_Defines.h"
#include "Bullet.h"
#include "ParticleGroup.h"
#include "EffectGroup.h"

BEGIN(Engine)
class CGameInstance;
class CRigidBody;
class CFSMComponent;
END

BEGIN(Client)

class COilBullet : public CBullet
{
private:
	COilBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COilBullet(const COilBullet& rhs);
	virtual ~COilBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;

	void SetDamage(_int iDamage) { m_iDamage = iDamage; }

private:
	CParticleGroup*	m_pTrailParticle = nullptr;
	CEffectGroup*	m_pBulletEffect = nullptr;
	_int m_iDamage = 0;
	_float m_fLife = 5.f;

public:
	static COilBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END