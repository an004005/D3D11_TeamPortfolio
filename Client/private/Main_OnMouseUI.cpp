#include "stdafx.h"
#include "..\public\Main_OnMouseUI.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "JsonStorage.h"

#include "Main_OnMouseColorUI.h"

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
	m_pMouseColorUI->Tick(TimeDelta);

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
	m_pMouseColorUI->Late_Tick(TimeDelta);
	//m_pMouseColorUI->SetVisible(true);

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

void CMain_OnMouseUI::Set_BrainInfo(CItem_Manager::BRAININFO tBrainInfo)
{
	m_BrainInfo = tBrainInfo;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Main_OnMouseColorUI.json");
	m_pMouseColorUI = dynamic_cast<CMain_OnMouseColorUI*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Main_OnMouseColorUI", &json));
	
	_float fColor = 0.0f;
	if (CItem_Manager::GREEN == m_BrainInfo.eColor)
		fColor = 0.0f;
	else if (CItem_Manager::YELLOW == m_BrainInfo.eColor)
		fColor = 1.0f;
	else if (CItem_Manager::RAD == m_BrainInfo.eColor)
		fColor = 2.0f;
	else if (CItem_Manager::PURPLE == m_BrainInfo.eColor)
		fColor = 3.0f;

	m_pMouseColorUI->Set_IconColor(fColor);
	m_pMouseColorUI->Set_Position({ m_fX, m_fY });
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
