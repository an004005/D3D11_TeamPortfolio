#include "stdafx.h"
#include "Level_GamePlay.h"

#include <Imgui_AnimModifier.h>

#include "GameInstance.h"
#include "Material.h"
#include "Controller.h"
#include "Floors.h"
#include "Navigation.h"
#include "SkyBox.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Material.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "PostVFX_WhiteOut.h"
#include "Camera.h"
#include "Sound.h"

#include "TestMonster.h"
#include "TestCamera.h"
#include "Boss1.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	Ready_Layer_Effect();

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CLevel_GamePlay::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : GAMEPLAY"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));


	//ZeroMemory(&LightDesc, sizeof LightDesc);

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.isEnable = true;
	///*LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);*/
	//LightDesc.vPosition = _float4(5.f, 3.f, 5.f, 1.f);
	//LightDesc.fRange = 10.0f;
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();


	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	{
		char szFileName[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	});

	pGameInstance->Add_Prototype(L"ModelPreview", CModelPreviwer::Create(m_pDevice, m_pContext));


	{
		auto pBoss1 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Model/AnimModel/Monster/boss1_em320/boss_1.anim_model");
		pBoss1->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/boss1_em320/Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(TEXT("MonsterBoss1"), pBoss1));
	}

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"), CController::Create())))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_TestCamera"), CTestCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_MonsterBoss1"), CBoss1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//// 23.02.20 PJW Work

	//{														// Bin\Resources\Model\AnimModel\Monster\Goat
	//	auto pGoat = CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/AnimModel/Monster/Goat/Goat.anim_model",
	//		_float4x4::CreateScale({ 0.1f, 0.1f, 0.1f }));
	//	pGoat->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/Goat/Anim/"); // Bin\Resources\Model\AnimModel\Monster\Goat\Anim
	//	pGameInstance->Add_Prototype(TEXT("TestMonsterGoat"), pGoat);
	//}

	//{														// \Bin\Resources\Model\AnimModel\Monster\FlowerLeg
	//	auto pFlowerLeg = CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/FlowerLeg.anim_model",
	//		_float4x4::CreateScale({ 0.1f, 0.1f, 0.1f }));
	//	pFlowerLeg->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/Anim/"); // \Bin\Resources\Model\AnimModel\Monster\FlowerLeg\Anim
	//	pGameInstance->Add_Prototype(TEXT("MonsterFlowerLeg"), pFlowerLeg);
	//}

	//pGameInstance->Add_Prototype(TEXT("TestMonster"), CTestMonster::Create(m_pDevice, m_pContext));

	//// Model Component Origin Create

	//// ~23.02.20 PJW Work

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();



	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/TestMap.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");


	//
	// const Json& json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/ShadowCam.json");
	// auto pCam = pGameInstance->Clone_GameObject_Get(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), (void*)&json);
	// pGameInstance->SetShadowCam(dynamic_cast<CCamera*>(pCam));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Json PreviewData;
	// PreviewData["Model"] = "MonsterBoss1";
	// auto pBoss = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("ModelPreview"), &PreviewData);
	pGameInstance->Clone_GameObject(pLayerTag, L"Prototype_MonsterBoss1");

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}