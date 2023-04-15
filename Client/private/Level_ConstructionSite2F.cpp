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

CLevel_ConstructionSite2F::CLevel_ConstructionSite2F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_ConstructionSite2F::Initialize()
{
   // m_bPlayerSpawn = false;

    m_strLevelName = L"ConstructionSite2F";
    //m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/ConstructionSite2F_ShadowCam.json"; // 아직 없음
    m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_ConstructionSite2F.json";

    if (FAILED(__super::Initialize()))
        return E_FAIL;
    if (FAILED(Ready_Layer_AI(LAYER_AI)))
        return E_FAIL;

    // Kinetic Object Batch & Item
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Kinetic_Normal_ConstructionSite2F.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Item_Batch.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Kinetic_Special_2F.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/Flower.json");

    // Monster Trigger(with Invisible Wall)
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Trigger_Monster1stWave.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Trigger_Monster2ndWave_Fix.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Trigger_Monster3rdWave.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Trigger_Monster4thWaveBoss.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/Story/ConstructionSite2F.json");

    // Test Code AchieveItem Test
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/AchieveTest.json");
    
    // Red String Batch
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedString_1.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedString_2.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedString_3.json");
    CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/RedString_4.json");

    // Boss Area Kinetic Object Preset(Recycle)
    Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/ConstructionSite2F/Kinetic_Normal_Construction2FBossRoom.json");
    CMap_KineticBatchPreset::GetInstance()->Initialize(kineticJson);

    CGameManager::SetGameManager(CGameManager::Create(m_pDevice, m_pContext));

    return S_OK;
}

void CLevel_ConstructionSite2F::Tick(_double TimeDelta)
{
    CMap_KineticBatchPreset::GetInstance()->Tick(TimeDelta);
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
    CLevel::Free();
}
