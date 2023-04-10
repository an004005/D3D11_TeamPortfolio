#include "stdafx.h"
#include "BulletBuilder.h"
#include "VFX_Manager.h"
#include "Bullet.h"
#include "GameInstance.h"
//#include "ScarletCharacter.h"

CBulletBuilder& CBulletBuilder::Set_Owner(CGameObject* pOwner)
{
	assert(pOwner != nullptr);
	m_pOwner = pOwner;

	return *this;
}

CBulletBuilder& CBulletBuilder::Set_Target(CScarletCharacter* pTarget)
{
	assert(pTarget != nullptr);
	m_pTarget = pTarget;

	return *this;
}

CBulletBuilder& CBulletBuilder::Set_InitBulletEffect(const vector<wstring>& EffectName)
{
	m_InitBulletEffects = EffectName;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_InitBulletParticle(const wstring& ParticleName, _bool IsRotParticles)
{
	m_InitParticle = ParticleName;
	m_bRotParticles = IsRotParticles;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_ShootSpeed(_float fSpeedsec)
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

CBulletBuilder& CBulletBuilder::Set_DeadBulletEffect(const vector<wstring>& EffectName)
{
	m_DeadBulletEffects = EffectName;
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

CBulletBuilder& CBulletBuilder::Set_TurnFixed(_float fAngle)
{
	m_fAngle = fAngle;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_BulletEffPivot(_float4x4 pivot)
{
	m_BulletEffPivot = pivot;
	return *this;
}

CBulletBuilder& CBulletBuilder::Set_Radius(_float fRad)
{
	m_fRadius = fRad;
	return *this;
}

void CBulletBuilder::Build()
{
	CBullet* pBullet = dynamic_cast<CBullet*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("Prototype_Bullet")));
	assert(pBullet != nullptr && "BulletBuilder CreatFail");
	pBullet->Create_InitEffects(m_InitBulletEffects, m_BulletEffPivot);

	if (m_bRotParticles == true)
		pBullet->Create_InitRotParticle(m_InitParticle, m_bRotParticles);
	else
		pBullet->Create_InitParticle(m_InitParticle);

	pBullet->Set_DeadEffects(m_DeadBulletEffects);
	pBullet->Set_DeadParticle(m_DeadParticle);

	pBullet->Set_Owner(m_pOwner);
	pBullet->Set_Target(m_pTarget);

	pBullet->Set_ShootSpeed(m_fBulletSpeed);
	pBullet->Set_LifeTime(m_fLife);
	pBullet->Set_DamageParam(m_eDamageParam);
	pBullet->Set_Radius(m_fRadius);

	pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_Position));
	pBullet->GetTransform()->LookAt(XMLoadFloat4(&m_TargetPos));
	pBullet->GetTransform()->Turn_Fixed(pBullet->GetTransform()->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_fAngle));

}

void CBulletBuilder::Free()
{
}
