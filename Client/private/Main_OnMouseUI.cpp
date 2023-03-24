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

	if (false == m_bVisible)
	{
		m_pMouseColorUI->SetVisible(false);
		return;
	}
	else
	{
		if(true == m_BrainInfo.bUse)
			m_pMouseColorUI->SetVisible(true);
	}

	__super::Tick(TimeDelta);

	if (true == IsCursorOn(CGameUtils::GetClientCursor()))
		m_bOnMouse = true;
	else
		m_bOnMouse = false;

	if (true == IsCursorOn(CGameUtils::GetClientCursor()) && CGameInstance::GetInstance()->KeyPressing(CInput_Device::DIM_LB))
		m_bOnButton = true;

	if (true == IsCursorOn(CGameUtils::GetClientCursor()) && CGameInstance::GetInstance()->KeyUp(CInput_Device::DIM_LB))
		m_bOnButton = true;

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

void CMain_OnMouseUI::Set_BrainInfo(CCanvas_BrainMap::BRAININFO tBrainInfo)
{
	// 각 아이콘의 정보를 셋팅하면서
	m_BrainInfo = tBrainInfo;

	// 뒤에 뜨는 색상 Icon 생성하기
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Main_OnMouseColorUI.json");
	m_pMouseColorUI = dynamic_cast<CMain_OnMouseColorUI*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Main_OnMouseColorUI", &json));
	
	_float fColor = 0.0f;
	if (CCanvas_BrainMap::GREEN == m_BrainInfo.eColor)
		fColor = 0.0f;
	else if (CCanvas_BrainMap::YELLOW == m_BrainInfo.eColor)
		fColor = 1.0f;
	else if (CCanvas_BrainMap::RAD == m_BrainInfo.eColor)
		fColor = 2.0f;
	else if (CCanvas_BrainMap::PURPLE == m_BrainInfo.eColor)
		fColor = 3.0f;

	m_pMouseColorUI->Set_IconColor(fColor);
	m_pMouseColorUI->Set_Position({ m_fX, m_fY });
}

void CMain_OnMouseUI::Set_IconIndex(const _float2 & fIndex)
{
	// 레벨에 따라 구매할 수 있으면 흰색 구매할 수 없으면 검정색 아이콘 설정
	m_tParams.Float2s[0] = fIndex;
}

void CMain_OnMouseUI::Set_BrainUse()
{
	// 아이콘 흰색으로 변경
	m_tParams.Float2s[0] = m_BrainInfo.vOnIconIndex;
	// 이 브레인은 이제 사용할 수 있음
	m_BrainInfo.bUse = true;
	// 그러면서 처음에 생성해 둔 Icon Color 도 출력한다.
	m_pMouseColorUI->SetVisible(true);
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
	Safe_Release(m_pMouseColorUI);

	__super::Free();

}
