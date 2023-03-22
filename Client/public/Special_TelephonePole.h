#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSpecial_TelephonePole : public CSpecialObject
{
private:
	CSpecial_TelephonePole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_TelephonePole(const CSpecial_TelephonePole& rhs);
	virtual ~CSpecial_TelephonePole() = default;

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
	void	TelephonePole_Bend(_float4 vPlayerPos, _float fForce);
	void	TelephonePole_PullOut(_float fForce);

public:
	void	SetUp_BoneMatrix(CModel* pModel, _fmatrix WorldMatrix);

private:
	HRESULT	SetUp_Components(void* pArg);

public:
	static CSpecial_TelephonePole* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END
