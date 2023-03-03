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

	FAILED_CHECK(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	{
		char szFileName[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	});

	/* For. big_building */
	FAILED_CHECK(Create_Model(TEXT("../Bin/Resources/Model/StaticModel/Buildings/big_building.static_model"), "../Bin/Resources/Model/StaticModel/Buildings/big_building.static_model"));

	/* For. MID_buildbase_M_01 */
	FAILED_CHECK(Create_Model(TEXT("../Bin/Resources/Model/StaticModel/Buildings/MID_buildbase_M_01.static_model"), "../Bin/Resources/Model/StaticModel/Buildings/MID_buildbase_M_01.static_model"));

	/* For. LC01_Area02_Minimap */
	FAILED_CHECK(Create_Model(TEXT("../Bin/Resources/Model/StaticModel/Minimaps/LC01_Area02_Minimap.static_model"), "../Bin/Resources/Model/StaticModel/Minimaps/LC01_Area02_Minimap.static_model"));

	/* For. HWBase_B02 */
	FAILED_CHECK(Create_Model(TEXT("../Bin/Resources/Model/StaticModel/Roads/HWBase_B02.static_model"), "../Bin/Resources/Model/StaticModel/Roads/HWBase_B02.static_model"));

	/* For. HWEndZ_tes02 */
	FAILED_CHECK(Create_Model(TEXT("../Bin/Resources/Model/StaticModel/Roads/HWEndZ_tes02.static_model"), "../Bin/Resources/Model/StaticModel/Roads/HWEndZ_tes02.static_model"));

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Layer_Camera(const wstring& pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag.c_str(), TEXT("Prototype_GameObject_Camera_Dynamic")));

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Layer_Map(const wstring& pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/HWBase.json");

	//TODO: 프로토 테그만 저장하고 로드할때 find로 찾아야함
	for (auto& Proto : m_pProtosTags)
	{	
		json["ProtoTags"].push_back(ws2s(Proto));
	}
	
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag.c_str(), TEXT("Prototype_GameObject_ScarletMap"), &json));
	return S_OK;
}


HRESULT CLevel_Maptool::Create_Model(const wstring& pProtoTag, const char* pModelPath)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CComponent* pComponent = nullptr;

	pComponent = CModel::Create(m_pDevice, m_pContext, pModelPath);
	assert(pComponent != nullptr);

	FAILED_CHECK(pGameInstance->Add_Prototype(
		pProtoTag.c_str(),
		pComponent));

	m_pProtosTags.emplace_back(pProtoTag);

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
