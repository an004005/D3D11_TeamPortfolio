#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRigidBody;
class CPhysXStaticModel;
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CHS_TeleportableWall : public CGameObject
{
private:
	CHS_TeleportableWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHS_TeleportableWall(const CHS_TeleportableWall& rhs);
	virtual ~CHS_TeleportableWall() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

private:
	HRESULT	SetUp_Components();

private:
	CRigidBody*			m_pRigidBody = nullptr;	
	CModel*				m_pModelCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

private:
	_float4x4				m_LocalMatrix;

public:
	static CHS_TeleportableWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END