#include "stdafx.h"
#include "..\public\Level_Effect.h"

#include "EffectGroup.h"
#include "EffectSystem.h"
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "JsonLib.h"
#include "Imgui_PropertyEditor.h"
#include "JsonStorage.h"
#include "MaterialPreview.h"
#include "GameUtils.h"
#include "Material.h"
#include "Imgui_PostProcess.h"
#include "ParticleSystem.h"
#include "TrailSystem.h"


CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Effect::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer(TEXT("Layer"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Effect::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_Effect::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT CLevel_Effect::Render()
{
	FAILED_CHECK(__super::Render());
	SetWindowText(g_hWnd, TEXT("Level : Effecttool"));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Lights()
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

HRESULT CLevel_Effect::Ready_Prototypes()
{
	auto pGameInstance = CGameInstance::GetInstance();

	// CGameUtils::ListFiles("../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/", [this](const string& filePath)
	// {
	// 	string fileName = CGameUtils::GetFileName(filePath);
	// 	CGameInstance::GetInstance()->Add_Prototype(
	// 		s2ws(fileName).c_str(),
	// 		CModel::Create(m_pDevice, m_pContext, filePath.c_str()));
	// });


	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	{
		char szFileName[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	});

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectSystem", CEffectSystem::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectGroup", CEffectGroup::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_TrailSystem", CTrailSystem::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_ParticleSystem", CParticleSystem::Create(m_pDevice, m_pContext)));

	// 모델 추가하는 방법
	// auto pModel_VFX = CModel::Create(m_pDevice, m_pContext,
	// 	"../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/Default_Attack_1.static_model");
	// // 프로토타입 추가 방법
	// FAILED_CHECK(pGameInstance->Add_Prototype(L"VFX_Model_Default_Attack_01", pModel_VFX));
	//
	// pModel_VFX = CModel::Create(m_pDevice, m_pContext,
	// 	"../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/Default_Attack_2.static_model");
	// FAILED_CHECK(pGameInstance->Add_Prototype(L"VFX_Model_Default_Attack_02", pModel_VFX));
	//
	// pModel_VFX = CModel::Create(m_pDevice, m_pContext,
	// 	"../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/Default_Attack_3.static_model");
	// FAILED_CHECK(pGameInstance->Add_Prototype(L"VFX_Model_Default_Attack_03", pModel_VFX));
	//
	// pModel_VFX = CModel::Create(m_pDevice, m_pContext,
	// 	"../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/Default_Attack_4.static_model");
	// FAILED_CHECK(pGameInstance->Add_Prototype(L"VFX_Model_Default_Attack_04", pModel_VFX));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic")));

	// FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_EffectSys", TEXT("ProtoVFX_EffectSystem")));

	// Json AttackMesh = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/EffectSystem/Player_Default_Attack/Air_Attack_1.json");
	// pGameInstance->Clone_GameObject(L"Layer_EffectSys", L"ProtoVFX_EffectSystem", &AttackMesh);

	// FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_EffectSys", TEXT("ProtoVFX_EffectGroup")));

	Json AttackMesh = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Curve/SaveTest.json");
	pGameInstance->Clone_GameObject(L"Layer_EffectSys", L"ProtoVFX_EffectGroup", &AttackMesh);

	// FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_EffectSys", TEXT("ProtoVFX_TrailSystem")));
	// FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_EffectSys", TEXT("ProtoVFX_ParticleSystem")));

	// FAILED_CHECK(pGameInstance->Clone_GameObject(L"Preview", L"MaterialPreview"));

	return S_OK;
}

CLevel_Effect* CLevel_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Effect*	pInstance = new CLevel_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Effect::Free()
{
	CLevel::Free();
}
