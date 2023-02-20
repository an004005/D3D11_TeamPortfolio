#include "stdafx.h"
#include "Level_Loading_MultiPlay.h"
#include "GameInstance.h"
#include "JsonStorage.h"

CLevel_Loading_MultiPlay::CLevel_Loading_MultiPlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                                             std::function<CLevel*()>&& LevelCreator)
	: CLoadingLevel(pDevice, pContext, std::move(LevelCreator))
{
}


HRESULT CLevel_Loading_MultiPlay::Initialize()
{
	CLoadingLevel::Initialize();

	return S_OK;
}

void CLevel_Loading_MultiPlay::Tick(_double TimeDelta)
{
	CLoadingLevel::Tick(TimeDelta);
}

void CLevel_Loading_MultiPlay::Late_Tick(_double TimeDelta)
{
	// if (m_bLoadingFinished == false && IsFinished())
	// {
	// 	m_bLoadingFinished = true;
	//
	// 	GClientService->DoAsync(&CClientServiceMgr::SendLoadingFinish);
	// }
	//
	// if (GClientService->IsAllPlayerLoadingFinish())
	// {
	// 	SetFinish();
	// }
}

HRESULT CLevel_Loading_MultiPlay::Render()
{
	if (m_bLoadingFinished)
	{
		SetWindowText(g_hWnd, TEXT("Level : Loading; Waiting for other players"));
		
	}
	else
	{
		SetWindowText(g_hWnd, TEXT("Level : Loading"));
	}


	return CLoadingLevel::Render();
}

void CLevel_Loading_MultiPlay::Free()
{
	CLoadingLevel::Free();
}


