#include "stdafx.h"
#include "..\public\Main_OnMouseUI.h"
#include "GameInstance.h"
#include "GameUtils.h"

CMain_OnMouseUI::CMain_OnMouseUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_OnMouseUI::CMain_OnMouseUI(const CMain_OnMouseUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_OnMouseUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_OnMouseUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}

void CMain_OnMouseUI::Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	__super::Tick(TimeDelta);

	if (true == IsCursorOn(CGameUtils::GetClientCursor()))
		m_bOnMouse = true;
	else
		m_bOnMouse = false;

	if (true == IsCursorOn(CGameUtils::GetClientCursor()) && CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
		m_bOnButton = true;

	if (true == IsCursorOn(CGameUtils::GetClientCursor()) && CGameInstance::GetInstance()->KeyUp(CInput_Device::DIM_LB))
		m_bOnButton = false;

}

void CMain_OnMouseUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_OnMouseUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

void CMain_OnMouseUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_OnMouseUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_OnMouseUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CMain_OnMouseUI::CurrentOnMouse(const _double & TimeDelta)
{

}

CMain_OnMouseUI * CMain_OnMouseUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_OnMouseUI*		pInstance = new CMain_OnMouseUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_OnMouseUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_OnMouseUI::Clone(void * pArg)
{
	CMain_OnMouseUI*		pInstance = new CMain_OnMouseUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_OnMouseUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_OnMouseUI::Free()
{
	__super::Free();

}
