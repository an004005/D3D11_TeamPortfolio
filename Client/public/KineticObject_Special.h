#pragma once
#include "KineticObject.h"

BEGIN(Client)

class CKineticObject_Special :
	public CKineticObject
{
private:
	CKineticObject_Special(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKineticObject_Special(const CKineticObject_Special& rhs);
	virtual ~CKineticObject_Special() = default;

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

public:

	CGameObject* Clone(void* pArg = nullptr) override { return nullptr; }
	void Free() override;
};

END