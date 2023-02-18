#include "stdafx.h"
#include "..\public\TextUI.h"
#include "GameInstance.h"

CTextUI::CTextUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTextUI::CTextUI(const CUI& rhs)
	: CUI(rhs)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

void CTextUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


}

void CTextUI::RenderFont()
{
	m_pGameInstance->Render_Font(m_strFont.c_str(), m_strText.c_str(), m_vFontPos, 0.f, m_vFontScale, m_vFontColor);
}

void CTextUI::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
