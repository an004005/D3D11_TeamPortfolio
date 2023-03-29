#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"
#include "HelperClasses.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_TankLorry : public CSpecialObject
{
private:
	CSpecial_TankLorry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_TankLorry(const CSpecial_TankLorry& rhs);
	virtual ~CSpecial_TankLorry() = default;

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
	void TankLorry_Activate();
	void TankLorry_Shake(_float fRange);
	void TankLorry_Bounce(_float fForce);
	void TankLorry_Explosion();
	void TankLorry_Cheage_TankIndex(_uint iIndex);

private:
	_bool  m_bDeadCheck = false;
	_float m_fDeadTime = 0.f;

private:
	HRESULT	SetUp_Components(void* pArg);

private:
	void			Create_ChildObject();
	CGameObject*	m_pTankLorry_Head = nullptr;
	CGameObject*	m_pTankLorry_Trailer = nullptr;
	CGameObject*	m_pTankLorry_Tank = nullptr;

private:
	_float4x4		m_HeadMatrix;
	_float4x4		m_TrailerMatrix;

public:
	static CSpecial_TankLorry* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END