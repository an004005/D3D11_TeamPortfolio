#include "stdafx.h"
#include "..\public\Level_Hospital_1F.h"
#include "GameInstance.h"
#include "GameManager_Tutorial.h"
#include "Imgui_Batch.h"
#include "GameManager.h"

CLevel_Hospital_1F::CLevel_Hospital_1F(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_Hospital_1F::Initialize()
{
	// m_bPlayerSpawn = false;

	m_strLevelName = L"Hospital1F";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/Hospital_1F_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_Hospital_1F.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/BossBatch.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/MonsterBatch_Test.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Player_AI_Pos.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/SpecialKinetic_Test.json");

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

CLevel_Hospital_1F * CLevel_Hospital_1F::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Hospital_1F*		pInstance = new CLevel_Hospital_1F(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Hospital_1F");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Hospital_1F::Free()
{
	__super::Free();

}
