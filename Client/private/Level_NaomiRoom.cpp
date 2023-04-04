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
	m_strLevelName = L"Subway";
	m_strShadowCamJsonPath = "../Bin/Resources/Objects/ShadowCam/NaomiRoom_ShadowCam.json";
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_NaomiRoom.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

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
