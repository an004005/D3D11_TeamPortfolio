#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "ParticleSystem.h"

BEGIN(Client)
class CParticleGroup :	public CGameObject
{
public:
	// friend class CParticleSystem;

public:
	CParticleGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleGroup(const CParticleGroup& rhs);
	virtual ~CParticleGroup() = default;

public:
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_double TimeDelta) override;
	virtual void			SaveToJson(Json& json) override;
	virtual void			LoadFromJson(const Json& json) override;

	virtual void			Imgui_RenderProperty() override;

	void					Load_ParticleSystem();
	void					ImGui_RenderParticleSystem(_int iSelectParticle);
	void		VisibleUpdate();
	void		Set_Transform(_matrix sokect);

	void		Delete_Particles();

public:
	void		Start_ParticleWork();

	void		Start_NoAttachPivot(CGameObject* pOwner, _float4x4 PivotMatrix, _bool trueisUpdate = false, _bool trueisRemoveScale = false);

	void		Start_NoAttach(CGameObject* pOwner, _bool trueisUpdate = false, _bool trueisRemoveScale = false);
	void		Start_Attach(CGameObject* pOwner, string BoneName, _bool trueisUpdate = false, _bool trueisRemoveScale = false);
	void		Start_AttachPivot(CGameObject* pOwner, _float4x4 PivotMatrix, string BoneName, _bool usepivot = false, _bool trueisUpdate = false, _bool trueisRemoveScale = false);
	void		Start_AttachPosition(CGameObject* pOwner, _float4 vPosition, _float4 vDirection, _bool trueisUpdate = false);
	void		Start_AttachPosition_Scaling(CGameObject* pOwner, _float4 vPosition, _float4 vDirection, _float4 vScale, _bool trueisUpdate = false);
	void		Start_AttachSword(CGameObject* pWeapon, _bool trueisUpdate = false);

private:
	// [ObjectTag : Key] [Value : <Directory, Ptr>]
	unordered_map<string, pair<string, CParticleSystem*>> m_mapParticleSystem;
	_bool	m_bRemoveScale = false;

	_float  m_fLifeTime = 10.f;
	_bool	m_bGenerate = false;
	_bool	m_bUpdate = false;
	_bool	m_bUsePivot = false;
	string m_BoneName = "";
	_float4x4 m_PivotMatrix = XMMatrixIdentity();

private:
	CGameObject* m_pAttachWeapon = nullptr;

public:
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;
private:
	list<char*>		m_ParticleTag;

public:
	static CParticleGroup*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};



END
