#include "stdafx.h"
#include "..\public\Boss_HpUI.h"
#include "GameInstance.h"

CBoss_HpUI::CBoss_HpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_HpUI::CBoss_HpUI(const CBoss_HpUI& rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_HpUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_HpUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBoss_HpUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
}

void CBoss_HpUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBoss_HpUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBoss_HpUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CBoss_HpUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CBoss_HpUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CBoss_HpUI * CBoss_HpUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CBoss_HpUI*		pInstance = new CBoss_HpUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_HpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CBoss_HpUI::Clone(void * pArg)
{
	CBoss_HpUI*		pInstance = new CBoss_HpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_HpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_HpUI::Free()
{
	__super::Free();

}
