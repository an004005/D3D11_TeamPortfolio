#include "stdafx.h"
#include "..\public\Level_Hospital_1F.h"
#include "GameInstance.h"
#include "GameManager_Tutorial.h"
#include "Imgui_Batch.h"
#include "GameManager.h"

// PJW Add
#include "Map_KineticBatchPreset.h"
#include "PhysX_Manager.h"
#include "GameUtils.h"
#include "JsonStorage.h"

#include "EM1100.h"
#include "BronJon.h"

#include "Level_Loading_Simple.h"
#include "Level_FinalStage.h"
#include "PlayerInfoManager.h"

CLevel_Hospital_1F::CLevel_Hospital_1F(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_Hospital_1F::Initialize()
{
	 //m_bPlayerSpawn = false;

	m_strLevelName = L"Hospital1F";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/Hospital_1F_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_Hospital_1F.json";

	m_BGM.CloneSound(m_MainSound); // 기본 bgm

	//Boss
	m_BGM.CloneSound("em1100BGM");

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/BossBatch.json");
	//CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/MonsterBatch_Test.json"); 기존 몬스터 배치 테스트
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Player_AI_Pos.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/SpecialKinetic_Test.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/SpecialKinetic_Boss.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/BossRoom_MonsterBatch.json");
	//CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/StartPosition.json");
	
	// 2F
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Normal_Kinetic.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Normal_Kinetic_2F_Outside.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Normal_Kinetic_2F_Inside.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Normal_Kinetic_Box_Batch.json");
		
	// 2F Monster 
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Monster_Batch_2F_1st.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Monster_Batch_2F_2nd.json");
		
	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/Hospital1F/Normal_Kinetic_2F_BossRoom.json");
	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);

	// 1F 
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Normal_Kinetic_1F.json");

	// 1F 문어잡을 때쯤 물체 없어서 프레셋
	Json gardenJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/Hospital1F/Kinetic_Normal_1F_garden.json");
	CMap_KineticBatchPreset::GetInstance()->Initialize(gardenJson);
	
	// 1F Monster
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Monster_Batch_1F_1stT.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Monster_Batch_1F_Garden.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Monster_Batch_1F_Trap.json");
	
	// Teleport_Wall
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Teleport_Wall_BatchTest.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Teleport_Wall_Batch.json");

	// Item Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Item_Batch.json");
	
	// AchieveItem Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/Achieve_Batch.json");

	// RedString Batch 
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Hospital1F/RedString_Batch.json");

	// Story Batch
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/Hospital_1F.json");

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_Hospital_1F::Tick(_double TimeDelta)
{
	if (CPlayerInfoManager::GetInstance()->GetTeleportDissolve() >= 3.f)
	{
		CGameInstance::GetInstance()->Open_Loading(
			LEVEL_FINAL_STAGE,
			CLevel_Loading_Simple::Create<CLevel_FinalStage>(m_pDevice, m_pContext));
	}

	if (m_BGMOnce.IsNotDo())
		m_BGM.PlaySound(m_MainSound);


	if (FindGameObjectInLayer<CEM1100>(L"Layer_Monster"))
	{
		if (m_bBossBGM == false)
		{
			m_BGM.StopAllLoop();
			m_bBossBGM = true;
			m_BGM.PlaySound("em1100BGM");
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
