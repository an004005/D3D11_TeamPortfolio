#include "stdafx.h"
#include "..\public\NextMapNameUI.h"
#include "GameInstance.h"
#include "Level_Loading_Simple.h"
#include "Level_Tutorial.h"
#include "Level_DownTown.h"
#include "Level_ConstructionSite3F.h"
#include "Level_ConstructionSite2F.h"
#include "Level_Subway.h"
#include "Level_NaomiRoom.h"
#include "Level_Hospital_1F.h"
#include "Level_FinalStage.h"

#include "Canvas_WorldMap.h"
#include "GameManager.h"

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

	if (CGameInstance::GetInstance()->KeyDown(DIK_E))
	{
		if (m_wsNextRoomName == L"대로")
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_TUTORIAL,
				CLevel_Loading_Simple::Create<CLevel_Tutorial>(m_pDevice, m_pContext));
		}
		else if (m_wsNextRoomName == L"스오-류신구")
		{
			if (LEVEL_NOW == LEVEL_TUTORIAL)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_DOWNTOWN_1,
					CLevel_Loading_Simple::Create<CLevel_DownTown>(m_pDevice, m_pContext));
			}
			else if (LEVEL_NOW == LEVEL_CONSTRUCTIONSITE_3F)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_DOWNTOWN_2,
					CLevel_Loading_Simple::Create<CLevel_DownTown_Second>(m_pDevice, m_pContext));
			}
			else if (LEVEL_NOW == LEVEL_CONSTRUCTIONSITE_2F)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_DOWNTOWN_3,
					CLevel_Loading_Simple::Create<CLevel_DownTown_Third>(m_pDevice, m_pContext));
			}
		}
		else if (m_wsNextRoomName == L"키쿠치바 3층")
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_CONSTRUCTIONSITE_3F,
				CLevel_Loading_Simple::Create<CLevel_ConstructionSite3F>(m_pDevice, m_pContext));
		}
		else if (m_wsNextRoomName == L"키쿠치바 2층")
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_CONSTRUCTIONSITE_2F,
				CLevel_Loading_Simple::Create<CLevel_ConstructionSite2F>(m_pDevice, m_pContext));
		}
		else if (m_wsNextRoomName == L"폐지하선로 스오 9호선")
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_SUBWAY,
				CLevel_Loading_Simple::Create<CLevel_Subway>(m_pDevice, m_pContext));
		}
		else if (m_wsNextRoomName == L"???")	 // 나오미방
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_NAOMIROOM,
				CLevel_Loading_Simple::Create<CLevel_NaomiRoom>(m_pDevice, m_pContext));
		}
		else if (m_wsNextRoomName == L"구 토벌군병원")
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_HOSPITAL_1F,
				CLevel_Loading_Simple::Create<CLevel_Hospital_1F>(m_pDevice, m_pContext));
		}
		else if (m_wsNextRoomName == L"????")	 // 최종보스방
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_FINAL_STAGE,
				CLevel_Loading_Simple::Create<CLevel_FinalStage>(m_pDevice, m_pContext));
		}

		CGameObject::SetDelete();
	}
}

void CNextMapNameUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CNextMapNameUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", m_wsNextRoomName.c_str(), GetScreenSpaceLeftTop() + _float2(55.0f, 17.0f), 0.f, { 0.35f, 0.35f });

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
