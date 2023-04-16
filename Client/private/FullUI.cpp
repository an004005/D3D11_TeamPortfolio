#include "stdafx.h"
#include "..\public\FullUI.h"
#include "GameInstance.h"
#include "Level_Loading_Simple.h"
#include "Level_Tutorial.h"

CFullUI::CFullUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CFullUI::CFullUI(const CFullUI& rhs)
	: CUI(rhs)
{
}

HRESULT CFullUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFullUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CFullUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fSizeX = _float(g_iWinSizeX);
	m_fSizeY = _float(g_iWinSizeY);


	if (LEVEL_NOW == LEVEL_LOGO)
	{
		if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_TUTORIAL,
				CLevel_Loading_Simple::Create<CLevel_Tutorial>(m_pDevice, m_pContext));
		}
	}
}

CFullUI * CFullUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CFullUI*		pInstance = new CFullUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFullUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CFullUI::Clone(void * pArg)
{
	CFullUI*		pInstance = new CFullUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFullUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFullUI::Free()
{
	__super::Free();

}
