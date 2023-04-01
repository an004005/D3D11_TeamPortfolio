#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_IronBars_MultiBars : public CSpecialObject
{
private:
	CSpecial_IronBars_MultiBars(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_IronBars_MultiBars(const CSpecial_IronBars_MultiBars& rhs);
	virtual ~CSpecial_IronBars_MultiBars() = default;

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
	void	Attack_BoneMatrix_SetPoint(CModel* pModel, CTransform* pTransform, const string& strBoneName, _float4 vPoint);
	void	Set_Trigger(_bool bTrigger);
	void	Shooting(_float4 vTargetPos);

private:
	HRESULT	SetUp_Components(void* pArg);

	_bool	m_bHit = false;
	_float	m_fChangeKineticTimer = 0.f;

public:
	static CSpecial_IronBars_MultiBars* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;

};

END