#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "HelperClasses.h"

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

	virtual _float4 GetPxPostion();

public:
	void CreateKineticParticle(_float4 vPos = { 0.f, 0.f, 0.f, 0.f }, _float4 vScale = {1.f, 1.f, 1.f ,0.f});
	void CreateKineticParticle_Mini(_float4 vPos = { 0.f, 0.f, 0.f, 0.f }, _float4 vScale = { 1.f, 1.f, 1.f ,0.f });

protected:
	_bool						Collision_Check_Capsule(CRigidBody* AttackTrigger, DAMAGE_PARAM DamageParam, _bool bCollisionCheck = false);
	list<CScarletCharacter*>	m_DamagedObjectList;

protected:
	_float4 m_BeforePos = _float4(0.f, 0.f, 0.f, 1.f);
	_float	m_fTimeDelta = 0.f;

protected:
	ESpecialType m_eSpecialObejctType = SPECAIL_END;

protected:
	void	OutlineMaker();
	_bool	m_bOutline = false;
	_bool	m_bBeforeOutline = false;

	CDoOnce	m_ParticleMakeable;
	_double	m_fParticleCoolTime = 0.f;

public:
	void	SpecialRimLightFix(_bool bFix) { m_bRimFix = bFix; }
	void	Set_Bright(_float fBright) 
	{ 
		m_fBright = fBright; 
		m_bBrightChange = true;
	}
	void	Set_Dissolve(_bool bDissolve) { m_bDissolve = bDissolve; }
	void	Set_Used() { m_bUseCheck = true; }
	_bool	Get_AddAble() { return m_bAddAble; }

protected:
	void	BrightChecker();
	void	DissolveChecker();
	_float	m_fBright = 0.f;
	_float	m_fDissolve = 0.f;
	_bool	m_bRimFix = false;	// 림라이트 값 고정
	_bool	m_bBrightChange = false;
	_bool	m_bDissolve = false;
	_bool	m_bUseCheck = false;
	_bool	m_bAddAble = false;

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

protected:
	vector<wstring>			m_vecRandomHitEffect{
		L"Special_G_Kinetic_Hit_Effect",
		L"Special_G_Kinetic_Hit_Effect_A",
		L"Special_G_Kinetic_Hit_Effect_B"
	};

protected:
	vector<wstring>			m_vecRandomParticle{
		L"Player_Kinetic_Particles_A",
		L"Player_Kinetic_Particles_B",
		L"Player_Kinetic_Particles_C",
		L"Player_Kinetic_Particles_D",
		L"Player_Kinetic_Particles_E",
		L"Player_Kinetic_Particles_F",
		L"Player_Kinetic_Particles_G"
	};

protected:
	vector<wstring>			m_vecRandomParticle_Small{
		L"Special_G_Iron_Bar_MeshData_Particle_A",
		L"Special_G_Iron_Bar_MeshData_Particle_B",
		L"Special_G_Iron_Bar_MeshData_Particle_C",
		L"Special_G_Iron_Bar_MeshData_Particle_D",
		L"Special_G_Iron_Bar_MeshData_Particle_E",
		L"Special_G_Iron_Bar_MeshData_Particle_F",
		L"Special_G_Iron_Bar_MeshData_Particle_G"
	};

public:
	CGameObject* Clone(void* pArg = nullptr)override { return nullptr; }
	void Free() override;
};

END