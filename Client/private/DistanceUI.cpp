#include "stdafx.h"
#include "..\public\DistanceUI.h"
#include "GameInstance.h"

CDistanceUI::CDistanceUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CDistanceUI::CDistanceUI(const CDistanceUI& rhs)
	: CUI(rhs)
{
}

HRESULT CDistanceUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDistanceUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDistanceUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

CDistanceUI * CDistanceUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CDistanceUI*		pInstance = new CDistanceUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDistanceUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CDistanceUI::Clone(void * pArg)
{
	CDistanceUI*		pInstance = new CDistanceUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDistanceUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDistanceUI::Free()
{
	__super::Free();

}
