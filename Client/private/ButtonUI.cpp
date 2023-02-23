#include "stdafx.h"
#include "..\public\ButtonUI.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "GameUtils.h"

CButtonUI::CButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CButtonUI::CButtonUI(const CButtonUI& rhs)
	: CUI(rhs)
{
}

HRESULT CButtonUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CButtonUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CButtonUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (true == IsCursorOn(CGameUtils::GetClientCursor()) && pGameInstance->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		IM_LOG("gg");
	}
}

HRESULT CButtonUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CButtonUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

}

CButtonUI * CButtonUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CButtonUI*		pInstance = new CButtonUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CButtonUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CButtonUI::Clone(void * pArg)
{
	CButtonUI*		pInstance = new CButtonUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CButtonUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CButtonUI::Free()
{
	__super::Free();

}
