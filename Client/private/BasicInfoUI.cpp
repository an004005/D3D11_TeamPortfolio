#include "stdafx.h"
#include "..\public\BasicInfoUI.h"
#include "GameInstance.h"

CBasicInfoUI::CBasicInfoUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBasicInfoUI::CBasicInfoUI(const CBasicInfoUI& rhs)
	: CUI(rhs)
{
}

HRESULT CBasicInfoUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBasicInfoUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBasicInfoUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


}

void CBasicInfoUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBasicInfoUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBasicInfoUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CBasicInfoUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CBasicInfoUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CBasicInfoUI * CBasicInfoUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CBasicInfoUI*		pInstance = new CBasicInfoUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBasicInfoUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CBasicInfoUI::Clone(void * pArg)
{
	CBasicInfoUI*		pInstance = new CBasicInfoUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBasicInfoUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBasicInfoUI::Free()
{
	__super::Free();

}
