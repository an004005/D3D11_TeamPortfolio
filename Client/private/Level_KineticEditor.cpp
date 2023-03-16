#include "stdafx.h"
#include "..\public\Level_KineticEditor.h"
#include <Imgui_AnimModifier.h>
#include "GameInstance.h"
#include "Controller.h"
#include "Floors.h"
#include "JsonStorage.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "Imgui_PhysX.h"


CLevel_KineticEditor::CLevel_KineticEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_KineticEditor::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PhysX::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_KineticEditor::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : Kinetic Editor"));

	return CLevel::Render();
}

HRESULT CLevel_KineticEditor::Ready_Lights()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_KineticEditor::Ready_Prototypes()
{
	return S_OK;
}

HRESULT CLevel_KineticEditor::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Tutorial.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));

	// 여기서 모델만 넣고 콜라이더 셋팅 후 json 저장, json을 맵툴에서 로드해서 사용
	Json Test;// = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/KineticPreset/Normal_Car1.json");
	vector<string> modeltags;

	modeltags.push_back("../Bin/Resources/Model/StaticModel/MapStaicModels/Kinetic/GarbageCan/garbageCan.static_model");

	Test["ModelTags"] = modeltags;

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_MapKinetic_Object"), &Test));

	return S_OK;
}

HRESULT CLevel_KineticEditor::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");
	//
	// if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	// 	return E_FAIL;


	return S_OK;
}

CLevel_KineticEditor* CLevel_KineticEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_KineticEditor*	pInstance = new CLevel_KineticEditor(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_KineticEditor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_KineticEditor::Free()
{
	CLevel::Free();
}
