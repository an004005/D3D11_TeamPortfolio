#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_HBeam_Single : public CSpecialObject
{
private:
	CSpecial_HBeam_Single(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_HBeam_Single(const CSpecial_HBeam_Single& rhs);
	virtual ~CSpecial_HBeam_Single() = default;

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
	void	Sync_position(CTransform* pTransform);
	void	Set_Trigger(_bool bTrigger);
	void	Set_Kinetic(_bool bKinetic);
	void	HBeam_Turn();
	void	HBeam_Finish();
	void	Activate(_bool bActivate);
	void	AddVelocity(_float3 vVelocity);
	_float4 GetOriginDir() { return m_vOriginDir; }

private:
	HRESULT	SetUp_Components(void* pArg);

private:
	_bool	m_bFinish = false;
	_float4 m_vOriginDir = { 0.f, 0.f, 0.f, 0.f };
	_float4 m_vBeforePos = { 0.f, 0.f, 0.f, 1.f };
	_float	m_fDecomposeTimer = 0.f;

public:
	static CSpecial_HBeam_Single* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END