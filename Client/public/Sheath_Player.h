#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Weapon_Player.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CSheath_Player : public CGameObject
{
private:
	CSheath_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSheath_Player(const CSheath_Player& rhs);
	virtual ~CSheath_Player() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();

public:
	void	Setup_BoneMatrix(CModel* pModel, _fmatrix Transform);
	CModel* GetModel() { return m_pModel; }

public:
	void	Change_Sheath(WEAPONTYPE eType);

private:
	WEAPONTYPE	m_eType = WP_END;

private:
	CRenderer* m_pRenderer = nullptr;
	CModel* m_pModel = nullptr;

private:
	HRESULT				SetUp_Components();
	vector<CModel*>		m_pModelComs;
	vector<wstring>		m_pModelTags;

private:
	wstring		m_ModelName;

public:
	static CSheath_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END