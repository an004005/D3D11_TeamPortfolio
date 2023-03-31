#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_IronBars : public CSpecialObject
{
private:
	CSpecial_IronBars(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_IronBars(const CSpecial_IronBars& rhs);
	virtual ~CSpecial_IronBars() = default;

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
	void	IronBars_AnimActive(_bool bActive);
	void	IronBars_Decompose(_bool bDecompose);
	void	IronBars_SetTrigger(_bool bTrigger);

	void	IronBars_AttachAnim(CModel* pModel, CTransform* pTransform, _float4 vPoint);

	void	IronBars_LookAtTarget(_float4 vTargetPos, _float fRatio);
	void	IronBars_Shooting_All(_float4 vTargetPos);
	void	IronBars_Shooting_Single(_float4 vTargetPos, _uint iIndex);

	void	IronBars_Reload(_float4 vDestPos, _float4 vTargetPos, _float fRatio);

	void	IronBars_LerpAnim(CModel* pModel, CTransform* pTransform, _float4 vPoint, _float fRatio);

	void	IronBars_ChangeToBundle();
	void	IronBars_AttachAnim_MulitBars(CModel* pModel, CTransform* pTransform, _float4 vPoint);
	void	IronBars_Shooting_Finish(_float4 vTargetPos);

private:
	HRESULT	SetUp_Components(void* pArg);

private:
	HRESULT		 Create_Child();
	CGameObject* m_pDoor = nullptr;
	CGameObject* m_pBars = nullptr;
	CGameObject* m_pSingleBar[8] = { nullptr, };
	CGameObject* m_pMultiBars = nullptr;

private:
	_float4x4		m_DoorSyncMatrix = XMMatrixIdentity();
	_float4x4		m_BarsSyncMatrix = XMMatrixIdentity();
	vector<string>	m_vecSingleBarBoneName;

private:
	_bool			m_bDecompose = false;
	_bool			m_bBundleCompose = false;

public:
	static CSpecial_IronBars* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;

};

END