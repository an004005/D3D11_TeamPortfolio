#include "stdafx.h"
#include "Level_PlayerTest.h"

#include <Material.h>
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "Model.h"
#include "JsonLib.h"
#include "Imgui_AnimModifier.h"
#include "GameUtils.h"
#include "AnimationInstance.h"
#include "Player.h"

CLevel_PlayerTest::CLevel_PlayerTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_PlayerTest::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;


	return S_OK;
}

void CLevel_PlayerTest::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_PlayerTest::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT CLevel_PlayerTest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : PlayerTest"));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	FAILED_CHECK(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& filePath)
	{
		string fileName = CGameUtils::GetFileName(filePath);
		CGameInstance::GetInstance()->Add_Prototype(
			s2ws(fileName).c_str(),
			CMaterial::Create(m_pDevice, m_pContext, filePath.c_str()));
	});

	pGameInstance->Add_Prototype(L"TestPlayer", CPlayer::Create(m_pDevice, m_pContext));

	auto pModel_TestPlayer = CModel::Create(m_pDevice, m_pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/TestPlayer/Test.anim_model");

	pModel_TestPlayer->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/TestPlayer/Animation/");
	FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_TestPlayer", pModel_TestPlayer));

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_PlayerTest::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json PreviewData;
	PreviewData["Model"] = "Model_TestPlayer";

	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("TestPlayer"), &PreviewData)))
		return E_FAIL;

	return S_OK;
}

CLevel_PlayerTest* CLevel_PlayerTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_PlayerTest*	pInstance = new CLevel_PlayerTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_PlayerTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_PlayerTest::Free()
{
	CLevel::Free();
}
