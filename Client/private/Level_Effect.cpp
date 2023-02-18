#include "stdafx.h"
#include "..\public\Level_Effect.h"
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "JsonLib.h"
#include "Imgui_PropertyEditor.h"
#include "JsonStorage.h"
#include "Trail.h"
#include "MaterialPreview.h"
#include "GameUtils.h"
#include "Material.h"
#include "Imgui_PostProcess.h"
#include "ParticleSystem.h"


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
	if (FAILED(__super::Render()))
		return E_FAIL;

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

	//CGameUtils::ListFilesRecursive("../Bin/Resources/Meshes/Valorant/Materials/", [this](const string& filePath)
	//{
	//	string fileName = CGameUtils::GetFileName(filePath);
	//	CGameInstance::GetInstance()->Add_Prototype(
	//		s2ws(fileName).c_str(),
	//		CMaterial::Create(m_pDevice, m_pContext, filePath.c_str()));
	//});

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();


	pGameInstance->Clone_GameObject(L"Preview", L"MaterialPreview");

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
