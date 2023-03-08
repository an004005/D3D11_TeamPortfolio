#pragma once
#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)

class CTrigger final :  public CGameObject
{
private:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	void AfterPhysX() override;
	HRESULT Render() override;
	void Imgui_RenderProperty() override;

public:
	void	SetMonster(const _tchar* ProtoTag, _fmatrix WorldMatrix);

private:
	HRESULT SetUp_Components(void * pArg);

private:
	CRigidBody*			m_pRigidBodyCom;


public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;

};

END