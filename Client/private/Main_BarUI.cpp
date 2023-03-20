#include "stdafx.h"
#include "..\public\Main_BarUI.h"
#include "GameInstance.h"

CMain_BarUI::CMain_BarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_BarUI::CMain_BarUI(const CMain_BarUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_BarUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_BarUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_BarUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMain_BarUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_BarUI::Set_Ber(const _float & fRatio)
{
	m_tParams.Floats[0] = fRatio;
}

CMain_BarUI * CMain_BarUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_BarUI*		pInstance = new CMain_BarUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_BarUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_BarUI::Clone(void * pArg)
{
	CMain_BarUI*		pInstance = new CMain_BarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_BarUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_BarUI::Free()
{
	__super::Free();

}
