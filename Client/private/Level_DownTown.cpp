#include "stdafx.h"
#include "Level_DownTown.h"

#include "GameManager.h"
#include "Imgui_Batch.h"

/*********************
 * CLevel_DownTown
 **********************/
CLevel_DownTown::CLevel_DownTown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_DownTown::Initialize()
{
	 // m_bPlayerSpawn = false;

	m_strLevelName = L"DownTown1";

	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/DownTown_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_DownTown.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/DownTown1.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Npc/DownTown1_Npc.json");

	return S_OK;
}

CLevel_DownTown* CLevel_DownTown::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_DownTown*		pInstance = new CLevel_DownTown(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_DownTown");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_DownTown::Free()
{
	CLevel_StageDefault::Free();
}



/*********************
 * CLevel_DownTown_Second
 **********************/
CLevel_DownTown_Second::CLevel_DownTown_Second(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_DownTown_Second::Initialize()
{
	 // m_bPlayerSpawn = false;

	m_strLevelName = L"DownTown1";

	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/DownTown_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_DownTown.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

CLevel_DownTown_Second* CLevel_DownTown_Second::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_DownTown_Second*		pInstance = new CLevel_DownTown_Second(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_DownTown_Second");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_DownTown_Second::Free()
{
	CLevel_StageDefault::Free();
}


/*********************
 * CLevel_DownTown_Third
 **********************/
CLevel_DownTown_Third::CLevel_DownTown_Third(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_DownTown_Third::Initialize()
{
	 // m_bPlayerSpawn = false;

	m_strLevelName = L"DownTown1";

	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/DownTown_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_DownTown.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

CLevel_DownTown_Third* CLevel_DownTown_Third::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_DownTown_Third*		pInstance = new CLevel_DownTown_Third(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_DownTown_Third");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_DownTown_Third::Free()
{
	CLevel_StageDefault::Free();
}