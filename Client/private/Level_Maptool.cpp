#include "stdafx.h"
#include "Level_Maptool.h"
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "JsonLib.h"
#include "Texture.h"
#include "Imgui_PropertyEditor.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Material.h"
#include "Model_Instancing.h"
#include "SkyBox.h"

CLevel_Maptool::CLevel_Maptool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Maptool::Initialize()
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

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Maptool::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_Maptool::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT CLevel_Maptool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : MapTool"));

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Lights()
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

HRESULT CLevel_Maptool::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	// {
	// 	char szFileName[MAX_PATH]{};
	// 	_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	// 	FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str())));
	// });

	/* 
	모델 프로토 타입은 굳이 생성 안해도 자동으로 됨.
	리스트박스에 넣으려고 툴에서만 생성해줌.
	그래서 다른 레벨에서는 아래 모델 프로토 타입 생성을 넘겨도 괜찮음.
	*/
	
	//일반 모델들의 프로토타입 생성
	CGameUtils::ListFilesRecursive("../Bin/Resources/Model/StaticModel/MapStaicModels/Default/",
		[this](const string& fileName)
	{
		char szFileExt[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szFileExt, MAX_PATH);

		if (0 == strcmp(szFileExt, ".static_model"))
		{
			FAILED_CHECK(Create_Model(s2ws(fileName), fileName.c_str(), NON_INSTANCE));
		}
	});


	//인스턴싱 모델들의 프로토타입 생성
	CGameUtils::ListFilesRecursive("../Bin/Resources/Model/StaticModel/MapStaicModels/Instancing/",
		[this](const string& fileName)
	{
		char szFileExt[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szFileExt, MAX_PATH);

		if (0 == strcmp(szFileExt, ".static_model"))
		{
			FAILED_CHECK(Create_Model_Instance(s2ws(fileName), fileName.c_str()));
		}
	});

	//키네틱 모델들의 프로토타입 생성
	CGameUtils::ListFilesRecursive("../Bin/Resources/Model/StaticModel/MapStaicModels/Kinetic/",
		[this](const string& fileName)
	{
		char szFileExt[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szFileExt, MAX_PATH);

		if (0 == strcmp(szFileExt, ".static_model"))
		{
			FAILED_CHECK(Create_Model(s2ws(fileName), fileName.c_str(), KINETIC));
		}
	});




	//SkyBox
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_NOW, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/StaticModel/Sky/SkySphere.static_model"))))
		return E_FAIL;

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Layer_BackGround(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// For_SkySphere
	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_Env", TEXT("Prototype_GameObject_SkyBox")));

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Layer_Camera(const wstring& pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag.c_str(), TEXT("Prototype_GameObject_Camera_Dynamic")));
	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Layer_Map(const wstring& pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/DownTown.json");

	json["Model_ProtoTypes"] = Json::array();

	for (auto& Proto : m_pProtosTags)
	{	
		json["Model_ProtoTypes"].push_back({ ws2s(Proto.first), Proto.second });
	}
	
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag.c_str(), TEXT("Prototype_GameObject_ScarletMap"), &json));
	return S_OK;
}


HRESULT CLevel_Maptool::Create_Model(const wstring& pProtoTag, const char* pModelPath, PROTOINFO eProtoInfo)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CComponent* pComponent = nullptr;

	pComponent = CModel::Create(m_pDevice, m_pContext, pModelPath);
	assert(pComponent != nullptr);

	FAILED_CHECK(pGameInstance->Add_Prototype(
		pProtoTag.c_str(),
		pComponent));

	m_pProtosTags.emplace_back(pProtoTag, eProtoInfo);

	return S_OK;
}

HRESULT CLevel_Maptool::Create_Model_Instance(const wstring & pProtoTag, const char * pModelPath)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CComponent* pComponent = nullptr;
	
	_uint iNumInstance = 500;
	pComponent = CModel_Instancing::Create(m_pDevice, m_pContext, pModelPath, iNumInstance);
	assert(pComponent != nullptr);

	FAILED_CHECK(pGameInstance->Add_Prototype(
		pProtoTag.c_str(),
		pComponent));

	m_pProtosTags.emplace_back(pProtoTag, INSTANCE);

	return S_OK;
}

CLevel_Maptool* CLevel_Maptool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Maptool*	pInstance = new CLevel_Maptool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Maptool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Maptool::Free()
{
	CLevel::Free();
}
