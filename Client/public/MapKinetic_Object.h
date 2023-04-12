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
	void	Set_Trigger(_bool bTrigger);
	void	Set_Dynamic();
	void	Reset_Transform();
	EKineticType GetType() const { return m_eType; }

	void	Set_Hit() { m_bHit = true; }
	_bool	Get_Hit() { return m_bHit; }

	void	Set_Swing(_bool bSwing) { m_bSwing = bSwing; }
	void	Set_ToAir(_bool bAir) { m_bToAir = bAir; }

	_float4 GetPxPostion();

public:
	void	SetOutline(_bool bOutLine) { m_bOutline = bOutLine; }

public:
	// For Boss Captured
	void	Set_Counter();
	void	Boss_Throw(_float4 vTargetPos);


	_bool	m_bCounter = false;



private:
	void	OutlineMaker();
	_bool	m_bOutline = false;
	_bool	m_bBeforeOutline = false;

public:
	void	KineticRimLightFix(_bool bFix) { m_bRimFix = bFix; }

private:
	void	BrightChecker();
	_float	m_fBright = 0.f;
	_float	m_fDissolve = 0.f;
	_bool	m_bRimFix = false;	// 림라이트 값 고정

private:
	_float	m_fTimeDelta = 0.f;

public:
	void	SetParticle();
	void	ReleaseParticle();

	void	CreateLargeParticle();

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

	_bool				IsUseBoss() { return m_bBossUse; }
	void				SetUseBoss() { m_bBossUse = true; }

	_bool				Usable() { return m_bUsable; }

	_bool				Get_CameRange() {
		return m_bCameRange;
	}
	void				Set_CameRange(const _bool bCameRange) {
		m_bCameRange = bCameRange;
	}

	_matrix				Get_PreMatrix();
	void				Set_PreMatrix(_matrix preMatrix);

private:
	_bool				m_bThrow = false;
	_bool				m_bUsable = true;
	_bool				m_bHit = false;
	_bool				m_bSwing = false;
	_float				m_fDeadTimer = 0.f;

	_bool				m_bCameRange = { false };

	_bool				m_bBossUse = false; 

	_bool				m_bToAir = false;

	_matrix				m_PreMatrix;

private:
	vector<wstring>		m_vecRandomParticle{
		L"Player_Kinetic_Particles_A",
		L"Player_Kinetic_Particles_B",
		L"Player_Kinetic_Particles_C",
		L"Player_Kinetic_Particles_D",
		L"Player_Kinetic_Particles_E",
		L"Player_Kinetic_Particles_F",
		L"Player_Kinetic_Particles_G"
	};

public:
	static CMapKinetic_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END

