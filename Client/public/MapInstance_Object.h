#pragma once
#include "MapObject.h"

BEGIN(Engine)
class CPhysXStaticModel;
END

BEGIN(Client)

class CMapInstance_Object : public CMapObject
{
private:
	CMapInstance_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapInstance_Object(const CMapInstance_Object& rhs);
	virtual ~CMapInstance_Object() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

public:
	CModel_Instancing* Get_Model_Instancing() { return m_pModel_InstancingCom; }

public:
	void		Set_Focus();
private:
	wstring MakePxModelProtoTag();

private:
	HRESULT	SetUp_Components();

	CPhysXStaticModel*		m_pPxModel = nullptr;
	CModel_Instancing*		m_pModel_InstancingCom = nullptr;

	_int					m_iIndex = -1;
	_float4x4				m_WorldMatrix;
public:
	static CMapInstance_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END