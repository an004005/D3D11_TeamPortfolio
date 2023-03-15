#include "stdafx.h"
#include "..\public\DefaultUI.h"
#include "GameInstance.h"

CDefaultUI::CDefaultUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CDefaultUI::CDefaultUI(const CDefaultUI& rhs)
	: CUI(rhs)
{
}

HRESULT CDefaultUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDefaultUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDefaultUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CDefaultUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CDefaultUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CDefaultUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CDefaultUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CDefaultUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CDefaultUI * CDefaultUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CDefaultUI*		pInstance = new CDefaultUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CDefaultUI::Clone(void * pArg)
{
	CDefaultUI*		pInstance = new CDefaultUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDefaultUI::Free()
{
	__super::Free();

}
