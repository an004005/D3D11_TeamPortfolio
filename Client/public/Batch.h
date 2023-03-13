#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Level_Batch.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CBatch : public CGameObject
{
private:
	CBatch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBatch(const CBatch& rhs);
	virtual ~CBatch() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;
	void Imgui_RenderProperty() override;
	virtual void SaveToJson(OUT Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

private:
	HRESULT SetUp_Components();
	_float4	SetUp_InitPosition();
	void	RayPicking();

private:
	vector<class CTrigger*>				m_pTriggers;
	CTrigger*							m_pTrigger = nullptr;

private:
	vector<pair<wstring, BATCHTYPE>>	m_ProtosInfo;
	wstring 							m_pProtoTag = L"";

	vector<CGameObject*>				m_pGameObjects;
	CGameObject*						m_pGameObject = nullptr;
	_int								m_Filter = BATCHTYPE::MONSTER;
	_bool								m_bPick = false;
public:
	static CBatch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END

