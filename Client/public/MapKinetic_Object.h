#pragma once
#include "MapObject.h"

BEGIN(Engine)
class CPhysXDynamicModel;
class CRigidBody;
END

BEGIN(Client)

class CMapKinetic_Object :
	public CMapObject
{
private:
	CMapKinetic_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapKinetic_Object(const CMapKinetic_Object& rhs);
	virtual ~CMapKinetic_Object() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX();
	HRESULT Render() override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

public:
	void	Add_Physical(_float3 vForce = { 0.f, 0.f, 0.f }, _float3 vTorque = {0.f, 0.f, 0.f});
	void	Reset_Transform();

private:
	wstring MakePxModelProtoTag();

private:
	HRESULT	SetUp_Components(void* pArg);

	CModel*				m_pModelCom = nullptr;
	CRigidBody*			m_pCollider = nullptr;

public:
	_bool				Usable() { return m_bUsable; }

private:
	_bool				m_bUsable = true;

public:
	static CMapKinetic_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END

