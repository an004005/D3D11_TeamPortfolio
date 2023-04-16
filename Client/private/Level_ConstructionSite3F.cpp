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

#include "EM0320.h"


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

	m_BGM.CloneSound(m_MainSound); // ±âº» bgm

	//Boss
	m_BGM.CloneSound("em0320BGM");

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
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Kinetic_Normal_Construction.json");
 	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/ConstructionSite3F.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/em210Test.json");

	// Red String Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/RedString_1.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/RedString_2.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/RedString_3.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/RedString_4.json");

	// AchieveItem Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Achieve_Batch.json");

	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Kinetic_Normal_BossRoom .json");
	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	CGameInstance::GetInstance()->LoadFogJson("../Bin/Resources/Batch/Construction3F_fog.json");
	return S_OK;
}

void CLevel_ConstructionSite3F::Tick(_double TimeDelta)
{
	if (m_BGMOnce.IsNotDo())
		m_BGM.PlaySound(m_MainSound);


	if (FindGameObjectInLayer<CEM0320>(L"Layer_Monster"))
	{
		if (m_bBossBGM == false)
		{
			m_BGM.StopAllLoop();
			m_bBossBGM = true;
			m_BGM.PlaySound("em0320BGM");
		}	
	}
	else
	{
		if (m_bBossBGM == true)
		{
			m_BGM.StopAllLoop();
			m_bBossBGM = false;
			m_BGM.PlaySound(m_MainSound);
		}
	}

	CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);

	CLevel_StageDefault::Tick(TimeDelta);
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
	__super::Free();
}
