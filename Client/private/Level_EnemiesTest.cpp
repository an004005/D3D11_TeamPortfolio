#include "stdafx.h"
#include "Level_EnemiesTest.h"
#include "GameInstance.h"
#include "Material.h"
#include "GameUtils.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "Imgui_LevelSwitcher.h"

// AJH's Initial Setting
#include "Controller.h"
#include "Floors.h"
#include "Navigation.h"
#include "PostVFX_WhiteOut.h"
#include "JsonStorage.h"
#include "Camera.h"

// Object
#include "TestMonster.h"
#include "FlowerLeg.h"

CLevel_EnemiesTest::CLevel_EnemiesTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_EnemiesTest::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	Ready_Layer_Effect();

	return S_OK;
}

void CLevel_EnemiesTest::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CLevel_EnemiesTest::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_EnemiesTest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : EnemiesTest"));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Lights()
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

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	{
		char szFileName[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	});

	/* Controller */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"),
		CController::Create())))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_DummyController"),
		CDummyController::Create())))
		return E_FAIL;

	// 게임오브젝트 로딩
	{
		/* For.Prototype_GameObject_Floors */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Floors"),
			CFloors::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Navigation */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Navigation"),
			CNavigation::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/*for ProtoPostVFX_WhiteOut */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_WhiteOut"),
			CPostVFX_WhiteOut::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	}

	{														// Bin\Resources\Model\AnimModel\Monster\Goat
		auto pGoat = CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/AnimModel/Monster/Goat/Goat.anim_model",
			_float4x4::CreateScale({ 0.1f, 0.1f, 0.1f }));
		pGoat->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/Goat/Anim/"); // Bin\Resources\Model\AnimModel\Monster\Goat\Anim
		pGameInstance->Add_Prototype(TEXT("TestMonsterGoat"), pGoat);
	}
	
	{														
		auto pFlowerLeg = CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/FlowerLeg.anim_model",
			_float4x4::CreateScale({ 0.1f, 0.1f, 0.1f }));
		pFlowerLeg->LoadAnimations("../Bin/Resources/Model/AnimModel/Monster/FlowerLeg/Anim/"); 
		pGameInstance->Add_Prototype(TEXT("MonsterFlowerLeg"), pFlowerLeg);
	}
	
	pGameInstance->Add_Prototype(TEXT("TestMonster"), CTestMonster::Create(m_pDevice, m_pContext));

	pGameInstance->Add_Prototype(TEXT("FlowerLeg"), CFlowerLeg::Create(m_pDevice, m_pContext));
		
	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	const Json& json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/ShadowCam.json");
	auto pCam = pGameInstance->Clone_GameObject_Get(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), (void*)&json);
	pGameInstance->SetShadowCam(dynamic_cast<CCamera*>(pCam));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("TestMonster"))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("FlowerLeg"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Effect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	return S_OK;
}

CLevel_EnemiesTest * CLevel_EnemiesTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_EnemiesTest*		pInstance = new CLevel_EnemiesTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EnemiesTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_EnemiesTest::Free()
{
	__super::Free();
}
