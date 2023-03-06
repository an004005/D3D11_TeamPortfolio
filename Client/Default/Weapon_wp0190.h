#pragma once
#include "ScarletWeapon.h"

BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)

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
	void	Trail_Setting(_bool TrailOn) { m_bTrailOn = TrailOn; }

private:
	_vector		m_vBeforePos;
	_bool		m_bTrailOn = false;

private:
	HRESULT		SetUp_Components();

public:
	static CWeapon_wp0190*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END