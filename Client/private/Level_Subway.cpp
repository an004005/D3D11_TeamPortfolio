#include "stdafx.h"
#include "..\public\Level_Subway.h"
#include "GameInstance.h"
#include "GameManager_Tutorial.h"
#include "Imgui_Batch.h"


CLevel_Subway::CLevel_Subway(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel_StageDefault(pDevice, pContext)
{
}

HRESULT CLevel_Subway::Initialize()
{
	m_strLevelName = L"Subway";
	m_strShadowCamJsonPath.clear();
	m_strMapJsonPath = "../Bin/Resources/Objects/Map/Map_Subway.json";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
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
