#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_IronBars_Bars : public CSpecialObject
{
private:
	CSpecial_IronBars_Bars(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_IronBars_Bars(const CSpecial_IronBars_Bars& rhs);
	virtual ~CSpecial_IronBars_Bars() = default;

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
	void	Set_Activate(_bool bActive) { m_bActive = bActive; }
	void	PlayAnim(_double TimeDelta);
	void	Set_SyncMatrix(_float4x4 SyncMatrix) { m_SyncMatrix = SyncMatrix; }
	void	Sync_Position(CTransform* pTransform);

public:
	CModel* Get_Model() { return m_pModelComs.front(); }

private:
	HRESULT	SetUp_Components(void* pArg);
	_bool	m_bActive = false;

private:
	_float4x4	m_SyncMatrix = XMMatrixIdentity();

public:
	static CSpecial_IronBars_Bars* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;

};

END