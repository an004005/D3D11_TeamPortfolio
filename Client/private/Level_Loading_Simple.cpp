#include "stdafx.h"
#include "..\public\Level_Loading_Simple.h"
#include "GameInstance.h"


CLevel_Loading_Simple::CLevel_Loading_Simple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                                             std::function<CLevel*()>&& LevelCreator)
	: CLoadingLevel(pDevice, pContext, std::move(LevelCreator))
{
}


HRESULT CLevel_Loading_Simple::Initialize()
{
	CLoadingLevel::Initialize();
	return S_OK;
}

void CLevel_Loading_Simple::Tick(_double TimeDelta)
{
	CLoadingLevel::Tick(TimeDelta);
}

void CLevel_Loading_Simple::Late_Tick(_double TimeDelta)
{
	SetFinish();
}

HRESULT CLevel_Loading_Simple::Render()
{
	if (IsFinished())
	{
		SetWindowText(g_hWnd, TEXT("Level : Loading Finish"));
	}
	else
	{
		SetWindowText(g_hWnd, TEXT("Level : Loading"));
	}

	return CLoadingLevel::Render();
}

void CLevel_Loading_Simple::Free()
{
	CLoadingLevel::Free();
}


