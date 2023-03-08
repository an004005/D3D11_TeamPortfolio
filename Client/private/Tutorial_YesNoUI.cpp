#include "stdafx.h"
#include "..\public\Tutorial_YesNoUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CTutorial_YesNoUI::CTutorial_YesNoUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTutorial_YesNoUI::CTutorial_YesNoUI(const CTutorial_YesNoUI& rhs)
	: CUI(rhs)
{
}

HRESULT CTutorial_YesNoUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTutorial_YesNoUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTutorial_YesNoUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CTutorial_YesNoUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CTutorial_YesNoUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CTutorial_YesNoUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CTutorial_YesNoUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CTutorial_YesNoUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CTutorial_YesNoUI * CTutorial_YesNoUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTutorial_YesNoUI*		pInstance = new CTutorial_YesNoUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTutorial_YesNoUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CTutorial_YesNoUI::Clone(void * pArg)
{
	CTutorial_YesNoUI*		pInstance = new CTutorial_YesNoUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTutorial_YesNoUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_YesNoUI::Free()
{
	__super::Free();

}
