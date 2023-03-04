#include "stdafx.h"
#include "Level_UI.h"
#include "GameInstance.h"

#include "Controller.h"
#include "JsonStorage.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "GameUtils.h"

#include "Camera.h"

#include "Canvas.h"
#include "Canvas_Item.h"
#include "Canvas_SASInfo.h"
#include "Canvas_PlayerInfo.h"
#include "Canvas_PlayerInfoMove.h"
#include "Canvas_Drive.h"
#include "Canvas_DriveMove.h"
#include "Canvas_SASSkill.h"
#include "Canvas_SASSkillMove.h"
#include "Canvas_ItemMove.h"

#include "DefaultUI.h"
#include "ButtonUI.h"
// SAS SKill
#include "SASSkillIconUI.h"
#include "SASSkillGaugeUI.h"
#include "SASSkillLightUI.h"
#include "SASSkillDefaultUI.h"
#include "SASSkillFullCircleUI.h"
// Direve
#include "Drive_RightDotUI.h"
#include "Drive_GaugeUI.h"
// Item
#include "Item_NameUI.h"
#include "Item_IconUI.h"
#include "Item_GaugeUI.h"
#include "Item_PushArrowUI.h"
#include "Item_LeftArrowUI.h"
#include "Item_RightArrowUI.h"
// PlayerInfo
#include "PlayerInfo_HpUI.h"
#include "PlayerInfo_HpBackUI.h"
#include "PlayerInfo_HpBothEndsUI.h"

CLevel_UI::CLevel_UI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_UI::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));


	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	return S_OK;
}

void CLevel_UI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CLevel_UI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_UI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : UI"));

	return S_OK;
}

HRESULT CLevel_UI::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	{
		// Canvas
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas"),
			CCanvas::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		// Canvas_Frount_UI
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Item"),
			CCanvas_Item::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_ItemMove */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_ItemMove"),
			CCanvas_ItemMove::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfo"),
			CCanvas_SASInfo::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_PlayerInfo"),
			CCanvas_PlayerInfo::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_PlayerInfoMove"),
			CCanvas_PlayerInfoMove::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Drive"),
			CCanvas_Drive::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_DriveMove"),
			CCanvas_DriveMove::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASSkill"),
			CCanvas_SASSkill::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SASSkillMove */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASSkillMove"),
			CCanvas_SASSkillMove::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	{
		// UI
		/* For.Prototype_GameObject_Default_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Default_UI"),
			CDefaultUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Button_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Button_UI"),
			CButtonUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		// Frount_UI -> SAS Skill
		/* For.Prototype_GameObject_SASSkillIcon_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillIcon_UI"),
			CSASSkillIconUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_SASSkillGauge_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillGauge_UI"),
			CSASSkillGaugeUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_SASSkillLight_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillLight_UI"),
			CSASSkillLightUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_SASSkillCtrl_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillDefault_UI"),
			CSASSkillDefaultUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_SASSkillFullCircle_UI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASSkillFullCircle_UI"),
			CSASSkillFullCircleUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		// Frount_UI -> Drive
		/* For.Prototype_GameObject_Drive_RightDotUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Drive_RightDotUI"),
			CDrive_RightDotUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Drive_RightDotUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Drive_GaugeUI"),
			CDrive_GaugeUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		// Frount_UI -> Item
		/* For.Prototype_GameObject_Item_GaugeUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_GaugeUI"),
			CItem_GaugeUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_IconUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_IconUI"),
			CItem_IconUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_NameUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_NameUI"),
			CItem_NameUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_PushArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_PushArrowUI"),
			CItem_PushArrowUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_LeftArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_LeftArrowUI"),
			CItem_LeftArrowUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Item_RightArrowUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Item_RightArrowUI"),
			CItem_RightArrowUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		// Frount_UI -> PlayerInfo
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_HpUI"),
			CPlayerInfo_HpUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_HpBackUI"),
			CPlayerInfo_HpBackUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_HpBothEndsUI"),
			CPlayerInfo_HpBothEndsUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	// 	return E_FAIL;
	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	// const Json& json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/ShadowCam.json");
	// auto pCam = pGameInstance->Clone_GameObject_Get(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), (void*)&json);
	// pGameInstance->SetShadowCam(dynamic_cast<CCamera*>(pCam));

	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Frount_UI Canvas_FrontUI 
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Item.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Item", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ItemMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_ItemMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfo_Left.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfo", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_PlayerInfo.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_PlayerInfo", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_PlayerInfoMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_PlayerInfoMove", &json));
	
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Drive.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Drive", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_DriveMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_DriveMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfo_Right.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfo", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASSkill.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(L"Canvas_SASSkill", L"Canvas_SASSkill", &json));		// 움직이지 않은 UI

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASSkillMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASSkillMove", &json));	// 움직이는 UI
	
	//CGameUtils::ListFilesRecursive("../Bin/Resources/Objects/UI/", [&](const string& filePath)
	//{
	//	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(filePath);
	//	wstring protoTag = s2ws(json["Prototype_GameObject"]);
	//	pGameInstance->Clone_GameObject(pLayerTag, protoTag.c_str(), &json);
	//});

	return S_OK;
}

CLevel_UI * CLevel_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_UI*		pInstance = new CLevel_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_UI::Free()
{
	__super::Free();
}

