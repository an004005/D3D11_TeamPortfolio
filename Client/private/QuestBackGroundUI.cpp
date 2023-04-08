#include "stdafx.h"
#include "..\public\QuestBackGroundUI.h"
#include "GameInstance.h"

CQuestBackGroundUI::CQuestBackGroundUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CQuestBackGroundUI::CQuestBackGroundUI(const CQuestBackGroundUI& rhs)
	: CUI(rhs)
{
}

HRESULT CQuestBackGroundUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestBackGroundUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CQuestBackGroundUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

CQuestBackGroundUI * CQuestBackGroundUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CQuestBackGroundUI*		pInstance = new CQuestBackGroundUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuestBackGroundUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CQuestBackGroundUI::Clone(void * pArg)
{
	CQuestBackGroundUI*		pInstance = new CQuestBackGroundUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuestBackGroundUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuestBackGroundUI::Free()
{
	__super::Free();

}
