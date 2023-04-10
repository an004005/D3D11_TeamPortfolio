#pragma once
#include "ScarletWeapon.h"

BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)

enum WEAPONTYPE { WP_0106, WP_0126, WP_0190, WP_END, };

class CWeapon_Player :
    public CScarletWeapon
{
private:
    CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWeapon_Player(const CWeapon_Player& rhs);
    virtual ~CWeapon_Player() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();

public:
	void	Setup_BoneMatrix(CModel* pModel, _fmatrix Transform, const string& strBoneName);
	void	Change_Weapon(WEAPONTYPE eType);
	_uint	Get_WeaponType() { return static_cast<_uint>(m_eType); }

private:
	WEAPONTYPE	m_eType = WP_END;

private:
	_float4		m_vBeforePos;

private:
	HRESULT				SetUp_Components();
	vector<CModel*>		m_pModelComs;
	vector<wstring>		m_pModelTags;

public:
	static CWeapon_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END