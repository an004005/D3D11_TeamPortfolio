#pragma once
#include "ScarletWeapon.h"

BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)
class CParticleGroup;

class CWeapon_wp0190 : public CScarletWeapon
{
private:
	CWeapon_wp0190(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_wp0190(const CWeapon_wp0190& rhs);
	virtual ~CWeapon_wp0190() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();

public:
	void		SetFire();
	void		ReleaseFire();

private:
	CParticleGroup*	m_pParticle = nullptr;

private:
	_float4		m_vBeforePos;

private:
	HRESULT		SetUp_Components();

public:
	static CWeapon_wp0190*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END