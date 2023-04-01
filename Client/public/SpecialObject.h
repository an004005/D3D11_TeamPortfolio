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
class CScarletCharacter;

enum ESpecialType { 
	SPECIAL_TRAIN, 
	SPECIAL_TELEPHONEPOLE, 
	SPECIAL_HBEAM_BUNDLE, 
	SPECIAL_HBEAM_SINGLE,
	SPECIAL_DROPOBJECT_BUNDLE,
	SPECIAL_DROPOBJECT_SINGLE,
	SPECIAL_TANKLORRY,
	SPECIAL_TANKLORRY_HEAD,
	SPECIAL_TANKLORRY_TRAILER,
	SPECIAL_TANKLORRY_TANK,
	SPECIAL_IRONBARS,
	SPECAIL_IRONBARS_DOOR,
	SPECIAL_IRONBARS_BARS,
	SPECIAL_IRONBARS_SINGLEBARS,
	SPECIAL_IRONBARS_MULTIBARS,
	SPECAIL_CONTAINER,
	SPECAIL_END };

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
	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;

	virtual void Imgui_RenderProperty() override;

public:
	ESpecialType Get_SpecialType() const { return m_eSpecialObejctType; }

protected:
	_bool						Collision_Check_Capsule(CRigidBody* AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollisionCheck = false);
	list<CScarletCharacter*>	m_DamagedObjectList;

protected:
	_float4 m_BeforePos = _float4(0.f, 0.f, 0.f, 1.f);

protected:
	ESpecialType m_eSpecialObejctType = SPECAIL_END;

protected:
	void	OutlineMaker();
	_bool	m_bOutline = false;
	_bool	m_bBeforeOutline = false;

public:
	void	SetOutline(_bool bOutLine) { m_bOutline = bOutLine; }

protected:
	HRESULT	SetUp_Components(void* pArg);

	CRenderer*				m_pRendererCom = nullptr;
	vector<CModel*>			m_pModelComs;
	vector<wstring>			m_pModelTags;
	CRigidBody*				m_pCollider = nullptr;

protected:
	_float4x4				m_LocalMatrix;

protected:
	wstring					m_strModelTag;

public:
	CGameObject* Clone(void* pArg = nullptr)override { return nullptr; }
	void Free() override;
};

END