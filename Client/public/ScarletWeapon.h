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
	//CBone*		m_pSocket;
	//CTransform*	m_pTransform;
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
	_matrix	Get_WeaponCenterMatrix();
	void	Setup_BoneMatrix(CModel* pModel, _fmatrix Transform);

public:
	void	Trail_Setting(_bool TrailOn) { m_bTrailOn = TrailOn; }
	void	Trail_Tick(_double TimeDelta);

protected:
	_bool	m_bTrailOn = false;

public:
	CRigidBody*	Get_Trigger() { return m_pCollider; }
	_float4		Get_Edge();
	
protected:
	WEAPON_DESC	m_Desc;
	Vector4		m_BeforePos;
	Matrix		m_BeforeMatrix;
	_float4		m_vSlashVector = { 0.f, 0.f, 0.f ,1.f };
	_float		m_fAdaptLength = 0.f;

public:
	void		Set_Bright(ESASType eType, _bool bBright);
	_float4		Get_SlashVector() { return m_vSlashVector; }

protected:
	ESASType	m_eSasType = ESASType::SAS_END;
	_bool		m_bBright = false;

protected:
	CRenderer*		m_pRenderer = nullptr;
	CModel*			m_pModel = nullptr;
	CRigidBody*		m_pCollider = nullptr;
	CTrailSystem*	m_pTrail = nullptr;

//protected:
//	CGameObject*	m_pCamSpot = nullptr;

protected:
	wstring		m_ModelName;

public:
	virtual void Free() override;
};

END