#include "stdafx.h"
#include "..\public\Level_Subway.h"
#include "GameInstance.h"
#include "Imgui_Batch.h"
#include "GameManager.h"

#include "Map_KineticBatchPreset.h"
#include "PhysX_Manager.h"
#include "GameUtils.h"
#include "JsonStorage.h"

#include "BronJon.h"
#include "MapInstance_Object.h"
#include "Material.h"
//#include "EM1200.h"


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

	m_BGM.CloneSound(m_MainSound); // ±âº» bgm

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
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Alarm/Subway.json");

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->LoadFogJson("../Bin/Resources/Batch/Subway_fog.json");


	for (auto pObj : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapInstanceObject")->GetGameObjects())
	{
		if (auto mapInst = dynamic_cast<CMapInstance_Object*>(pObj))
		{
			for (auto pMtrl : mapInst->Get_Model_Instancing()->GetMaterials())
			{
				if (pMtrl->GetInstancePass() == 14)
				{
					pMtrl->SetInstancePass(1);
				}
			}
		}
	}


	return S_OK;
}

void CLevel_Subway::Tick(_double TimeDelta)
{
	if (m_BGMOnce.IsNotDo())
		m_BGM.PlaySound(m_MainSound);

	CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);

	CLevel_StageDefault::Tick(TimeDelta);
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
