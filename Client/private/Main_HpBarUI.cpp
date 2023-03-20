#include "stdafx.h"
#include "..\public\Main_HpBarUI.h"
#include "GameInstance.h"

CMain_HpBarUI::CMain_HpBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_HpBarUI::CMain_HpBarUI(const CMain_HpBarUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_HpBarUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_HpBarUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_HpBarUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMain_HpBarUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_HpBarUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMain_HpBarUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_HpBarUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_HpBarUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CMain_HpBarUI * CMain_HpBarUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_HpBarUI*		pInstance = new CMain_HpBarUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_HpBarUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_HpBarUI::Clone(void * pArg)
{
	CMain_HpBarUI*		pInstance = new CMain_HpBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_HpBarUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_HpBarUI::Free()
{
	__super::Free();

}
