#pragma once
#include "ScarletWeapon.h"

BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)
class CParticleGroup;

class CWeapon_wp0500 : public CScarletWeapon
{
private:
	CWeapon_wp0500(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_wp0500(const CWeapon_wp0500& rhs);
	virtual ~CWeapon_wp0500() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();

private:
	CParticleGroup* m_pParticle = nullptr;

private:
	_double		m_fTimeDelta = 0.f;
	_float4		m_vBeforePos;

private:
	HRESULT		SetUp_Components();

public:
	static CWeapon_wp0500* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END