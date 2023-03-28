#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_TankLorry_Tank : public CSpecialObject
{
private:
	CSpecial_TankLorry_Tank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_TankLorry_Tank(const CSpecial_TankLorry_Tank& rhs);
	virtual ~CSpecial_TankLorry_Tank() = default;

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

private:
	HRESULT	SetUp_Components(void* pArg);

private:
	_int	m_iCurModelIdx = 0;

public:
	static CSpecial_TankLorry_Tank* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END
