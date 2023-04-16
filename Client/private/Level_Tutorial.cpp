#include "stdafx.h"
#include "..\public\Level_Tutorial.h"
#include "GameInstance.h"
#include "GameManager_Tutorial.h"
#include "Imgui_Batch.h"

// PJW Add
#include "Map_KineticBatchPreset.h"
#include "PhysX_Manager.h"
#include "GameUtils.h"
#include "JsonStorage.h"

CLevel_Tutorial::CLevel_Tutorial(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_Tutorial::Initialize()
{
	 //m_bPlayerSpawn = false;

	m_strLevelName = L"Tutorial";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/Tutorial_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_Tutorial.json";

	m_BGM.CloneSound(m_MainSound); // ±âº» bgm

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Tutorial/TutorialBatch_Enemy.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Tutorial/Tutorial_UI.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Tutorial/MapKineticObject.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/Tutorial.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Alarm/Tutorial_NextMap.json");

	CGameManager_Tutorial::SetGameManager(CGameManager_Tutorial::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_Tutorial::Tick(_double TimeDelta)
{
	if (m_BGMOnce.IsNotDo())
		m_BGM.PlaySound(m_MainSound);

	CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);

	CLevel_StageDefault::Tick(TimeDelta);
}

CLevel_Tutorial * CLevel_Tutorial::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Tutorial*		pInstance = new CLevel_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tutorial::Free()
{
	__super::Free();

}
