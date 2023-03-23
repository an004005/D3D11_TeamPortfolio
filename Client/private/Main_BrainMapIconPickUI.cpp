#include "stdafx.h"
#include "..\public\Main_BrainMapIconPickUI.h"
#include "GameInstance.h"

CMain_BrainMapIconPickUI::CMain_BrainMapIconPickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_BrainMapIconPickUI::CMain_BrainMapIconPickUI(const CMain_BrainMapIconPickUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_BrainMapIconPickUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_BrainMapIconPickUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_BrainMapIconPickUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMain_BrainMapIconPickUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_BrainMapIconPickUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMain_BrainMapIconPickUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_BrainMapIconPickUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_BrainMapIconPickUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CMain_BrainMapIconPickUI * CMain_BrainMapIconPickUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_BrainMapIconPickUI*		pInstance = new CMain_BrainMapIconPickUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_BrainMapIconPickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_BrainMapIconPickUI::Clone(void * pArg)
{
	CMain_BrainMapIconPickUI*		pInstance = new CMain_BrainMapIconPickUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_BrainMapIconPickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_BrainMapIconPickUI::Free()
{
	__super::Free();

}
