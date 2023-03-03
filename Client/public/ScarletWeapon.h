#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Engine)
class CBone;
class CRigidBody;
END

BEGIN(Client)
class CTrailSystem;

typedef struct tagWeaponDesc
{
	_float4x4	m_PivotMatrix;
	CBone*		m_pSocket;
	CTransform*	m_pTransform;
	void*		m_pJson;
}WEAPON_DESC;

class CScarletWeapon : public CGameObject
{
protected:
	CScarletWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScarletWeapon(const CScarletWeapon& rhs);
	virtual ~CScarletWeapon() = default;

public:
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Late_Tick(_double TimeDelta) override;

public:
	void	Setup_BoneMatrix(CModel* pModel, _fmatrix Transform);

public:
	CRigidBody*	Get_Trigger() { return m_pCollider; }

protected:
	void	Collision_Check();
	
protected:
	WEAPON_DESC	m_Desc;
	Vector4		m_BeforePos;
	Matrix		m_BeforeMatrix;

protected:
	CRenderer*		m_pRenderer = nullptr;
	CModel*			m_pModel = nullptr;
	CRigidBody*		m_pCollider = nullptr;
	CTrailSystem*	m_pTrail = nullptr;

protected:
	CGameObject*	m_pCamSpot = nullptr;

protected:
	wstring		m_ModelName;

public:
	virtual void Free() override;
};

END