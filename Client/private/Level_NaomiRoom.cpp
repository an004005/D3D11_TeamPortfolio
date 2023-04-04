#include "stdafx.h"
#include "..\public\Level_NaomiRoom.h"
#include "GameInstance.h"
#include "GameManager_Tutorial.h"
#include "Imgui_Batch.h"


CLevel_NaomiRoom::CLevel_NaomiRoom(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_NaomiRoom::Initialize()
{
	// m_bPlayerSpawn = false;

	m_strLevelName = L"NaomiRoom";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/NaomiRoom_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_NaomiRoom.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Layer_AI(LAYER_AI)))
		return E_FAIL;

	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Naomi_SpeicialContainer.json");
	CImgui_Batch::RunBatchFile("../Bin/Resources/Batch/BatchFiles/NaomiRoom/Player_AI_Pos.json");


	return S_OK;
}

CLevel_NaomiRoom * CLevel_NaomiRoom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_NaomiRoom*		pInstance = new CLevel_NaomiRoom(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_NaomiRoom");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_NaomiRoom::Free()
{
	__super::Free();

}
