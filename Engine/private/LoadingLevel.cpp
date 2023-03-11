#include "stdafx.h"
#include "..\public\LoadingLevel.h"
#include "Level_Manager.h"

CLoadingLevel::CLoadingLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::function<CLevel*()>&& LevelCreator)
	: CLevel(pDevice, pContext), m_LevelCreator(std::move(LevelCreator))
{
}

HRESULT CLoadingLevel::Initialize()
{
	if (FAILED(CLevel::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CLoadingLevel::StartLoading()
{
	m_LoadingThread = std::thread([this]()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		m_pCreatedLevel = m_LevelCreator();
		m_bFinished.store(true);
	});
	m_LoadingThread.detach();
}

void CLoadingLevel::Free()
{
	CLevel::Free();
	if (m_LoadingThread.joinable())
		m_LoadingThread.join();
}

void CLoadingLevel::SetFinish()
{
	if (m_bFinished)
	{
		CLevel_Manager::GetInstance()->Finish_Loading(m_pCreatedLevel);
	}
}
