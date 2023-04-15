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
#include "BronJon.h"

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

	m_BGM.CloneSound("Ambient_Bridge");
	m_BGM.CloneSound("Attention Please");
	m_BGM.CloneSound("Abandoned Subway to Suoh Line 9"); // 몬스터 조우
	m_BGM.CloneSound("The OSF -Advance"); // 기본 bgm

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

	Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/ConstructionSite3F/Kinetic_Normal_BossRoom .json");
	CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);

	CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_ConstructionSite3F::Tick(_double TimeDelta)
{
	if (m_BGMOnce.IsNotDo())
		m_BGM.PlaySound("The OSF -Advance");

	if (m_bMiddleBGM == false)
	{
		if (auto pMonsterLayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Monster"))
		{
			for (auto pObj : pMonsterLayer->GetGameObjects())
			{
				if (auto pBoss = dynamic_cast<CBronJon*>(pObj))
				{
					m_BGM.StopAllLoop();
					m_bMiddleBGM = true;
					m_BGM.PlaySound("Abandoned Subway to Suoh Line 9");
					break;
				}
			}
		}
	}
	else
	{
		if (auto pMonsterLayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Monster"))
		{
			for (auto pObj : pMonsterLayer->GetGameObjects())
			{
				if (auto pBoss = dynamic_cast<CBronJon*>(pObj))
				{
					break;
				}
			}
			if (m_BGMChange.IsNotDo())
			{
				m_BGM.StopAllLoop();
				m_BGM.PlaySound("A Sedated Heart");
			}
		}
	}

	if (m_bBossBGM == false)
	{
		if (auto pMonsterLayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Monster"))
		{
			for (auto pObj : pMonsterLayer->GetGameObjects())
			{
				if (auto pBoss = dynamic_cast<CEM0320*>(pObj))
				{
					m_BGM.StopAllLoop();
					m_bBossBGM = true;
					m_BGM.PlaySound("em0320BGM");
					break;
				}
			}
		}
	}

	CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);

	CLevel::Tick(TimeDelta);
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
