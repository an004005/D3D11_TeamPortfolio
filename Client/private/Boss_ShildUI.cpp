#include "stdafx.h"
#include "..\public\Boss_ShildUI.h"
#include "GameInstance.h"

CBoss_ShildUI::CBoss_ShildUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_ShildUI::CBoss_ShildUI(const CBoss_ShildUI& rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_ShildUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_ShildUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBoss_ShildUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
}

void CBoss_ShildUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBoss_ShildUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBoss_ShildUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CBoss_ShildUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CBoss_ShildUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CBoss_ShildUI * CBoss_ShildUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CBoss_ShildUI*		pInstance = new CBoss_ShildUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_ShildUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CBoss_ShildUI::Clone(void * pArg)
{
	CBoss_ShildUI*		pInstance = new CBoss_ShildUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_ShildUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_ShildUI::Free()
{
	__super::Free();

}
