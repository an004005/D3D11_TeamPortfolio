#include "stdafx.h"
#include "..\public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"
#include "LoadingLevel.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Open_Loading(_uint iNewLevelIdx, CLoadingLevel* pLoadingLevel)
{
	if (nullptr == pLoadingLevel)
		return E_FAIL;

	m_bLoading = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if(nullptr != m_pCurrentLevel)
		pGameInstance->Clear_Level(m_iLevelIndex);
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pLoadingLevel;
	m_iLevelIndex = iNewLevelIdx;

	return S_OK;
}

HRESULT CLevel_Manager::Finish_Loading(class CLevel* pNewLevel)
{
	if (nullptr == pNewLevel)
		return E_FAIL;

	m_bLoading = false;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	// pGameInstance->Stop_All_Sound();

	if(nullptr != m_pCurrentLevel)
		pGameInstance->Clear_Level(LEVEL_LOADING);
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	return S_OK;
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == pNewLevel)
		return E_FAIL;	

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if(nullptr != m_pCurrentLevel)
		pGameInstance->Clear_Level(m_iLevelIndex);
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;
	m_iLevelIndex = iLevelIndex;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Manager::Tick(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(TimeDelta);
}

void CLevel_Manager::Late_Tick(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Late_Tick(TimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

_uint CLevel_Manager::GetUpdatedLevel() const
{
	if (m_bLoading)
		return LEVEL_LOADING;
	return m_iLevelIndex;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
