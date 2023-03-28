#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_TankLorry_Head : public CSpecialObject
{
private:
	CSpecial_TankLorry_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_TankLorry_Head(const CSpecial_TankLorry_Head& rhs);
	virtual ~CSpecial_TankLorry_Head() = default;

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
	void	Set_ChildMatrix(_float4x4 matrix);
	void	Sync_position(CTransform* pTransform);
	void	Set_Kinetic(_bool bKinetic);
	void	Activate(_bool bActivate);

	void	Shake(_float fRange);
	void	Bounce(_float fForce);
	void	Exploision();

private:
	HRESULT	SetUp_Components(void* pArg);

private:
	_float4x4	m_SyncMatrix = XMMatrixIdentity();

public:
	static CSpecial_TankLorry_Head* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;

};

END