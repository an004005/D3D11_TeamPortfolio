#pragma once
#include "MapObject.h"

BEGIN(Engine)
class CRigidBody;
class CModel;
END

BEGIN(Client)
class CParticleGroup;

enum EKineticType
{
	KT_NORMAL,
	KT_END
};

enum KineticModeltag
{
	Tag_default,
	Tag_broken,
	Tag_End
};

class CMapKinetic_Object : public CMapObject
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
	virtual HRESULT Render_ShadowDepth() override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

	virtual const wstring&	Get_ModelTag() override;

public:
	void	Add_Physical(_float3 vForce = { 0.f, 0.f, 0.f }, _float3 vTorque = { 0.f, 0.f, 0.f });
	void	Set_Kinetic(_bool bKinetic);
	void	Reset_Transform();
	EKineticType GetType() const { return m_eType; }

public:
	void	SetOutline(_bool bOutLine) { m_bOutline = bOutLine; }

private:
	void	OutlineMaker();
	_bool	m_bOutline = false;
	_bool	m_bBeforeOutline = false;

public:
	void	SetParticle();
	void	ReleaseParticle();

private:
	CParticleGroup*	m_pParticle = nullptr;

private:
	HRESULT	SetUp_Components(void* pArg);

	vector<CModel*>			m_pModelComs;
	vector<wstring>			m_pModelTags;
	CRigidBody*				m_pCollider = nullptr;
	EKineticType			m_eType = KT_NORMAL;
	_float4x4				m_LocalMatrix;

	KineticModeltag			m_eCurModelTag = Tag_End;
public:
	void				SetThrow() { m_bThrow = true; }
	_bool				GetThrow() { return m_bThrow; }
	_bool				Usable() { return m_bUsable; }

	_bool				Get_CameRange() {
		return m_bCameRange;
	}
	void				Set_CameRange(const _bool bCameRange) {
		m_bCameRange = bCameRange;
	}

private:
	_bool				m_bThrow = false;
	_bool				m_bUsable = true;
	_bool				m_bHit = false;
	_float				m_fDeadTimer = 0.f;

	_bool				m_bCameRange = { false };

public:
	static CMapKinetic_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END

