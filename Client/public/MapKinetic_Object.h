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
	void	Set_Kinetic(_bool bKinetic);

private:
	wstring MakePxModelProtoTag();

private:
	HRESULT	SetUp_Components(void* pArg);

	CPhysXDynamicModel*	m_pPxModel = nullptr;
	CRigidBody*			m_pKinetic_RigidBody = nullptr;
	CRigidBody*			m_pDynamic_RigidBody = nullptr;

	_bool				m_bKinetic = false;
	
	CModel*				m_pModelCom = nullptr;

public:
	void				SetThrow(_bool bThrow) { m_bThrow = bThrow; }
	_bool				Usable() { return (m_bThrow == false); }

private:
	_bool				m_bThrow = false;		//	데미지를 줄 수 있는 상태인가?

public:
	static CMapKinetic_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END

