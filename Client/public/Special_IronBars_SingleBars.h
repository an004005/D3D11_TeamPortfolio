#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_IronBars_SingleBars : public CSpecialObject
{
private:
	CSpecial_IronBars_SingleBars(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_IronBars_SingleBars(const CSpecial_IronBars_SingleBars& rhs);
	virtual ~CSpecial_IronBars_SingleBars() = default;

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
	void	Attach_BoneMatrix(CModel* pModel, CTransform* pTransform, const string& strBoneName);
	void	Set_Kinetic(_bool bKinetic);

private:
	HRESULT	SetUp_Components(void* pArg);
	_bool	m_bActive = false;

private:
	_float4x4	m_SyncMatrix = XMMatrixIdentity();

public:
	static CSpecial_IronBars_SingleBars* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;

};

END