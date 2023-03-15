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

class CSkMpBullet : public CBullet
{
private:
	CSkMpBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkMpBullet(const CSkMpBullet& rhs);
	virtual ~CSkMpBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	virtual void AfterPhysX();

	/*void Set_TrailParticle(CParticleGroup* pTrail);
	CParticleGroup* Get_TrailParticle();

	void Set_BulletEffect(CEffectGroup* pBulletEffect);
	CEffectGroup* Get_BulletEffect();*/

public:
	void Set_ShootDir(_vector vDest) { m_vDir = XMVector3Normalize(vDest); }

private:
	CFSMComponent*			m_pFSM = nullptr;
	// Bullet Loop
	CParticleGroup*	m_pTrailParticle = nullptr;
	CEffectGroup*	m_pBulletEffect = nullptr;

private:
	_vector					m_vDir = { 0.f, 0.f, 0.f, 0.f };
	_vector					m_vPrePos;
	_bool					m_bPrePos = false;

public:
	static CSkMpBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END