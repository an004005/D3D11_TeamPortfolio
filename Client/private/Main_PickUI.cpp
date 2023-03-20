#include "stdafx.h"
#include "..\public\Main_PickUI.h"
#include "GameInstance.h"
#include "GameUtils.h"

CMain_PickUI::CMain_PickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_PickUI::CMain_PickUI(const CMain_PickUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_PickUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_PickUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_PickUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (true == IsCursorOn(CGameUtils::GetClientCursor()) &&
		CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		m_vOnButton = true;
		m_vOnAlpha = true;
		IM_LOG("Button %i", _int(m_vOnButton));
	}

	CurrentPick(TimeDelta);


}

void CMain_PickUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_PickUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//_float2 vPosition = GetScreenSpaceLeftTop();
	//pGameInstance->Render_Font(L"Pretendard32", m_szText, vPosition + m_vPosition, 0.f, m_vSize/*, { 1.0f, 0.99f, 0.87f, 1.0f }*/);


	return S_OK;
}

void CMain_PickUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_PickUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_PickUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CMain_PickUI::CurrentPick(const _double & TimeDelta)
{
	if (false == m_vOnAlpha)
	{
		m_fAlpha = 0.1f;
		m_tParams.Float4s[0] = { 0.0f, 0.0f, 0.0f, m_fAlpha };
		return;
	}

	if (true == m_vAlphaDwon)
	{
		m_fAlpha -= _float(TimeDelta) * 0.15f;

		if (0.05f > m_fAlpha)
		{
			m_vAlphaDwon = false;
		}
	}
	else
	{
		m_fAlpha += _float(TimeDelta) * 0.15f;

		if (0.1f < m_fAlpha)
		{
			m_vAlphaDwon = true;
		}
	}

	m_tParams.Float4s[0] = { 1.0f, 1.0f, 1.0f, m_fAlpha };
}

CMain_PickUI * CMain_PickUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_PickUI*		pInstance = new CMain_PickUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_PickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_PickUI::Clone(void * pArg)
{
	CMain_PickUI*		pInstance = new CMain_PickUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_PickUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_PickUI::Free()
{
	__super::Free();

}
