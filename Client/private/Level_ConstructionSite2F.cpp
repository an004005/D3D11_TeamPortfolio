#include "stdafx.h"
#include "Level_ConstructionSite2F.h"
#include "GameInstance.h"
#include "Imgui_Batch.h"
#include "GameManager.h"

// PJW Add
#include "Map_KineticBatchPreset.h"
#include "PhysX_Manager.h"
#include "GameUtils.h"
#include "JsonStorage.h"

#include "EM0110.h"
#include "BronJon.h"


CLevel_ConstructionSite2F::CLevel_ConstructionSite2F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_ConstructionSite2F::Initialize()
{
   // m_bPlayerSpawn = false;

    m_strLevelName = L"ConstructionSite2F";
    m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/ConstructionSite2F_ShadowCam.json"; 
    m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_ConstructionSite2F.json";

	m_BGM.CloneSound(m_MainSound); // ±âº» bgm

	//Boss
	m_BGM.CloneSound("em0110BGM");

    if (FAILED(__super::Initialize()))
        return E_FAIL;
    if (FAILED(Ready_Layer_AI(LAYER_AI)))
        return E_FAIL;

    // Kinetic Object Batch & Item
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Kinetic_Normal_ConstructionSite2F.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Item_Batch.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Kinetic_Special_2F.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/Flower.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/StartPosition.json");

    // Monster Trigger(with Invisible Wall)
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Trigger_Monster1stWave.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Trigger_Monster2ndWave_Fix.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Trigger_Monster3rdWave.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Trigger_Monster4thWaveBoss.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/ConstructionSite2F.json");

    // AchieveItem Batch
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Achieve_Batch.json");
    
    // Red String Batch
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedString_1.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedString_2.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedString_3.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedString_4.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedPaticle.json");

    // Boss Area Kinetic Object Preset(Recycle)
    Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Kinetic_Normal_Construction2FBossRoom.json");
    CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);

    CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));
    CGameInstance::GetInstance()->LoadFogJson("../Bin/Resources/Batch/Construction2F_Fog.json");

    return S_OK;
}

void CLevel_ConstructionSite2F::Tick(_double TimeDelta)
{
    if (m_BGMOnce.IsNotDo())
        m_BGM.PlaySound(m_MainSound);


    if (FindGameObjectInLayer<CEM0110>(L"Layer_Monster"))
    {
        if (m_bBossBGM == false)
        {
            m_BGM.StopAllLoop();
            m_bBossBGM = true;
            m_BGM.PlaySound("em0110BGM");
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

CLevel_ConstructionSite2F* CLevel_ConstructionSite2F::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_ConstructionSite2F* pInstance = new CLevel_ConstructionSite2F(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_ConstructionSite2F");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CLevel_ConstructionSite2F::Free()
{
	__super::Free();
}
