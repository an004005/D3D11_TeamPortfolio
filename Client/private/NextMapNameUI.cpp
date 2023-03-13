#include "stdafx.h"
#include "..\public\NextMapNameUI.h"
#include "GameInstance.h"

CNextMapNameUI::CNextMapNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CNextMapNameUI::CNextMapNameUI(const CNextMapNameUI& rhs)
	: CUI(rhs)
{
}

HRESULT CNextMapNameUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNextMapNameUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	return S_OK;
}

void CNextMapNameUI::Tick(_double TimeDelta)
{
	if (false == m_bVisible)
		return;

	CUI::Tick(TimeDelta);

}

void CNextMapNameUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CNextMapNameUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", m_pNextRoomName, GetScreenSpaceLeftTop() + _float2(55.0f, 17.0f), 0.f, { 0.35f, 0.35f });

	return S_OK;
}

void CNextMapNameUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

CNextMapNameUI * CNextMapNameUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CNextMapNameUI*		pInstance = new CNextMapNameUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNextMapNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CNextMapNameUI::Clone(void * pArg)
{
	CNextMapNameUI*		pInstance = new CNextMapNameUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNextMapNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNextMapNameUI::Free()
{
	CUI::Free();

}
