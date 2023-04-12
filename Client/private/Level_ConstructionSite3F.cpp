#include "stdafx.h"
#include "..\public\Level_ConstructionSite3F.h"
#include "GameInstance.h"
#include "Imgui_Batch.h"
#include "GameManager.h"

// PJW Add
#include "Map_KineticBatchPreset.h"
#include "PhysX_Manager.h"
#include "GameUtils.h"
#include "JsonStorage.h"

CLevel_ConstructionSite3F::CLevel_ConstructionSite3F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_ConstructionSite3F::Initialize()
{
	 //m_bPlayerSpawn = false;

	m_strLevelName = L"ConstructionSite3F";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/ConstructionSite3F_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_ConstructionSite3F.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Boss_HBeams.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Monster_PlayerStart.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/SpecialKinetics.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/MonsterBatch.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Kinetic_Normal_Construction.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Item_Batch.json");

	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Kinetic_Normal_BossRoom .json");
	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_ConstructionSite3F::Tick(_double TimeDelta)
{
	CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);
}

CLevel_ConstructionSite3F* CLevel_ConstructionSite3F::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_ConstructionSite3F*		pInstance = new CLevel_ConstructionSite3F(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_ConstructionSite3F");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_ConstructionSite3F::Free()
{
	CLevel::Free();
}
