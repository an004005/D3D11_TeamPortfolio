#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CBulletBuilder : public CBase
{
public:
	CBulletBuilder& CreateBullet() { return *this; }
	~CBulletBuilder() = default;

	CBulletBuilder& Set_Owner(CGameObject* pOwner);
	CBulletBuilder& Set_InitBulletEffect(const wstring& EffectName);
	CBulletBuilder& Set_InitBulletParticle(const wstring& ParticleName);

	CBulletBuilder& Set_ShootSpped(_float fSpeedsec);
	CBulletBuilder& Set_Life(_float fLife);
	CBulletBuilder& Set_DamageParam(const DAMAGE_PARAM	&	eDamageParam);

	CBulletBuilder& Set_DeadBulletEffect(const wstring& EffectName);
	CBulletBuilder& Set_DeadBulletParticle(const wstring& ParticleName);

	CBulletBuilder& Set_Position(_fvector vPosition);
	CBulletBuilder& Set_LookAt(_fvector vTargetPos);
	void Build();
private:
	vector<wstring>	m_InitBulletEffects;
	wstring m_InitParticle = L"";
	
	vector<wstring>	m_DeadBulletEffects;
	wstring m_DeadParticle = L"";

	CGameObject* m_pOwner = nullptr;
	_float			m_fBulletSpeed = 0.f;
	_float			m_fLife = 0.f;
	DAMAGE_PARAM		m_eDamageParam;

	_float4			m_Position;
	_float4			m_TargetPos;
public:
	virtual void Free() override;
};

END