#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRigidBody;
class CModel;
class CRenderer;
class CShader;
END

BEGIN(Client)

enum KineticModeltag
{
	Tag_default,
	Tag_broken,
	Tag_End
};

class CSpecialObject :
	public CGameObject
{
protected:
	CSpecialObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecialObject(const CSpecialObject& rhs);
	virtual ~CSpecialObject() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX();
	HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth() override;

	virtual void LoadFromJson(const Json& json) override {}
	virtual void SaveToJson(Json& json) override {}

	virtual void Imgui_RenderProperty() override;

protected:
	void	OutlineMaker();
	_bool	m_bOutline = false;
	_bool	m_bBeforeOutline = false;

protected:
	HRESULT	SetUp_Components(void* pArg);

	CRenderer*				m_pRendererCom = nullptr;
	vector<CModel*>			m_pModelComs;
	vector<wstring>			m_pModelTags;
	CRigidBody*				m_pCollider = nullptr;

protected:
	_float4x4				m_LocalMatrix;

protected:
	KineticModeltag			m_eCurModelTag;

public:
	CGameObject* Clone(void* pArg = nullptr)override { return nullptr; }
	void Free() override;
};

END