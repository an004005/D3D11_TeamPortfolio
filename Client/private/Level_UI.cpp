#include "stdafx.h"
#include "Level_UI.h"
#include "GameInstance.h"

#include "Controller.h"
#include "JsonStorage.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "GameUtils.h"

#include "Material.h"
#include "Camera.h"

#include "Canvas.h"
#include "Canvas_Item.h"
#include "Canvas_SASInfoLeft.h"
#include "Canvas_SASInfoLeftMove.h"
#include "Canvas_SASInfoRight.h"
#include "Canvas_SASInfoRightMove.h"
#include "Canvas_PlayerInfo.h"
#include "Canvas_PlayerInfoMove.h"
#include "Canvas_Drive.h"
#include "Canvas_DriveMove.h"
#include "Canvas_SASSkill.h"
#include "Canvas_SASSkillMove.h"
#include "Canvas_ItemMove.h"
#include "Canvas_Tutorial.h"

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
#include "PlayerInfo_PsychokinesisUI.h"
#include "PlayerInfo_PsychokinesisBackUI.h"
#include "PlayerInfo_GaugeBackGround.h"
// SASInfo Left
#include "SASInfoLeftHpUI.h"
#include "SASInfoLeftHpBackUI.h"
#include "SASInfoLeftHpBothEndsUI.h"
#include "SASInfoRightHpUI.h"
#include "SASInfoRightHpBackUI.h"
#include "SASInfoRightHpBothEndsUI.h"
// Tutorial
#include "TutorialUI.h"
#include "Tutorial_CheckUI.h"
#include "Tutorial_YesNoUI.h"
#include "Tutorial_TipsUI.h"
#include "Tutorial_SuccessUI.h"

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

	//if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
	//	return E_FAIL;

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

	CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	{
		char szFileName[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str())));
	});

	{
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
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfoLeft"),
			CCanvas_SASInfoLeft::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfoLeftMove"),
			CCanvas_SASInfoLeftMove::Create(m_pDevice, m_pContext))))
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
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfoRight"),
			CCanvas_SASInfoRight::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASInfoRightMove"),
			CCanvas_SASInfoRightMove::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		/* For.Prototype_GameObject_Canvas_PlayerInfo */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASSkill"),
			CCanvas_SASSkill::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Canvas_SASSkillMove */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_SASSkillMove"),
			CCanvas_SASSkillMove::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	
		/* For.Prototype_GameObject_Canvas_Tutorial*/
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Canvas_Tutorial"),
			CCanvas_Tutorial::Create(m_pDevice, m_pContext))))
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
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_PsychokinesisUI"),
			CPlayerInfo_PsychokinesisUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_PsychokinesisBackUI"),
			CPlayerInfo_PsychokinesisBackUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_PsychokinesisBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("PlayerInfo_GaugeBackGround"),
			CPlayerInfo_GaugeBackGround::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		// Frount_UI -> SASInfoLeft
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoLeft_HpUI"),
			CSASInfoLeftHpUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoLeft_HpBackUI"),
			CSASInfoLeftHpBackUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoLeft_HpBothEndsUI"),
			CSASInfoLeftHpBothEndsUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		// Frount_UI -> SASInfoRight
		/* For.Prototype_GameObject_PlayerInfo_HpUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoRight_HpUI"),
			CSASInfoRightHpUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBackUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoRight_HpBackUI"),
			CSASInfoRightHpBackUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_PlayerInfo_HpBothEndsUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("SASInfoRight_HpBothEndsUI"),
			CSASInfoRightHpBothEndsUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		// Tutorial
		/* For.Prototype_GameObject_TutorialUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("TutorialUI"),
			CTutorialUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_CheckUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Tutorial_CheckUI"),
			CTutorial_CheckUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_YesNoUI */ 
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Tutorial_YesNoUI"),
			CTutorial_YesNoUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_TipsUI */ 
			if (FAILED(pGameInstance->Add_Prototype(TEXT("Tutorial_TipsUI"),
				CTutorial_TipsUI::Create(m_pDevice, m_pContext))))
				return E_FAIL;
		/* For.Prototype_GameObject_Tutorial_SuccessUI */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Tutorial_SuccessUI"),
			CTutorial_SuccessUI::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}	

	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Camera(const _tchar * pLayerTag)
{
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

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfoLeft.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfoLeft", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfoLeftMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfoLeftMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_PlayerInfo.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_PlayerInfo", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_PlayerInfoMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_PlayerInfoMove", &json));
	
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Drive.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Drive", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_DriveMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_DriveMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfoRight.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfoRight", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASInfoRightMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASInfoRightMove", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASSkill.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASSkill", &json));		// 움직이지 않은 UI

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SASSkillMove.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_SASSkillMove", &json));	// 움직이는 UI
	
	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Tutorial.json");
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, L"Canvas_Tutorial", &json));

	//CGameUtils::ListFilesRecursive("../Bin/Resources/Objects/UI/", [&](const string& filePath)
	//{
	//	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(filePath);
	//	wstring protoTag = s2ws(json["Prototype_GameObject"]);
	//	pGameInstance->Clone_GameObject(pLayerTag, protoTag.c_str(), &json);
	//});

	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Tutorial.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));
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

