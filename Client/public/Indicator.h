#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
class CRigidBody;
class CPhysXStaticModel;
END

BEGIN(Client)

class CIndicator : public CGameObject
{
private:
	CIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIndicator(const CIndicator& rhs);
	virtual ~CIndicator() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;

public:
	void SetPosition(_float3 vPos);


private:
	CRenderer*				m_pRendererCom = nullptr;
	CRigidBody*				m_pCollider = nullptr;
	

public:
	static CIndicator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END