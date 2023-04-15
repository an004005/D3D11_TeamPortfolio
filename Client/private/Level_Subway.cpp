#include "stdafx.h"
#include "..\public\Level_Subway.h"
#include "GameInstance.h"
#include "Imgui_Batch.h"
#include "GameManager.h"

CLevel_Subway::CLevel_Subway(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_Subway::Initialize()
{
	//m_bPlayerSpawn = false;

	m_strLevelName = L"Subway";
	m_strShadowCamJsonPath.clear();   
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_Subway.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;
		
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/Player_AI_StartPos.json");

	// Kinetic Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/SpecialKientics_Train.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/SpecialKinetics.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/Kinetic_Normal.json");
	
	// Monster Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/MonsterBatches_1_Fix.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/MonsterBatches_2_Fix.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/MonsterBatches_3.json");
	
	// Item Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/Item_Batch.json");

	// AchieveItem Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/Achieve_Batch.json");

	// RedString Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Subway/RedStringBatch.json");

	// UI Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/Subway.json");

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

CLevel_Subway * CLevel_Subway::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Subway*		pInstance = new CLevel_Subway(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Subway");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Subway::Free()
{
	__super::Free();

}
