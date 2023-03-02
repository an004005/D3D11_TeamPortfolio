#include "stdafx.h"
#include "..\public\Drive_GaugeUI.h"
#include "GameInstance.h"

CDrive_GaugeUI::CDrive_GaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CDrive_GaugeUI::CDrive_GaugeUI(const CDrive_GaugeUI& rhs)
	: CUI(rhs)
{
}

HRESULT CDrive_GaugeUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDrive_GaugeUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDrive_GaugeUI::BeginTick()
{


}

void CDrive_GaugeUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

}

void CDrive_GaugeUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CDrive_GaugeUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CDrive_GaugeUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CDrive_GaugeUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CDrive_GaugeUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CDrive_GaugeUI * CDrive_GaugeUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CDrive_GaugeUI*		pInstance = new CDrive_GaugeUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDrive_GaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CDrive_GaugeUI::Clone(void * pArg)
{
	CDrive_GaugeUI*		pInstance = new CDrive_GaugeUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDrive_GaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDrive_GaugeUI::Free()
{
	CUI::Free();

}
