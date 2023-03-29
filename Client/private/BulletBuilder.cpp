#include "stdafx.h"
#include "BulletBuilder.h"
#include "VFX_Manager.h"
#include "Bullet.h"
#include "GameInstance.h"

CBulletBuilder& CBulletBuilder::Set_Owner(CGameObject* pOwner)
{
	assert(pOwner != nullptr);
	m_pOwner = pOwner;

	return *this;
}

CBulletBuilder& CBulletBuilder::Set_InitBulletEffect(const wstring& EffectName)
{
	m_InitBulletEffects.emplace_back(EffectName);
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_InitBulletParticle(const wstring& ParticleName)
{
	m_InitParticle = ParticleName;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_ShootSpped(_float fSpeedsec)
{
	m_fBulletSpeed = fSpeedsec;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_Life(_float fLife)
{
	m_fLife = fLife;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_DamageParam(const DAMAGE_PARAM& eDamageParam)
{
	m_eDamageParam = eDamageParam;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_DeadBulletEffect(const wstring& EffectName)
{
	m_DeadBulletEffects.emplace_back(EffectName);
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_DeadBulletParticle(const wstring& ParticleName)
{
	m_DeadParticle = ParticleName;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_Position(_fvector vPosition)
{
	XMStoreFloat4(&m_Position, vPosition);
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_LookAt(_fvector vTargetPos)
{
	XMStoreFloat4(&m_TargetPos, vTargetPos);
	return *this;
}

void CBulletBuilder::Build()
{
	CBullet* pBullet = dynamic_cast<CBullet*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("Prototype_Bullet")));
	assert(pBullet != nullptr && "BulletBuilder CreatFail");
	pBullet->Create_InitEffects(m_InitBulletEffects);
	pBullet->Create_InitParticle(m_InitParticle);
	pBullet->Set_DeadEffects(m_DeadBulletEffects);
	pBullet->Set_DeadParticle(m_DeadParticle);
	pBullet->Set_Owner(m_pOwner);
	pBullet->Set_ShootSpeed(m_fBulletSpeed);
	pBullet->Set_LifeTime(m_fLife);
	pBullet->Set_DamageParam(m_eDamageParam);
	pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_Position));
	pBullet->GetTransform()->LookAt(XMLoadFloat4(&m_TargetPos));
}

void CBulletBuilder::Free()
{
}
