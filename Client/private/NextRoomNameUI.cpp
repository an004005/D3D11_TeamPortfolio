#include "stdafx.h"
#include "..\public\NextRoomNameUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CNextRoomNameUI::CNextRoomNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CNextRoomNameUI::CNextRoomNameUI(const CNextRoomNameUI& rhs)
	: CUI(rhs)
{
}

HRESULT CNextRoomNameUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNextRoomNameUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	return S_OK;
}

void CNextRoomNameUI::Tick(_double TimeDelta)
{
	if (false == m_bVisible)
		return;

	__super::Tick(TimeDelta);


}

void CNextRoomNameUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CNextRoomNameUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", m_pNextRoomName, GetScreenSpaceLeftTop() + _float2(55.0f, 17.0f), 0.f, { 0.35f, 0.35f });

	return S_OK;
}

void CNextRoomNameUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CNextRoomNameUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CNextRoomNameUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CNextRoomNameUI * CNextRoomNameUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNextRoomNameUI*      pInstance = new CNextRoomNameUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNextRoomNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CNextRoomNameUI::Clone(void * pArg)
{
	CNextRoomNameUI*      pInstance = new CNextRoomNameUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNextRoomNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNextRoomNameUI::Free()
{
	__super::Free();

}