#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CControlledRigidBody;
END

BEGIN(Client)
class CTestObject : public CGameObject
{
private:
	CTestObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestObject(const CTestObject& rhs);
	virtual ~CTestObject() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;
	virtual HRESULT Render() override;


private:
	CRenderer*				m_pRendererCom = nullptr;
	CControlledRigidBody* m_pBody = nullptr;

	_float4 m_vDown;

public:
	static CTestObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END