#include "stdafx.h"
#include "Level_Maptool.h"
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "Floors.h"
#include "Navigation.h"
#include "Indicator.h"
#include "JsonLib.h"

CLevel_Maptool::CLevel_Maptool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Maptool::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_MapEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));

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

	// CGameUtils::ListFilesRecursive("../Bin/Resources/Meshes/Valorant/Materials/", [this](const string& fileName)
	// {
	// 	char szFileName[MAX_PATH]{};
	// 	_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	// 	CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	// });


	/* For.Prototype_GameObject_Floors */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Floors"),
		CFloors::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Indicator */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Indicator"),
		CIndicator::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Layer_Terrain(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Maptool::Ready_Layer_Player(const _tchar* pLayerTag)
{


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
