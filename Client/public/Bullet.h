#pragma once
#include <Timeline.h>

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CRigidBody;
END

BEGIN(Client)

class CBullet abstract : public CGameObject
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

protected:
	_bool CheckDamagedTarget(CGameObject* pTarget);

	void CollisionCheck_Bullet(CTransform* pTransform, DAMAGE_PARAM mParam, _float fRad = 1.f, _uint eColTypes = CTB_STATIC || CTB_PLAYER || CTB_MONSTER || CTB_MONSTER_PART || CTB_PSYCHICK_OBJ);

	virtual void CheckHitTarget(CGameObject* pTarget, CComponent* pHitBox, DAMAGE_PARAM mParam);

protected:
	void ClearDamagedTarget();
	
protected:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	
//	DAMAGE_PARAM			m_Damage_Params;
	_float					m_fShootSpeed = 0.f;

	_float4					m_BeforePos = _float4(0.f, 0.f, 0.f, -1.f);
	_bool					m_bHitCheck = false;

	set<CGameObject*> m_DamagedTargetList;

	CSimpleTimeline m_DeathTimeline; // 추후에 정립

//protected:
//	CScarletCharacter*		m_pCastOwner = nullptr;

public:
	virtual void Free() override;
};

END