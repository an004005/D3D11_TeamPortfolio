#include "stdafx.h"
#include "..\public\ItemIconUI.h"
#include "GameInstance.h"

// m_tParams.Float2s[0] : 이미지 참고 ^_^

CItemIconUI::CItemIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItemIconUI::CItemIconUI(const CItemIconUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItemIconUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CItemIconUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItemIconUI::Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	__super::Tick(TimeDelta);

}

void CItemIconUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItemIconUI::Set_IconIndex(const _float2 & fIconIndex)
{
	m_tParams.Float2s[0] = fIconIndex;
}

CItemIconUI * CItemIconUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItemIconUI*		pInstance = new CItemIconUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItemIconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItemIconUI::Clone(void * pArg)
{
	CItemIconUI*		pInstance = new CItemIconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItemIconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItemIconUI::Free()
{
	__super::Free();

}
