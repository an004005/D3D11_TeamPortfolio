#pragma once
#include <Timeline.h>

#include "Client_Defines.h"
#include "GameObject.h"
#include "ParticleGroup.h"
#include "EffectGroup.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CRigidBody;
END

BEGIN(Client)

class CBullet  : public CGameObject
{
protected:
	CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet(const CGameObject& rhs);
	virtual ~CBullet() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;

	virtual void Imgui_RenderProperty() override;

public:
	void Create_InitEffects(vector<wstring>& InitEffects, _float4x4 pivot = XMMatrixIdentity());
	void Create_InitParticle(wstring& InitParticle);
	void Create_InitRotParticle(wstring& InitParticle, _bool trueisUpdate);

	void Create_DeadEffects();
	void Create_DeadParticle();

	void Set_DeadEffects(vector<wstring>& DeadEffects) {
		m_pDeadEffects = DeadEffects;
	}

	void Set_DeadParticle(wstring& DeadParticle) {
		m_pDeadParticle = DeadParticle;
	}
	void Set_ShootSpeed(_float fShootSpped) {
		m_pTransformCom->SetSpeed(fShootSpped);
	}
	void Set_LifeTime(_float fLifeTime) {
		m_fLife = fLifeTime;
	}
	void Set_DamageParam(DAMAGE_PARAM& eDamageParam) {
		m_eDamageParam = eDamageParam;
	}

	void Set_Target(class CScarletCharacter* pTarget) {
		m_pTarget = pTarget;
	}

	void Set_Radius(_float fRad) {
		m_fRadius = fRad;
	}
protected:
	_bool CheckDamagedTarget(CGameObject* pTarget);

	void CollisionCheck_Bullet(CTransform* pTransform, DAMAGE_PARAM mParam, _float fRad = 1.f, _uint eColTypes = CTB_STATIC || CTB_PLAYER || CTB_MONSTER || CTB_MONSTER_PART || CTB_PSYCHICK_OBJ);

	virtual void CheckHitTarget(CGameObject* pTarget, CComponent* pHitBox, DAMAGE_PARAM mParam);

protected:
	void ClearDamagedTarget();
	
protected:
	CRenderer*				m_pRendererCom = nullptr;
	class CScarletCharacter* m_pTarget = nullptr;
//	DAMAGE_PARAM			m_Damage_Params;
	//_float					m_fShootSpeed = 0.f;

	_float4					m_BeforePos = _float4(0.f, 0.f, 0.f, -1.f);
	_bool					m_bHitCheck = false;

	set<CGameObject*> m_DamagedTargetList;

	CSimpleTimeline m_DeathTimeline; // 추후에 정립

	//Builder
	vector<CEffectGroup*> m_pInitEffects;
	CParticleGroup* m_pInitParticle = nullptr;

	 vector<wstring> m_pDeadEffects;
	 wstring m_pDeadParticle = L"";

	_float m_fLife = 0.f;
	DAMAGE_PARAM m_eDamageParam;
	_float	m_fRadius = 0.3f;


	_bool	m_bRotParticles = false;
public:
	static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END