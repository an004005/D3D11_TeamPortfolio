#include "stdafx.h"
#include "..\public\MouseCousorUI.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "MouseCousorLightUI.h"

CMouseCousorUI::CMouseCousorUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMouseCousorUI::CMouseCousorUI(const CMouseCousorUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMouseCousorUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMouseCousorUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/MouseCousorLightUI.json");
	m_pMouseLight = dynamic_cast<CMouseCousorLightUI*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"MouseCousorLightUI", &json));
	assert(m_pMouseLight != nullptr && "Failed to Clone : CMouseCousorLightUI");

	return S_OK;
}

void CMouseCousorUI::Tick(_double TimeDelta)
{
	if (false == CUI_Manager::GetInstance()->IsMouseActive()) 
	{
		m_bVisible = false;
		return;
	}

	m_bVisible = true;

	CUI::Tick(TimeDelta);
	m_pMouseLight->Tick(TimeDelta);

	POINT vCutsorPosition = CGameUtils::GetClientCursor();
	m_fX = static_cast<_float>(vCutsorPosition.x - (g_iWinSizeX * 0.5f));
	m_fY = static_cast<_float>(-vCutsorPosition.y + g_iWinSizeY * 0.5f);

	if(1 == m_tParams.Ints[0])
		m_pMouseLight->Set_Position({ m_fX, m_fY });

	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		CUI_Manager::GetInstance()->PlaySound("Mouseover");
		m_tParams.Ints[0] = 1;
		m_pMouseLight->Set_MouseLight(true);
	}
	if (CGameInstance::GetInstance()->KeyUp(CInput_Device::DIM_LB))
	{
		m_tParams.Ints[0] = 0;
		m_pMouseLight->Set_MouseLight(false);

	}
}

void CMouseCousorUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
	m_pMouseLight->Late_Tick(TimeDelta);

}

CMouseCousorUI * CMouseCousorUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMouseCousorUI*		pInstance = new CMouseCousorUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMouseCousorUI::Clone(void * pArg)
{
	CMouseCousorUI*		pInstance = new CMouseCousorUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMouseCousorUI::Free()
{
	Safe_Release(m_pMouseLight);
	__super::Free();

}
