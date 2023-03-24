#include "stdafx.h"
#include "..\public\Main_OnMouseColorUI.h"
#include "GameInstance.h"

CMain_OnMouseColorUI::CMain_OnMouseColorUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_OnMouseColorUI::CMain_OnMouseColorUI(const CMain_OnMouseColorUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_OnMouseColorUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_OnMouseColorUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	return S_OK;
}

void CMain_OnMouseColorUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMain_OnMouseColorUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_OnMouseColorUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMain_OnMouseColorUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_OnMouseColorUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_OnMouseColorUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CMain_OnMouseColorUI::Set_IconColor(const _float & fColor)
{
	m_tParams.Float2s[0].x = fColor;
}

CMain_OnMouseColorUI * CMain_OnMouseColorUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_OnMouseColorUI*		pInstance = new CMain_OnMouseColorUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_OnMouseColorUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_OnMouseColorUI::Clone(void * pArg)
{
	CMain_OnMouseColorUI*		pInstance = new CMain_OnMouseColorUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_OnMouseColorUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_OnMouseColorUI::Free()
{
	__super::Free();

}
