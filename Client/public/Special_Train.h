#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSpecial_Train : public CSpecialObject
{
private:
	CSpecial_Train(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_Train(const CSpecial_Train& rhs);
	virtual ~CSpecial_Train() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX();
	HRESULT Render() override;

	virtual void LoadFromJson(const Json& json) override {}
	virtual void SaveToJson(Json& json) override {}

	virtual void Imgui_RenderProperty() override;

private:
	HRESULT	SetUp_Components(void* pArg);

public:
	static CSpecial_Train* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END