#include "stdafx.h"
#include "Level_Batch.h"
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "JsonLib.h"
#include "Texture.h"
#include "Imgui_PropertyEditor.h"
#include "JsonStorage.h"
#include "GameUtils.h" 
#include "Material.h"
#include "SkyBox.h"
#include "Batch.h"
#include "BronJon.h"
#include "BuddyLumi.h"
#include "FlowerLeg.h"
#include "SkummyPandou.h"
#include "SkummyPool.h"

CLevel_Batch::CLevel_Batch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Batch::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Batch(TEXT("Layer_Batch"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Batch::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_Batch::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT CLevel_Batch::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : BatchTool"));

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Lights()
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

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	{
		char szFileName[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str())));
	});

	//Batch
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_Batch", CBatch::Create(m_pDevice, m_pContext)));

	//Monster
	FAILED_CHECK(Push_Back_Prototype(L"Prototype_GameObject_BronJon", CBronJon::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(L"Prototype_GameObject_BuddyLumi", CBuddyLumi::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(L"Prototype_GameObject_FlowerLeg", CFlowerLeg::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(L"Prototype_GameObject_SkummyPandou", CSkummyPandou::Create(m_pDevice, m_pContext), MONSTER));
	FAILED_CHECK(Push_Back_Prototype(L"Prototype_GameObject_SkummyPool", CSkummyPool::Create(m_pDevice, m_pContext), MONSTER));

	//SkyBox
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_NOW, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/StaticModel/Sky/SkySphere.static_model"))))
		return E_FAIL;

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// For_SkySphere
	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_Env", TEXT("Prototype_GameObject_SkyBox")));

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag.c_str(), TEXT("Prototype_GameObject_Camera_Dynamic")));
	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	return S_OK;
}

HRESULT CLevel_Batch::Ready_Layer_Batch(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json;
	json["ProtosInfo"] = Json::array();

	for (auto& info : m_ProtosInfo)
		json["ProtosInfo"].emplace_back(info);

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Batch"), &json));
	return S_OK;
}

HRESULT CLevel_Batch::Ready_Layer_Map(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/TestMap.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));
	return S_OK;
}

HRESULT CLevel_Batch::Push_Back_Prototype(const _tchar * pLayerTag, CGameObject * pGameObject, BATCHTYPE eType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, pLayerTag, pGameObject));
	m_ProtosInfo.emplace_back(pLayerTag, eType);

	return S_OK;
}

CLevel_Batch* CLevel_Batch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Batch*	pInstance = new CLevel_Batch(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Batch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Batch::Free()
{
	CLevel::Free();
}
