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

private:
	// [ObjectTag : Key] [Value : <Directory, Ptr>]
	unordered_map<string, pair<string, CParticleSystem*>> m_mapParticleSystem;
public:
	virtual void Late_Tick(_double TimeDelta) override;
private:
	list<char*>		m_ParticleTag;

public:
	static CParticleGroup*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};



END
